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

Basically, OpenRaider depends on the following:

* OpenGL
* SDL2 & SDL2-TTF
* OpenAL & ALUT
* Posix Threads
* zlib

On Mac OS X 10.9 with [XCode](https://developer.apple.com/xcode/) and [MacPorts](http://www.macports.org) installed, the following should be enough to get all dependencies that are available as port:

    sudo port install zlib cmake libsdl2 libsdl2_ttf

A similar command for the package manager of your favorite Linux Distribution should do the trick.

cmake is only needed to build [freealut](https://github.com/vancegroup/freealut), which you'll need to enable sound output on Mac OS X.
Get, compile and install freealut like this:

    git clone git@github.com:vancegroup/freealut.git
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX:STRING="/usr/local" -DCMAKE_C_FLAGS:STRING="-O2"
    make
    sudo make install

Linux Distributions will probably provide an ALUT library ready to install with their package manager, so you won't need to compile freealut.

## Building

If you installed the dependencies using MacPorts, you'll need to have `/opt/local/bin` in your `$PATH` before you can execute make.
If you're using Bash, the MacPorts installer should have automatically edited your configuration.

Just run `make debug` and run `bin/debug/OpenRaider` for a debug build.
`make release` builds a release binary.
`make bundle` creates a Mac App Bundle that also runs the setup script, if necessary.

## Usage

### Configuration file

OpenRaider will try to load `~/.OpenRaider/OpenRaider.init` or, if it doesn't exist, `OpenRaider.init` from the current directory.
Running `setup.sh` will create a minimal configuration in your home directory.

The configuration file format is very simple:
* Anything following a `#` up to the next `\n` will be ignored.
* Lines starting with an `@` mark a "preprocessor" command. The only one currently implemented is `@include`, allowing you to include another file as configuration file.
* Lines surrounded with `[]`, eg. `[Engine.OpenGL.Driver]` set the mode for following commands.
* Everything else is interpreted as command for the current mode.

### Level PAKs

You can use paks from any Tomb Raider version supported and most user made paks for Tomb Raider.
Only PHD, TR1, TR2, TR3, and TR4 paks are supported as of this writing.

### Sound FXs

Setting up external sound SFX, for TR2 and TR3 paks only.

TR2 and TR3 paks (both have file extension `.tr2`) don't have sound data contained in the pak itself, but instead
they share a common SFX file (often MAIN.SFX) for each pak from a given Tomb Raider version.

Previously, OpenRaider tried to load an SFX file with the same name as the level file, with `.sfx` added to the end.
You had to create a symlink for each level to the SFX file if you were using original Tomb Raider levels, like this:

    cd ~/.OpenRaider/paks/tr3/
    cp /mnt/cdrom/data/MAIN.SFX .
    for i in *.tr2; do ln -s MAIN.SFX $i.sfx; done

Now, OpenRaider just tries to load a `MAIN.SFX` in the same folder as the level file.
If you want to change this behaviour, this happens around line 1075 of `src/OpenRaider.cpp`.

### Key Bindings

| Key                      | Action                                       |
| ------------------------:|:-------------------------------------------- |
| &lt;Alt&gt;&lt;Enter&gt; | Toggle fullscreen                            |
| &lt;Esc&gt;              | Interactive Level load menu                  |
| `                        | Console toggle on/off                        |
| Mouse                    | Turn                                         |
| MouseBtn Left            | Shoot                                        |
| w                        | Move forward                                 |
| s                        | Move back                                    |
| e                        | Move down                                    |
| q                        | Move up                                      |
| r                        | Play sound quick test                        |
| /                        | Drop a waypoint test (formally undocumented) |
| [ ]                      | Adjust idle animation for testing            |

### Console/Config Commands

Console commands (BOOL is now '0' or '1' for less typing).
Pressing &lt;UP&gt; will go back to last command entered (saves typing).

#### Network

| Command          | Action                                |
| ----------------:|:------------------------------------- |
| server           | Start network listen server           |
| killserver       | Stop network listen server            |
| port INT         | Set network port number               |
| connect HOSTNAME | Connect to network server at HOSTNAME |
| disconnect       | Disconnect from network server        |

#### Game

| Command             | Action                                                 |
| -------------------:|:------------------------------------------------------ |
| quit                | Quit the game                                          |
| sshot               | Take screenshot                                        |
| play INT            | Play sound fx with number                              |
| loadlevel STRING    | Load level with mapname STRING                         |
| sensitivity.x FLOAT | Set mouse sensitivity for X movement                   |
| sensitivity.y FLOAT | Set mouse sensitivity for Y movement                   |
| fullscreen          | Toggles fullscreen mode                                |
| walk                | Toggle world clipping on                               |
| ghost               | Toggle world clipping off                              |
| fly                 | Toggle world clipping off without gravity              |
| noclip              | Toggle world clipping in cycle                         |
| hop                 | Toggle room hopping hack                               |
| showfps BOOL        | Show FPS                                               |
| resize STRING       | Change resolution to `xga`, `svga` or `vga`            |

#### Render

| Command          | Action                                       |
| ----------------:|:-------------------------------------------- |
| wireframe        | Render in wireframe (for debugging)          |
| solid            | Render solid color polygons                  |
| texture          | Render with textures                         |
| vertexlight      | Render with vertexlights                     |
| titlescreen      | Render titlescreen                           |
| r_animate BOOL   | Animate all models at once                   |
| r_upf BOOL       | Update room render list once per frame       |
| r_ponytail BOOL  | Render ponytail on Lara                      |
| r_pigtails BOOL  | Render ponytails as pigtails on Lara         |
| r_ponyangle INT  | Set rotation of ponytail on Lara             |
| r_ponyx INT      | Set X offset of ponytail on Lara             |
| r_ponyy INT      | Set Y offset of ponytail on Lara             |
| r_ponyz INT      | Set Z offset of ponytail on Lara             |
| r_viewmodel INT  | Load skeletal model with index INT           |
| r_fog BOOL       | Render fog toggle                            |
| r_portal BOOL    | Render portals in rooms                      |
| r_particle BOOL  | Render particles                             |
| r_vmodel BOOL    | Render view model                            |
| r_sprite BOOL    | Render sprites (room fx and items)           |
| r_roommodel BOOL | Render room models                           |
| r_entmodel BOOL  | Render entity models                         |
| r_light BOOL     | Render with GL lights                        |
| r_ralpha BOOL    | Render alpha pass for rooms                  |
| r_vis BOOL       | Render using visibility checking             |
| r_oneroom        | Render only the current room                 |
| r_allrooms       | Render all rooms in the level (debug/stress) |

#### Set Commands

| Command        | Action                    |
| --------------:|:------------------------- |
| mousegrab BOOL | Set mouse grabbing on/off |

#### Stat Commands

| Command | Action                 |
| -------:|:---------------------- |
| fps     | Toggle showing FPS     |
| pos     | Show current location  |
| room    |                        |
| flags   | Show room flags in hex |

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

The included example Font, [Droid Sans Mono](http://www.droidfonts.com/licensing/), was created by Steve Matteson and is licensed under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0).

The included sample WAV file is made from the [Falling Mate ringtone](http://brechler-web.de/fallingmate/) by [Plaetzchen](http://plaetzchen.cc).

The included Unit Test Framework [greatest](https://github.com/silentbicycle/greatest/) by Scott Vokes is licensed under the [ISC License](http://www.isc.org/downloads/software-support-policy/isc-license/).

OpenRaider is based on code, specs, and alogrithms from:

+ GooseEgg/Freyja 3d Modelers by Mongoose
+ TR Rosetta Stone spec sheet by Anonymous
+ TRView 1.0.0 by Yuri Zhivago, with patches by Mongoose

All code should be GPLed, unless otherwise noted.

2013, 2014 improvements & clean-up by xythobuz.
