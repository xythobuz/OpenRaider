# OpenRaider

[OpenRaider](http://openraider.sourceforge.net) is an Open Source implementation of the classic Tomb Raider Game Engine. It was abandoned in 2003.

This project aims to get OpenRaider in a more usable state.

If you just want to see OpenRaider doing something, without installing any of the dependencies needed to build OpenRaider, install [XQuartz](http://xquartz.macosforge.org/trac) (if you don't have it already) and grab the Mac App Bundle from the [most recent release](https://github.com/xythobuz/OpenRaider/releases).

## Configuration

OpenRaider needs some configuration files, and level data and assets from custom levels or the Tomb Raider games.
These are stored in `~/.OpenRaider`. Running the included `setup.sh` will create/copy the necessary files and directories. The Mac App Bundle will do this for you automatically.

You still need to add level files in `~/.OpenRaider/paks/` and add them to `~/.OpenRaider/OpenRaider.init`.
Dust off your old Tomb Raider CDs or grab some [custom levels](http://www.aspidetr.com/levels/yvel-woods-v1-5/) from the interwebs.

## Documentation

All included documentation was moved into the [OpenRaider Repo Wiki](https://github.com/xythobuz/OpenRaider/wiki/_pages)!

## Dependencies

On Mac OS X 10.9 with [XCode](https://developer.apple.com/xcode/) and [MacPorts](http://www.macports.org) installed, the following should be enough to get all dependencies that are available as port:

    sudo port install zlib cmake libsdl libsdl_ttf

You also need the [XQuartz](http://xquartz.macosforge.org/trac) X11 Server.

To get Sound, you need [freealut](https://github.com/vancegroup/freealut). Get, compile and install it like this:

    git clone git@github.com:vancegroup/freealut.git
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX:STRING="/opt/local" -DCMAKE_C_FLAGS:STRING="-O2"
    make
    sudo make install

## Building

If you installed the dependencies using MacPorts, you'll need to have `/opt/local/bin` in your `$PATH` before you can execute make.
If you're using Bash, the MacPorts installer should have automatically edited your configuration.

Just run `make debug` and run `bin/debug/OpenRaider` for a debug build.
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
