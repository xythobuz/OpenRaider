# To-Do List

## General

There are these DebugModel, DebugMap flags...?

* Endian dependence ugly, shouldn't dereference to different types?
    * TombRaider.h/cpp structs aren't aligned... unportable to some big endian & other archs?!
* Use more asserts
* Don't use C-Style code, try to replace with C++ lib
    * Use shared_ptr, should make memory handling/segfaults much less of a problem
        * Don't even new ... the data structures but use std::make_shared or allocate_shared?
        * Pass object references to all other objects that need it, completely remove gOpenRaider
    * Use streams for (file) I/O
        * Does not need strtok() anymore
    * Use std::strings
* SDL_TTF 2.0.12+ can do line breaks, use it: http://stackoverflow.com/questions/17847818/how-to-do-line-breaks-and-line-wrapping-with-sdl-ttf/18418688#18418688
* Rewrite Console using std::strings and use operator << to write to the console

## Changes

* Using std::vector with [] is not bound checked. Segfaults were caused because the upper bound of the argument was never checked and garbage was returned... Do consistent checks, or use .at() as it throws an exception
* The wrong SkeletalModels are used by entities, except for Lara...?

## Cmake

* Windows setup script needed, moving to AppData/Roaming
* Support SSE with other compilers than Clang (src/CMakeLists.txt)
* Visual C++ compiler flags? (CMakeLists.txt)

## Future Features

* Use only assets from old TR games?
* [PCX image reading](http://bespin.org/~qz/pc-gpe/pcx.txt) for eg. TR2 built-in menu background
    * Cut TGA image reader, currently only used for menu background?!
* When cutscene rendering is working, use TR4/5 style menu?

