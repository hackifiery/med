# med (Minimal text EDitor) -- a very tiny text editor written in c++
A tiny text editor i wrote cuz vim nano and others are too bloated imo. The final executable is <500kb so yea

# build/install
med uses CMake, so to build it, just do
``````
cmake -B ./build -S .
cmake --build ./build
``````
The executable will be located inside `build/bin/`. To install, run
``````
cmake --install ./build
``````
as root, and it will install to `/usr/local/bin/`.

