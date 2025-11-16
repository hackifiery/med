# med (Minimal text EDitor) -- a very tiny text editor written in c++
A tiny text editor i wrote cuz others are too bloated imo.
## Features:
* Mostly functional, can edit text (obviously)
* Very lightweight, ~500kb
* `ncurses`-free

# Get the code
Go to the releases page and download the latest one.
For development (potentially unstable) versions, clone this repo:
```````
git clone https://github.com/hackifiery/med
```````

# Build/install
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

# Usage
``````
med [options] [filename]
``````
--version/-v: display the current version

Once in the editor, type ctrl-s to save, ctrl-x to quit.

# Known issues

med is still buggy, so there are some problems that you may encounter:

* ~~[fixed] No horizontal scrolling~~
* [partially fixed] Downloads from the release page do not display version correctly; building encounters non-fatal git errors
* ~~[fixed] Shell is not preserved after close~~

# TODO
* ~~[done] Scrolling~~
* ~~[done] Display row/column~~
* ~~[done] Home/End key support~~
* ~~[done] Display percentage~~
* ~~[done] Highlight titlebar~~
* ~~[done]Line numbering~~
* Fix bugs
