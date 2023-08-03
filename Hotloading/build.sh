
if [ ! -d ./build ];
then
    cmake -B ./build
fi

if [ -d ./build ];
then
    cmake --build ./build
fi

