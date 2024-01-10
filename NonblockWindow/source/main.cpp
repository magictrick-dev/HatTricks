// --- Entry Point -------------------------------------------------------------
//
// This is the entry point for a Win32 application; generally, if it is marked as
// Win32 application, the entry point function changes from main to WinMain. As a
// consequence, you lose a hook to the system console since the expectation is that
// you won't be needing it for front-end GUI applications. (Which is true.)
//
// You can restore a hook to the console by reallocating one (by that, a new one
// that isn't the one you may have invoked the exe from), but I would advise just
// using OutputDebugStringA() intrinsic and Visual Studio should you need runtime
// debug output. The result is effectively the same.
//

#include <main.h>
#include <windows.h>

static DWORD    runtime_thread_id;
static bool     main_runtime_flag;

static LRESULT CALLBACK
wDisplayWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

static LRESULT CALLBACK
wGhostWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

// --- Runtime Thread ----------------------------------------------------------
//
// The runtime thread is our true execution point, not main. Main handles all
// message processes and other routines that we want to happen independently
// from our runtime code.
//

static DWORD WINAPI
wRuntimeThread(LPVOID user_param)
{

    // Our user parameter is our ghost window's handle.
    HWND ghost_window_handle = (HWND)user_param;

    // Create the window class as normal.
    WNDCLASSEXW display_window_class    = {};
    display_window_class.cbSize         = sizeof(display_window_class);
    display_window_class.lpfnWndProc    = &wDisplayWindowProc;
    display_window_class.hInstance      = GetModuleHandleW(NULL);
    display_window_class.hIcon          = LoadIconA(NULL, IDI_APPLICATION);
    display_window_class.hCursor        = LoadCursorA(NULL, IDC_ARROW);
    display_window_class.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    display_window_class.lpszClassName  = L"displayWindowClass";
    RegisterClassExW(&display_window_class);

    // Establish our creation context.
    win32_window_creation_context context   = {};
    context.ex_style            = 0;
    context.class_name          = display_window_class.lpszClassName;
    context.window_name         = L"Nonblocking Window Example";
    context.style               = WS_OVERLAPPEDWINDOW;
    context.x                   = CW_USEDEFAULT;
    context.y                   = CW_USEDEFAULT;
    context.width               = CW_USEDEFAULT;
    context.height              = CW_USEDEFAULT;
    context.instance            = display_window_class.hInstance;
    
    // Create the window.
    HWND display_window_handle = (HWND)SendMessageW(ghost_window_handle,
            UD_CREATE_WINDOW, (WPARAM)&context, 0);

    // The window is most likely created, so we can just pump the message loop
    // and inspect the messages that the display window procedure fed us.
    ShowWindow(display_window_handle, SW_SHOWNORMAL);

    static bool runtime_flag = true;
    while (runtime_flag)
    {

        // Here, we process all messages that the display window procedure
        // forwarded to us. This is now our catchall for any events we want
        // to process. We capture should_exit when we encounter WM_CLOSE, as it
        // is our signal to break the runtime loop.
        MSG current_message = {};
        bool should_exit = false;
        while(PeekMessage(&current_message, 0, 0, 0, PM_REMOVE))
        {
            
            switch(current_message.message)
            {
                case WM_CLOSE:
                {
                    SendMessageW(ghost_window_handle, UD_DESTROY_WINDOW, current_message.wParam, 0);
                    should_exit = true;
                    break;
                };  
            }

        }

        if (should_exit == true) break;

        // This is where you can do some runtime stuff, whatever that runtime
        // stuff may be. If you're writing game code, this is where the runtime
        // stuff would be placed.
        Sleep(1);

    }

    // If our runtime loop goes, we should stop our main thread as well.
    main_runtime_flag = false;
    ExitProcess(0);

}

// --- Main Thread -------------------------------------------------------------
//
// The main thread is wWinMain, which we delegate as our event handler thread for
// our created windows. The main thread assumes responsibility of creating our
// "ghost window" and then pumping the message loops.
//
// The main messages we care about from our ghost window is the UD_CREATE_WINDOW
// and UD_DESTROY_WINDOW messages.
//
// We also define the displaying window's procedure here, because this too also
// runs on the main thread. The messages that we receive from this window can be
// forwarded off to the message pump of the runtime thread as needed.
//

