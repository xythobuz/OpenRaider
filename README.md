# OpenRaider

[![Build Status](https://travis-ci.org/xythobuz/OpenRaider.svg?branch=master)](https://travis-ci.org/xythobuz/OpenRaider)

This project started as a fork of the [OpenRaider](http://openraider.sourceforge.net) Open Source implementation of the classic Tomb Raider Game Engine. The original project was abandoned in 2003. In the meantime, this OpenRaider has become a full rewrite of the original.

OpenRaider currently supports TR1, TR2 and TR3 files, as well as custom maps based on these engines.

Visit the IRC Channel `#OpenRaider` on `chat.freenode.net` ([Webchat](http://webchat.freenode.net/?channels=%23OpenRaider)).

[![Flattr this git repo](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=xythobuz&url=https://github.com/xythobuz/OpenRaider&title=OpenRaider&language=&tags=github&category=software)

## Dependencies

* [C++11 compatible compiler](http://en.cppreference.com/w/cpp/compiler_support)
* [OpenGL](https://www.opengl.org) (3.3 Core Profile compatible)
* [cmake](http://www.cmake.org) (>= 2.8.8)
* [GLM](http://glm.g-truc.net) (>= 0.9.6)
* [glbinding](https://github.com/hpicgs/glbinding)
* [SDL2](http://www.libsdl.org) or [GLFW](http://www.glfw.org)
* [OpenAL](http://www.openal.org) & [ALUT](https://github.com/vancegroup/freealut) (optional, needed for sound output)

## Building

The basic steps needed to build and run OpenRaider:

    git clone https://github.com/xythobuz/OpenRaider.git
    mkdir build && cd build
    cmake ../OpenRaider
    make run

If cmake can’t find OpenAL and ALUT, Sound support will be deactivated. If you want to explicitly deactivate it, call cmake with `-DENABLE_AUDIO=NO`.

GLFW will be used if it is found instead of SDL2, or if you call cmake with `-DFORCE_GLFW=YES`. Be aware that limited controller support is currently only included when building with SDL2.

Running `make install` on Linux will install OpenRaider to /usr/local/... (the executable in bin, the data files in share). You can change this path by passing `-DCMAKE_INSTALL_PREFIX=”/some/where”` to cmake.

Running `make install` on Mac OS X will produce a distributable OpenRaider.app bundle that includes all linked shared libraries, so it can be run without installing any dependencies.

Run `make check` to execute the included Unit Tests.

Run `make cppcheck`, `make cppcheckFull` or `make cppcheckConfig` to run a static analysis using cppcheck.

Run `make format` to auto-indent and format all source files using astyle.

Run `make doc` to create a doxygen documentation.

## Configuration

OpenRaider is still somewhat depending on some configuration and data files. These can be stored in different locations, the most common one being `~/.OpenRaider`. Running `make setup` will create/copy the necessary files and directories and an example configuration.

Place the asset files of your Tomb Raider games in the pak folder created in `~./OpenRaider`. Don’t forget to copy at least the maps, the `TOMBPC.DAT` script (TR2/TR3 only) and the sound file `MAIN.SFX` (TR2/TR3 only).

Currently, you can load single level files, but the game script included with the Tomb Raider games will be read as well, if available. In the future, OpenRaider will hopefully be able to read disc images of the Tomb Raider games as well. Also, OpenRaider should run without a prepared config file in the future.

## Configuration file

OpenRaider will try to load `~/.OpenRaider/OpenRaider.ini` or, if it doesn’t exist, `OpenRaider.ini` from the current working directory.

Every line in this (text) file will be executed as an in-game console command. The # character marks the beginning of a comment until the next new-line.

Use the in-game console help for more informations about available commands and their usage.

## Default Key Bindings

| Key                | Action                |
| ------------------:|:--------------------- |
| &lt;Esc&gt;        | Toggle menu           |
| q                  | Toggle debug UI       |
| &lt;back quote&gt; | Toggle console        |
| w                  | Move forward          |
| s                  | Move back             |
| a                  | Move left             |
| d                  | Move right            |
| &lt;space&gt;      | Jump                  |
| &lt;left ctrl&gt;  | crouch                |
| Mouse              | Turn                  |
| MouseBtn Left      | Shoot/Use             |
| MouseBtn Right     | Holster/Equip weapons |

If SDL2 is used, the first controller connected at the start of OpenRaider can be used as well, if its type is known to SDL2. The button-mapping is not yet changeable, however.

## License

The original OpenRaider is based on code, specs, and algorithms from:

* GooseEgg/Freyja 3d Modelers by Mongoose
* TR Rosetta Stone spec sheet by Anonymous
* TRView 1.0.0 by Yuri Zhivago, with patches by Mongoose

All code should be GPLed, unless otherwise noted.

Forked/Rewritten in December 2013 by Thomas Buck (xythobuz@xythobuz.de).

The included example Font, [Droid Sans Mono](http://www.droidfonts.com/licensing/), was created by Steve Matteson and is licensed under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0).

The included TRLE Font is from [Laras Levelbase](http://laraslevelbase.org/stuff/index.asp?id=1967).

There are some included cmake scripts:

* [FindALUT](https://github.com/rpavlik/cmake-modules/blob/master/FindALUT.cmake)
* [FindSDL2](https://github.com/dhewm/dhewm3/blob/master/neo/sys/cmake/FindSDL2.cmake)
* [GetGitRevisionDescription.cmake](https://github.com/rpavlik/cmake-modules/blob/master/GetGitRevisionDescription.cmake)
* [GetGitRevisionDescription.cmake.in](https://github.com/rpavlik/cmake-modules/blob/master/GetGitRevisionDescription.cmake.in)
* [FindGLM] (https://github.com/g-truc/glm/blob/master/util/FindGLM.cmake)
* [FindGLFW] (https://github.com/PixarAnimationStudios/OpenSubdiv/blob/master/cmake/FindGLFW.cmake)

See the respective files in `cmake` for their licensing.

The included header [ezOptionParser](http://ezoptionparser.sourceforge.net) is licensed under the [MIT License](http://opensource.org/licenses/MIT).

The included GUI lib, [imgui](https://github.com/ocornut/imgui/) is Copyright (c) 2014 Omar Cornut. See src/deps/imgui/LICENSE for more informations about the MIT license used. 

Also included is the imgui addon [imguifilesystem by Flix01](https://gist.github.com/Flix01/f34b5efa91e50a241c1b).
