# To-Do List

## General

* Move to newer OpenGL (GL ES or 2.1 or 3.x or 4.x?)
* Endian dependence ugly, shouldn't dereference to different types?
    * TombRaider.h/cpp structs aren't aligned... unportable to some big endian & other archs?!
    * Maybe replace loader with [VT](http://icculus.org/vt/), also used by OpenTomb.
* Don't use C-Style code, try to replace with C++ lib
    * Use std::strings
* Mesh has 2 different approaches of storing the same data (eg. mColors and mColorArray), but half of ‘em isn’t implemented. Unify this, probably even combining Mesh and StaticMesh...
* Don’t use float everywhere just because (eg. float colors)
* Add verbose command line flag for debug output also in release builds

## Cmake

* Support SSE with other compilers than Clang (src/CMakeLists.txt)
* Visual C++ compiler flags? (CMakeLists.txt)
* Better test integration

## Future Features

* Depend on libcdio, use it to read original CDs or CUE/TOC/ISO images
* Depend on imgui for nicer GUIs?
* Add ability to play the FMVs. Format? VLC can play them!
* Cut TGA image reader, currently only used for menu background?!
    * Need useful, always available image writer alternative for screenshots then