static LRESULT CALLBACK
wDisplayWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{

    LRESULT ret_result = 0;   

    // Essentially, our window procedure catches the messages and then forwards
    // them off as a thread message to our runtime thread, effectively making it
    // non-blocking since the message can be sent and allow the window procedure
    // to keep going.
    switch (message)
    {
        
        // For close, we simply just smuggle the window handle with w_param.
        case WM_CLOSE:
        {
            PostThreadMessageW(runtime_thread_id, message, (WPARAM)window, l_param);
            break;
        };

        // For all other messages that we want to process, we just send it over.
        case WM_DESTROY:
        {
            PostThreadMessageW(runtime_thread_id, message, w_param, l_param);
            break;
        };

        // Anything we don't care about is defaulted.
        default:
        {
            ret_result = DefWindowProcW(window, message, w_param, l_param);
        };

    };

    return ret_result;

}

static LRESULT CALLBACK
wGhostWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{

    LRESULT ret_result = 0;

    switch (message)
    {
        case UD_CREATE_WINDOW:
        {

            // Cast to the creation context and then run it.
            win32_window_creation_context *context = (win32_window_creation_context*)w_param;
            HWND window_handle = CreateWindowExW(context->ex_style, context->class_name,
                    context->window_name, context->style, context->x, context->y, context->width,
                    context->height, context->parent_window, context->menu, context->instance,
                    context->user_parameter);

            // The result can be packed as a LRESULT, which is kinda neat.
            ret_result = (LRESULT)window_handle;
            break;

        };

        case UD_DESTROY_WINDOW:
        {

            // A request to destroy the window is made.
            DestroyWindow((HWND)w_param);
            break;

        };

        default:
        {

            // Generally, any other event that the ghost window receives can
            // be defaulted. We don't really care about this window's existence.
            ret_result = DefWindowProcW(window, message, w_param, l_param);
            break;

        };
    };

    return ret_result;

}

int WINAPI
wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR command_line, int show_command)
{
    
    // We need to create our ghost window class.
    WNDCLASSEXW ghost_window_class    = {};
    ghost_window_class.cbSize         = sizeof(ghost_window_class);
    ghost_window_class.lpfnWndProc    = &wGhostWindowProc;
    ghost_window_class.hInstance      = GetModuleHandleW(NULL);
    ghost_window_class.hIcon          = LoadIconA(NULL, IDI_APPLICATION);
    ghost_window_class.hCursor        = LoadCursorA(NULL, IDC_ARROW);
    ghost_window_class.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    ghost_window_class.lpszClassName  = L"ghostWindowClass";
    RegisterClassExW(&ghost_window_class);

    // Our creation routine is fairly normal, except for the fact we don't specify
    // any styling; effectively making this window hidden. This is intentional, as
    // we only want this ghost window to capture messages for us.
    HWND ghost_window_handle = CreateWindowExW(0, ghost_window_class.lpszClassName,
            L"Ghost Window", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, ghost_window_class.hInstance, 0);

    // This is where we create the runtime thread; once this thread runs, it will
    // send off a message to our ghost window to create the real window. We assign
    // the user parameter to our ghost window's handle since the runtime thread will
    // need that to send messages to in order create windows. We also set the
    // global runtime_thread_id, since it is a critical identifier for our application.
    CreateThread(0, 0, wRuntimeThread, ghost_window_handle, 0, &runtime_thread_id);

    // Finally, we create a message pump for our main which handles all incoming events
    // for windows that we receive.
    main_runtime_flag = true;
    while(main_runtime_flag)
    {

        MSG current_message = {};
        GetMessageW(&current_message, 0, 0, 0);
        TranslateMessage(&current_message);

        // The messages we receive here come from any messages that the main thread
        // receives; we simply capture them and forward them off.
        if ((current_message.message == WM_CHAR) ||
            (current_message.message == WM_QUIT) ||
            (current_message.message == WM_SIZE))
        {
            PostThreadMessageW(runtime_thread_id, current_message.message,
                    current_message.wParam, current_message.lParam);
        }
        else
        {
            DispatchMessageW(&current_message);
        }

    }

    return 0;
}
