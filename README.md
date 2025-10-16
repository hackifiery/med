# med (Minimal text EDitor) -- a very tiny text editor written in c++
A tiny text editor i wrote cuz others are too bloated imo. The final executable is <500kb so yea

# get the code
Go to the releases page and download the latest one.
For development (potentially unstable) versions, clone this repo:
```````
git clone https://github.com/hackifiery/med
```````

# build/install
med uses CMake, so to build it, just do
``````
cmake -B build -S .
cmake --build build
``````
The executable will be located inside `build/bin/`. To install, run
``````
cmake --install build
``````
as root, and it will install to `/usr/local/bin/`.

# usage
``````
med [filename]
``````
Once in the editor, type ctrl-s to save, ctrl-q to quit.
# TODO
* Scrolling
* Display row/column
* Line numbering (?)
