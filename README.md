# OpenRaider

[OpenRaider](http://openraider.sourceforge.net) is an Open Source implementation of the classic Tomb Raider Game Engine. It was abandoned in 2003.

This project aims to get OpenRaider running in a modern Unix.

## Building

On Mac OS X 10.9 with [XCode](https://developer.apple.com/xcode/) and [MacPorts](http://www.macports.org) installed, the following should be enough to get all dependencies:

    sudo port install zlib cmake libsdl libsdl_ttf

You will also need an installed [X11 Server](http://xquartz.macosforge.org/trac)!

Also, to get Sound, get [freealut](https://github.com/vancegroup/freealut) and compile it like this:

    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX:STRING="/opt/local" -DCMAKE_C_FLAGS:STRING="-O2"
    make
    sudo make install

Then, just `make debug` and run bin/debug/OpenRaider.

Alternatively, run `make bundle` to create a Mac App Bundle, or `make bundle-image` to put this App Bundle in a Distributable .DMG file.

## License

OpenRaider is based on code, specs, and alogrithms from:

+ GooseEgg/Freyja 3d Modelers by Mongoose
+ TR Rosetta Stone spec sheet by Anonymous
+ TRView 1.0.0 by Yuri Zhivago, with patches by Mongoose

All code should be GPLed, unless otherwise noted.

2013 Housekeeping by xythobuz.
