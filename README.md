# OpenRaider

[OpenRaider](http://openraider.sourceforge.net) is an Open Source implementation of the classic Tomb Raider Game Engine. It was abandoned in 2003.

This project aims to get OpenRaider running in a modern Unix.

## Configuration

OpenRaider needs some configuration files, and level data and assets from custom levels or the Tomb Raider games.
These are stored in `~/.OpenRaider`. Running the included `setup.sh` will create/copy the necessary files and directories.

## Dependencies

On Mac OS X 10.9 with [XCode](https://developer.apple.com/xcode/) and [MacPorts](http://www.macports.org) installed, the following should be enough to get all dependencies that are available as port:

    sudo port install zlib cmake libsdl libsdl_ttf

You also need the  [XQuartz](http://xquartz.macosforge.org/trac) X11 Server.

To get Sound, you need [freealut](https://github.com/vancegroup/freealut). Get, compile and install it like this:

    git clone git@github.com:vancegroup/freealut.git
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX:STRING="/opt/local" -DCMAKE_C_FLAGS:STRING="-O2"
    make
    sudo make install

## Building

Just `make debug` and run `bin/debug/OpenRaider` for a debug build.
`make release` builds a release binary.
`make bundle` creates a Mac App Bundle that also runs the setup script, if necessary.
`make bundle-image` packs the App Bundle into a DMG ready for distribution.

## Usage

See the original `README.old` file. In the Main Menu, &lt;Esc&gt; will allow you to select a level to play.

## License

OpenRaider is based on code, specs, and alogrithms from:

+ GooseEgg/Freyja 3d Modelers by Mongoose
+ TR Rosetta Stone spec sheet by Anonymous
+ TRView 1.0.0 by Yuri Zhivago, with patches by Mongoose

All code should be GPLed, unless otherwise noted.

2013 Housekeeping by xythobuz.
