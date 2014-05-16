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
    * Use std::strings

## Changes

* Using std::vector with [] is not bound checked. Segfaults were caused because the upper bound of the argument was never checked and garbage was returned... Do consistent checks, or use .at() as it throws an exception
* Creating the Room class has introduced a bug that crashes when walking the level:

    Assertion failed: (sector < (int)mRooms.at(room)->sizeSectors()), function isWall, file /Users/thomas/Projekte/OpenRaider/src/World.cpp, line 180.

* The wrong SkeletalModels are used by other entities...?

## Cmake

* Windows setup script needed, moving to AppData/Roaming
* Support SSE with other compilers than Clang (src/CMakeLists.txt)
* Visual C++ compiler flags? (CMakeLists.txt)

## Future Features

* Use only assets from old TR games?
* PCX image reading for eg. TR2 built-in menu background
    * Cut TGA image reader, currently only used for menu background?!
* When cutscene rendering is working, use TR4/5 style menu?

## Errors

* Sometimes this on quitting OpenRaider:

    OpenRaider(92329,0x7fff7a8c4310) malloc: *** error for object 0x7fabc2001000: incorrect checksum for freed object - object was probably modified after being freed.


