
if (-Not (Test-Path -Path "./build"))
{
    cmake -B ./build
}

if (Test-Path -Path "./build")
{
    cmake --build ./build
}

