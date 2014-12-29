# To-Do List

## General

* Add verbose command line flag for debug output also in release builds
* Don’t depend on setup: no data or config files should be necessary
* Be able to change configuration from within OpenRaider

## Bugs

* Screenshots are sometimes distorted?
    * Seems to happen if OpenRaider.ini sets a resolution larger than the window can be

## Cmake

* Support SSE with other compilers than Clang (src/CMakeLists.txt)
* Visual C++ compiler flags? (CMakeLists.txt)
* Better test integration

## Future Features

* Depend on physfs for easier file location management
* Depend on libcdio, use it to read original CDs or CUE/TOC/ISO images
* Add ability to play the FMVs. Format? VLC can play them!

