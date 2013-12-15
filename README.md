# OpenRaider

[OpenRaider](http://openraider.sourceforge.net) is an Open Source implementation of the classic Tomb Raider Game Engine. It was abandoned in 2003.

This project aims to get OpenRaider running in a modern Unix.

## Dependencies

+ Linux 2.4.16 SMP kernel (Debian distrobution)
+ GCC 2.95.2 compiler and supporting development packages
+ X11 4.1.0 with X11 development package
+ OpenGL 1.2 core and utils development packages
+ SDL 1.2.1 development package
+ OpenAL 0.0.6 development package
+ libz 1.1.3 development package

On Mac OS X 10.9 with [XCode](https://developer.apple.com/xcode/) and [MacPorts](http://www.macports.org) installed, the following should be enough:

    sudo port install libsdl libsdl_ttf

You will also need an installed [X11 Server](http://xquartz.macosforge.org/trac)!

## License

OpenRaider is based on code, specs, and alogrithms from:

+ GooseEgg/Freyja 3d Modelers by Mongoose
+ TR Rosetta Stone spec sheet by Anonymous
+ TRView 1.0.0 by Yuri Zhivago, with patches by Mongoose

All code should be GPLed, unless otherwise noted.

Housekeeping by xythobuz.
