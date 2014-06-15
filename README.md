# OpenRaider

This is a fork of the [OpenRaider](http://openraider.sourceforge.net) Open Source implementation of the classic Tomb Raider Game Engine. The original project was abandoned in 2003.

This fork aims to get OpenRaider in a more usable state, maybe one day even being able to play the old Tomb Raider games flawlessly...

If you just want to see OpenRaider doing something on your Mac, without installing any of the dependencies needed to build OpenRaider, grab the Mac App Bundle from the [most recent release](https://github.com/xythobuz/OpenRaider/releases).

It seems as if OpenRaider will currently only work on Little-Endian platforms. This is not nice and will hopefully be fixed in the future!

[![Flattr this git repo](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=xythobuz&url=https://github.com/xythobuz/OpenRaider&title=OpenRaider&language=&tags=github&category=software)

## Configuration

OpenRaider needs some configuration files, and level data and assets from custom levels or the Tomb Raider games.
These are stored in `~/.OpenRaider`. Running `make setup` will create/copy the necessary files and directories.

You still need to add level files in `~/.OpenRaider/paks/`.
Dust off your old Tomb Raider CDs or grab some [custom levels](http://www.aspidetr.com/levels/yvel-woods-v1-5/) from the interwebs.

## Documentation

All previously included documentation was moved into the [OpenRaider Repo Wiki](https://github.com/xythobuz/OpenRaider/wiki/_pages).

A more or less recent [Doxygen documentation](http://xythobuz.github.io/OpenRaider/) of OpenRaider should be on the Github Pages for OpenRaider.

## Dependencies

Basically, OpenRaider depends on the following:

* OpenGL
* SDL2
* SDL2-TTF (if you want to use TTF fonts)
* OpenAL & ALUT
* zlib
* cmake as build system

On Mac OS X 10.9 with [XCode](https://developer.apple.com/xcode/) and [MacPorts](http://www.macports.org) installed, the following should be enough to get all dependencies that are available as port:

    sudo port install cmake zlib libsdl2 libsdl2_ttf

A similar command for the package manager of your favorite Linux Distribution should do the trick.

cmake is also needed to build [freealut](https://github.com/vancegroup/freealut), which you'll need to enable sound output on Mac OS X.
Get, compile and install freealut like this:

    git clone git@github.com:vancegroup/freealut.git
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX:STRING="/usr/local" -DCMAKE_C_FLAGS:STRING="-O2"
    make
    sudo make install

Linux Distributions will probably provide an ALUT library ready to install with their package manager, so you won't need to compile freealut.

Sound support is optional now, however. If cmake can’t find OpenAL and ALUT, Audio will be deactivated. If you want to explicitly deactivate it, call cmake with `-DENABLE_AUDIO=NO`.

## Building

Do a standard cmake out-of-source build to get a debug configuration and then run the resulting executable:

    mkdir build && cd build
    cmake ..
    make run

On Mac OS X, running `make install` will produce a (more or less) distributable OpenRaider.app bundle.

You can run a static analysis with cppcheck using `make check`, `make checkFull` or `make checkConfig`.

A Doxygen API documentation can be created with `make doc`.

`make clean` should remove all intermediary files. `make run` will run the binary.

You can also generate working XCode Project files with `cmake -G Xcode ..`.

## Usage

### Configuration file

OpenRaider will try to load `~/.OpenRaider/OpenRaider.ini` or, if it doesn't exist, `OpenRaider.ini` from the current directory.
Running `make setup` will create a minimal configuration in your home directory.

The configuration file format is very simple:
* Every line will be executed as in-game console command
* The # character marks the beginning of a single-line comment

### Level PAKs

You can use paks from any classic Tomb Raider Game (1 - 5) and most user made paks for Tomb Raider. Tomb Raider 5 support is however still very limited.

### Sound FXs

TR2 and TR3 paks (both have file extension `.tr2`) don't have sound data contained in the pak itself, but instead
they share a common SFX file (often MAIN.SFX) for each pak from a given Tomb Raider version.

OpenRaider tries to load a `MAIN.SFX` from the same folder as the selected level file.

### Default Key Bindings

| Key               | Action                |
| -----------------:|:--------------------- |
| &lt;Esc&gt;       | Toggle menu           |
| &lt;backquote&gt; | Console toggle on/off |
| w                 | Move forward          |
| s                 | Move back             |
| a                 | Move left             |
| d                 | Move right            |
| &lt;space&gt;     | Jump                  |
| &lt;left ctrl&gt; | crouch                |
| Mouse             | Turn                  |
| MouseBtn Left     | Shoot/Use             |
| MouseBtn Right    | Holster/Equip weapons |

### Console/Config Commands

Every available command should be listed in the in-game help. Just type `help` in the OpenRaider console, which can be activated by default with the backquote key.

### Tomb Raider Level Editor Font support

OpenRaider can read Font.pc files used by the TRLE. If the fonts glyph positions match the TR4 defaults, only a Font.pc file is required. If the positions differ, you also need a [Leikkuri/Cutter](http://trep.trlevel.de/en/downloads.html) preset file (`.lps`).

TRLE Font support will automatically be built and used when selecting a font in your config file ending with `.pc`.

If a `.lps` is required, give it the same name as your font file.

I’ve made [a small writeup on my website](http://xythobuz.de/2014_06_14_trle_font.html) about this.

### Wireframe mode colors

* Red (thick) lines are portal outlines
* Red (thin) lines are triangle room polygons
* Black lines are quadralateral room polygons
* White meshes are entities
* Blue meshes are sprites
* Yellow meshes are room models
* Green lines with red endpoints are room bboxes
* Pink dots are visibility checks

## License

OpenRaider is based on code, specs, and alogrithms from:

* GooseEgg/Freyja 3d Modelers by Mongoose
* TR Rosetta Stone spec sheet by Anonymous
* TRView 1.0.0 by Yuri Zhivago, with patches by Mongoose

All code should be GPLed, unless otherwise noted.

Forked in December 2013 by xythobuz.

The included example Font, [Droid Sans Mono](http://www.droidfonts.com/licensing/), was created by Steve Matteson and is licensed under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0).

The included TRLE Font is from [Laras Levelbase](http://laraslevelbase.org/stuff/index.asp?id=1967).

There are some included cmake scripts:

* [FindALUT](https://github.com/rpavlik/cmake-modules/blob/master/FindALUT.cmake)
* [FindSDL2](https://github.com/dhewm/dhewm3/blob/master/neo/sys/cmake/FindSDL2.cmake)
* [FindSDL2TTF](https://github.com/Deraen/ohj2710/blob/master/cmake_modules/FindSDL2TTF.cmake)
* [FindSSE](https://gitorious.org/vc/vc/source/a1d8b9fc31060d870386613cc72319546c850b87:cmake/FindSSE.cmake)

See the respective files in `cmake` for their licensing.

* Copyright 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>, Iowa State University, Distributed under the Boost Software License.
* Copyright 2003-2009 Kitware, Inc.
* Eric Wing

