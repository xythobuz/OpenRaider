# To-Do List

## General

* Endian dependence ugly, shouldn't dereference to different types?
    * TombRaider.h/cpp structs aren't aligned... unportable to some big endian & other archs?!
* Use more asserts
* Don't use C-Style code, try to replace with C++ lib
    * Use std::strings
    * Rewrite Console and use operator << to write to the console?
* SDL_TTF 2.0.12+ can do line breaks, use it: http://stackoverflow.com/questions/17847818/how-to-do-line-breaks-and-line-wrapping-with-sdl-ttf/18418688#18418688
* Mesh has 2 different approaches of storing the same data (eg. mColors and mColorArray), but half of ‘em isn’t implemented. Unify this, probably even reusing the Mesh class for the model_meshes...

## Changes

* Using std::vector with [] is not bound checked. Segfaults were caused because the upper bound of the argument was never checked and garbage was returned... Do consistent checks, or use .at() as it throws an exception

## Cmake

* Windows setup script needed, moving to AppData/Roaming
* Support SSE with other compilers than Clang (src/CMakeLists.txt)
* Visual C++ compiler flags? (CMakeLists.txt)

## Future Features

* Cut TGA image reader, currently only used for menu background?!
    * Abstract image reading/writing, like Font Engine selection
* When cutscene rendering is working, use TR4/5 style menu?

