
# Nonblocking Windows

This is a base project for Win32 projects that need to create a window to draw to,
except it fixes a very annoying behavior that resizing or moving the window causes
the application to loop-lock during those operations. There are a few ways to fix
this, but this method seems to work pretty great without having to rewrite the entire
resizing behavior from scratch so it doesn't loop-lock.

Props to [C. Muratori at Molly Rocket](https://github.com/cmuratori/dtc/tree/main)
for this clever little hack. My implementation is fairly similar, save for some
stripped down functionality and variable name changes.

### The Concept

We create to custom events, UD_CREATE_WINDOW and UD_DESTROY_WINDOW, which we manually
send to a "ghost window" that is invisible to the user. This "window" captures these
messages and create real windows since the window procedure for the ghost window.

When we create a window through this manner, we can create a seperate thread that
contains our true runtime code and then message the main thread to create a window.
When the main thread creates the window, it assumes responsibility of handling the events
of that window. For the events that we care about, we just forward them back to the
runtime thread through a thread message (which doesn't block, just informs) and we
can respond as normal.

### Implementation Concerns

Designing a runtime loop that doesn't spin forever on a single thread is annoying
enough with Win32, but now we have two threads. Even if the runtime thread exits
and the window closes, the main thread still needs to be informed that this happened.
The crude work around is creating a global variable which the runtime thread sets to
false just before it exists to inform the main thread that it should stop looping over
the message pump.

It's a bit annoying, but if you intend to architect a project, you will, at some point,
need a little flag to tell the application that it is running.

