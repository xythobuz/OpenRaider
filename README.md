# OpenRaider

[OpenRaider](http://openraider.sourceforge.net) is an Open Source implementation of the classic Tomb Raider Game Engine. It was abandoned in 2003.

This project aims to get OpenRaider in a more usable state, maybe one day even being able to play the old Tomb Raider games flawlessly...

Currently, my focus is on building and running under Mac OS X.
However, my changes should not break the Linux "buildability" :)

If you just want to see OpenRaider doing something on your Mac, without installing any of the dependencies needed to build OpenRaider, install [XQuartz](http://xquartz.macosforge.org/trac) (if you don't have it already) and grab the Mac App Bundle from the [most recent release](https://github.com/xythobuz/OpenRaider/releases).
If you don't install XQuartz, Mac OS X should prompt you to do so on the first launch of OpenRaider.

It seems as if OpenRaider will currently only work on Little-Endian platforms. This is not nice and will hopefully be fixed in the future!

## Configuration

OpenRaider needs some configuration files, and level data and assets from custom levels or the Tomb Raider games.
These are stored in `~/.OpenRaider`. Running the included `setup.sh` will create/copy the necessary files and directories. The Mac App Bundle will do this for you automatically.

You still need to add level files in `~/.OpenRaider/paks/` and add them to `~/.OpenRaider/OpenRaider.init`.
Dust off your old Tomb Raider CDs or grab some [custom levels](http://www.aspidetr.com/levels/yvel-woods-v1-5/) from the interwebs.

## Documentation

All previously included documentation was moved into the [OpenRaider Repo Wiki](https://github.com/xythobuz/OpenRaider/wiki/_pages).
Also take a look at the [BUGS](https://github.com/xythobuz/OpenRaider/wiki/Bugs), [TODO](https://github.com/xythobuz/OpenRaider/wiki/To-Do) and [Requirements](https://github.com/xythobuz/OpenRaider/wiki/Requirements).

A more or less recent [Doxygen documentation](http://xythobuz.github.io/OpenRaider/) of OpenRaider should be on the Github Pages for OpenRaider.

## Dependencies

On Mac OS X 10.9 with [XCode](https://developer.apple.com/xcode/) and [MacPorts](http://www.macports.org) installed, the following should be enough to get all dependencies that are available as port:

    sudo port install zlib cmake libsdl libsdl_ttf

cmake is only needed for [freealut](https://github.com/vancegroup/freealut), which you'll need to enable sound output.
Get, compile and install freealut like this:

    git clone git@github.com:vancegroup/freealut.git
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX:STRING="/usr/local" -DCMAKE_C_FLAGS:STRING="-O2"
    make
    sudo make install

You'll also need the [XQuartz](http://xquartz.macosforge.org/trac) X11 Server.

## Building

If you installed the dependencies using MacPorts, you'll need to have `/opt/local/bin` in your `$PATH` before you can execute make.
If you're using Bash, the MacPorts installer should have automatically edited your configuration.

Just run `make debug` and run `bin/debug/OpenRaider` for a debug build.
`make release` builds a release binary.
`make bundle` creates a Mac App Bundle that also runs the setup script, if necessary.

## Usage

See the original `README.old` file. In the Main Menu, &lt;Esc&gt; will allow you to select a level to play.

## License

The included example Font, [Droid Sans Mono](http://www.droidfonts.com/licensing/), was created by Steve Matteson and is licensed under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0).

The included sample WAV file is made from the [Falling Mate ringtone](http://brechler-web.de/fallingmate/) by [Plaetzchen](http://plaetzchen.cc).

OpenRaider is based on code, specs, and alogrithms from:

+ GooseEgg/Freyja 3d Modelers by Mongoose
+ TR Rosetta Stone spec sheet by Anonymous
+ TRView 1.0.0 by Yuri Zhivago, with patches by Mongoose

All code should be GPLed, unless otherwise noted.

2013 improvements & clean-up by xythobuz.
