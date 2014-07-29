# To-Do List

## General

* Move to newer OpenGL (GLES or 3.x or 4.x?)
* Endian dependence ugly, shouldn't dereference to different types?
    * TombRaider.h/cpp structs aren't aligned... unportable to some big endian & other archs?!
    * Maybe replace loader with [VT](http://icculus.org/vt/), also used by OpenTomb.
* Don't use C-Style code, try to replace with C++ lib
    * Use std::strings
    * Rewrite Console and use operator << to write to the console?
* Mesh has 2 different approaches of storing the same data (eg. mColors and mColorArray), but half of ‘em isn’t implemented. Unify this, probably even combining Mesh and StaticMesh...
* Don’t use float everywhere just because (eg. float colors)
* Add verbose command line flag for debug output also in release builds

## Cmake

* Support SSE with other compilers than Clang (src/CMakeLists.txt)
* Visual C++ compiler flags? (CMakeLists.txt)
* Better test integration

## Future Features

* Cut TGA image reader, currently only used for menu background?!
    * Need useful, always available image writer alternative for screenshots then
* When cutscene rendering is working, use TR4/5 style menu?

