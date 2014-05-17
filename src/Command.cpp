/*!
 * \file src/Command.cpp
 * \brief OpenRaider command implementation
 *
 * \author xythobuz
 */

#include <cstdio>
#include <cstring>

#include "WindowSDL.h"

#include "global.h"
#include "Console.h"
#include "Entity.h"
#include "Game.h"
#include "math/math.h"
#include "Menu.h"
#include "Sound.h"
#include "TombRaider.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "OpenRaider.h"

int OpenRaider::command(const char *command, std::vector<char *> *args) {
    assert(command != NULL);
    assert(command[0] != '\0');
    assert(args != NULL);

    if (strcmp(command, "set") == 0) {
        if (args->size() != 2) {
            getConsole().print("Invalid use of set-command");
            return -1;
        } else {
            return set(args->at(0), args->at(1));
        }
    } else if (strcmp(command, "bind") == 0) {
        if (args->size() != 2) {
            getConsole().print("Invalid use of bind-command");
            return -2;
        } else {
            return bind(args->at(0), args->at(1));
        }
    } else if (strcmp(command, "quit") == 0) {
        exit(0);
    } else if (strcmp(command, "load") == 0) {
        if (!mRunning) {
            getConsole().print("Use load command interactively!");
            return -999;
        }
        char *tmp = bufferString("%s/%s", mPakDir, args->at(0));
        int error = getGame().loadLevel(tmp);
        delete [] tmp;
        return error;
    } else if (strcmp(command, "sshot") == 0) {
        if (!mRunning) {
            getConsole().print("Use sshot command interactively!");
            return -999;
        }
        char *filename = bufferString("%s/sshots/%s", mBaseDir, VERSION);
        bool console = (args->size() > 0) && (strcmp(args->at(0), "console") == 0);
        bool menu = (args->size() > 0) && (strcmp(args->at(0), "menu") == 0);
        if (!console) {
            getConsole().setVisible(false);
            if (menu)
                getMenu().setVisible(true);
            frame();
            frame(); // Double buffered
        }
        getRender().screenShot(filename);
        if (!console) {
            getConsole().setVisible(true);
            if (menu)
                getMenu().setVisible(false);
        }
        getConsole().print("Screenshot stored...");
        delete filename;
    } else if (strcmp(command, "mode") == 0) {
        if (args->size() > 0) {
            char *mode = args->at(0);
            if (strcmp(mode, "wireframe") == 0) {
                if (getGame().isLoaded()) {
                    getRender().setMode(Render::modeWireframe);
                    getConsole().print("Wireframe mode");
                } else {
                    getConsole().print("Load a level to set this mode!");
                    return -3;
                }
            } else if (strcmp(mode, "solid") == 0) {
                if (getGame().isLoaded()) {
                    getRender().setMode(Render::modeSolid);
                    getConsole().print("Solid mode");
                } else {
                    getConsole().print("Load a level to set this mode!");
                    return -4;
                }
            } else if (strcmp(mode, "texture") == 0) {
                if (getGame().isLoaded()) {
                    getRender().setMode(Render::modeTexture);
                    getConsole().print("Texture mode");
                } else {
                    getConsole().print("Load a level to set this mode!");
                    return -5;
                }
            } else if (strcmp(mode, "vertexlight") == 0) {
                if (getGame().isLoaded()) {
                    getRender().setMode(Render::modeVertexLight);
                    getConsole().print("Vertexlight mode");
                } else {
                    getConsole().print("Load a level to set this mode!");
                    return -6;
                }
            } else if (strcmp(mode, "titlescreen") == 0) {
                getRender().setMode(Render::modeLoadScreen);
                getConsole().print("Titlescreen mode");
            } else {
                getConsole().print("Invalid use of mode command (%s)!", mode);
                return -7;
            }
        } else {
            getConsole().print("Invalid use of mode command!");
            return -8;
        }
    } else if (strcmp(command, "move") == 0) {
        if (!mRunning) {
            getConsole().print("Use move command interactively!");
            return -999;
        }
        if (args->size() > 0) {
            if (getGame().isLoaded()) {
                char *move = args->at(0);
                if (strcmp(move, "walk") == 0) {
                    getGame().getLara().setMoveType(Entity::MoveTypeWalk);
                    getConsole().print("Lara is walking...");
                } else if (strcmp(move, "fly") == 0) {
                    getGame().getLara().setMoveType(Entity::MoveTypeFly);
                    getConsole().print("Lara is flying...");
                } else if (strcmp(move, "noclip") == 0) {
                    getGame().getLara().setMoveType(Entity::MoveTypeNoClipping);
                    getConsole().print("Lara is noclipping...");
                } else {
                    getConsole().print("Invalid use of move command (%s)!", move);
                    return -9;
                }
            } else {
                getConsole().print("Load a level to change the movement type!");
                return -10;
            }
        } else {
            getConsole().print("Invalid use of move command!");
            return -11;
        }
    } else if (strcmp(command, "sound") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole().print("Use sound command interactively!");
            return -999;
        }
        if (args->size() > 0) {
            getSound().play(atoi(args->at(0)));
        } else {
            getConsole().print("Invalid use of sound command!");
            return -12;
        }
    } else if (strcmp(command, "animate") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole().print("Use animate command interactively!");
            return -999;
        }
        if (args->size() > 0) {
            char c = args->at(0)[0];
            if (c == 'n') {
                // Step all skeletal models to their next animation
                if (getRender().getFlags() & Render::fAnimateAllModels) {
                    for (unsigned int i = 0; i < getWorld().sizeEntity(); i++) {
                        Entity &e = getWorld().getEntity(i);
                        SkeletalModel &m = e.getModel();
                        if (e.getAnimation() < (m.size() - 1))
                            e.setAnimation(e.getAnimation() + 1);
                        else
                            e.setAnimation(0);
                    }
                } else {
                    getConsole().print("Animations need to be enabled!");
                }
            } else if (c == 'p') {
                // Step all skeletal models to their previous animation
                if (getRender().getFlags() & Render::fAnimateAllModels) {
                    for (unsigned int i = 0; i < getWorld().sizeEntity(); i++) {
                        Entity &e = getWorld().getEntity(i);
                        SkeletalModel &m = e.getModel();
                        if (e.getAnimation() > 0)
                            e.setAnimation(e.getAnimation() - 1);
                        else
                            if (m.size() > 0)
                                e.setAnimation(m.size() - 1);
                    }
                } else {
                    getConsole().print("Animations need to be enabled!");
                }
            } else {
                // Enable or disable animating all skeletal models
                bool b;
                if (readBool(args->at(0), &b) < 0) {
                    getConsole().print("Pass BOOL to animate command!");
                    return -13;
                }
                if (b)
                    getRender().setFlags(Render::fAnimateAllModels);
                else
                    getRender().clearFlags(Render::fAnimateAllModels);
                getConsole().print(b ? "Animating all models" : "No longer animating all models");
            }
        } else {
            getConsole().print("Invalid use of animate command!");
            return -14;
        }
    } else if (strcmp(command, "light") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to light command!");
                return -15;
            }
            if (b)
                getRender().setFlags(Render::fGL_Lights);
            else
                getRender().clearFlags(Render::fGL_Lights);
            getConsole().print("GL-Lights are now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of light-command!");
            return -16;
        }
    } else if (strcmp(command, "fog") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to fog command!");
                return -17;
            }
            if (b)
                getRender().setFlags(Render::fFog);
            else
                getRender().clearFlags(Render::fFog);
            getConsole().print("Fog is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of fog-command!");
            return -18;
        }
    } else if (strcmp(command, "viewmodel") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole().print("Use viewmodel command interactively!");
            return -999;
        }
        unsigned int n = atoi(args->at(0));
        if (n < getWorld().sizeSkeletalModel())
            getGame().getLara().setSkeletalModel(n);
        else {
            getConsole().print("Invalid SkeletalModel index!");
            return -66;
        }
    } else if (strcmp(command, "pos") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole().print("Use pos command interactively!");
            return -21;
        }
        getGame().getLara().print();
    } else if (strcmp(command, "vmodel") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to vmodel command!");
                return -22;
            }
            if (b)
                getRender().setFlags(Render::fViewModel);
            else
                getRender().clearFlags(Render::fViewModel);
            getConsole().print("Viewmodel is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of vmodel-command!");
            return -23;
        }
    } else if (strcmp(command, "ralpha") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to ralpha command!");
                return -24;
            }
            if (b)
                getRender().setFlags(Render::fRoomAlpha);
            else
                getRender().clearFlags(Render::fRoomAlpha);
            getConsole().print("Room Alpha is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of ralpha-command!");
            return -25;
        }
    } else if (strcmp(command, "vis") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to vis command!");
                return -28;
            }
            if (b)
                getRender().setFlags(Render::fUsePortals);
            else
                getRender().clearFlags(Render::fUsePortals);
            getConsole().print("Portals are now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of vis-command!");
            return -29;
        }
    } else if (strcmp(command, "upf") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to upf command!");
                return -30;
            }
            if (b)
                getRender().setFlags(Render::fUpdateRoomListPerFrame);
            else
                getRender().clearFlags(Render::fUpdateRoomListPerFrame);
            getConsole().print("URLPF is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of upf-command!");
            return -31;
        }
    } else if (strcmp(command, "entmodel") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to entmodel command!");
                return -38;
            }
            if (b)
                getRender().setFlags(Render::fEntityModels);
            else
                getRender().clearFlags(Render::fEntityModels);
            getConsole().print("Entmodels are now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of entmodel-command!");
            return -39;
        }
    } else if (strcmp(command, "oneroom") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to oneroom command!");
                return -40;
            }
            if (b)
                getRender().setFlags(Render::fOneRoom);
            else
                getRender().clearFlags(Render::fOneRoom);
            getConsole().print("Rendering one room is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of oneroom-command!");
            return -41;
        }
    } else if (strcmp(command, "allrooms") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to allrooms command!");
                return -42;
            }
            if (b)
                getRender().setFlags(Render::fAllRooms);
            else
                getRender().clearFlags(Render::fAllRooms);
            getConsole().print("Rendering all rooms is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of allrooms-command!");
            return -43;
        }
    } else if (strcmp(command, "ponytail") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to ponytail command!");
                return -44;
            }
            if (b)
                getRender().setFlags(Render::fRenderPonytail);
            else
                getRender().clearFlags(Render::fRenderPonytail);
            getConsole().print("Ponytail is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of ponytail-command!");
            return -45;
        }
    } else if (strcmp(command, "pigtail") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole().print("Use pigtail command interactively!");
            return -999;
        }
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to pigtail command!");
                return -46;
            }
            SkeletalModel &tmp = getGame().getLara().getModel();
            tmp.setPigTail(b);
            getConsole().print("Pigtail is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of pigtail-command!");
            return -47;
        }
    } else if (strcmp(command, "ponypos") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole().print("Use ponypos command interactively!");
            return -999;
        }
        if (args->size() > 3) {
            SkeletalModel &tmp = getGame().getLara().getModel();
            tmp.setPonyPos((float)atof(args->at(0)), (float)atof(args->at(1)),
                    (float)atof(args->at(2)), (float)atof(args->at(3)));
        } else {
            getConsole().print("Invalid use of ponypos-command!");
            return -48;
        }
    } else if (strcmp(command, "help") == 0) {
        if (args->size() == 0) {
            getConsole().print("Available commands:");
            getConsole().print("  load      - load a level");
            getConsole().print("  set       - set a parameter");
            getConsole().print("  bind      - bind a keyboard/mouse action");
            getConsole().print("  sshot     - make a screenshot");
            getConsole().print("  move      - [walk|fly|noclip]");
            getConsole().print("  sound     - INT - Test play sound");
            getConsole().print("  mode      - MODE - Render mode");
            getConsole().print("  animate   - [BOOL|n|p] - Animate models");
            getConsole().print("  light     - BOOL - GL Lights");
            getConsole().print("  fog       - BOOL - GL Fog");
            getConsole().print("  viewmodel - INT - Change Laras model");
            getConsole().print("  pos       - Print position info");
            getConsole().print("  vmodel    - BOOL - View Model");
            getConsole().print("  ralpha    - BOOL - Room Alpha");
            getConsole().print("  vis       - BOOL - Use Portals");
            getConsole().print("  upf       - BOOL - Update Room List Per Frame");
            getConsole().print("  entmodel  - BOOL");
            getConsole().print("  oneroom   - BOOL");
            getConsole().print("  allrooms  - BOOL");
            getConsole().print("  ponytail  - BOOL");
            getConsole().print("  pigtail   - BOOL");
            getConsole().print("  ponypos   - FLOAT FLOAT FLOAT FLOAT - x y z angle");
            getConsole().print("  help      - print command help");
            getConsole().print("  quit      - exit OpenRaider");
            getConsole().print("Use help COMMAND to get additional info");
        } else if (args->size() == 1) {
            return help(args->at(0));
        } else {
            getConsole().print("Invalid use of help-command");
            return -49;
        }
    } else {
        getConsole().print("Unknown command: %s ", command);
        return -50;
    }

    return 0;
}

int OpenRaider::help(const char *cmd) {
    assert(cmd != NULL);
    assert(cmd[0] != '\0');

    if (strcmp(cmd, "set") == 0) {
        getConsole().print("set-Command Usage:");
        getConsole().print("  set VAR VAL");
        getConsole().print("Available Variables:");
        getConsole().print("  basedir    STRING");
        getConsole().print("  pakdir     STRING");
        getConsole().print("  audiodir   STRING");
        getConsole().print("  datadir    STRING");
        getConsole().print("  font       STRING");
        getConsole().print("  gldriver   STRING");
        getConsole().print("  size       \"INTxINT\"");
        getConsole().print("  fullscreen BOOL");
        getConsole().print("  audio      BOOL");
        getConsole().print("  volume     BOOL");
        getConsole().print("  mouse_x    FLOAT");
        getConsole().print("  mouse_y    FLOAT");
        getConsole().print("  fps        BOOL");
        getConsole().print("Enclose STRINGs with \"\"!");
        getConsole().print("size expects a STRING in the specified format");
    } else if (strcmp(cmd, "bind") == 0) {
        getConsole().print("bind-Command Usage:");
        getConsole().print("  bind ACTION KEY");
        getConsole().print("Available Actions:");
        getConsole().print("  menu");
        getConsole().print("  console");
        getConsole().print("  forward");
        getConsole().print("  backward");
        getConsole().print("  left");
        getConsole().print("  right");
        getConsole().print("  jump");
        getConsole().print("  crouch");
        getConsole().print("  use");
        getConsole().print("  holster");
        getConsole().print("Key-Format:");
        getConsole().print("  'a' or '1'    for character/number keys");
        getConsole().print("  \"leftctrl\"  for symbols and special keys");
    } else if (strcmp(cmd, "load") == 0) {
        getConsole().print("load-Command Usage:");
        getConsole().print("  load levelfile.name");
    } else if (strcmp(cmd, "game") == 0) {
        getConsole().print("Use \"game help\" for more info");
    } else if (strcmp(cmd, "sshot") == 0) {
        getConsole().print("sshot-Command Usage:");
        getConsole().print("  sshot [console|menu]");
        getConsole().print("Add console/menu to capture them too");
    } else if (strcmp(cmd, "sound") == 0) {
        getConsole().print("sound-Command Usage:");
        getConsole().print("  sound INT");
        getConsole().print("Where INT is a valid sound ID integer");
    } else if (strcmp(cmd, "move") == 0) {
        getConsole().print("move-Command Usage:");
        getConsole().print("  move COMMAND");
        getConsole().print("Where COMMAND is one of the following:");
        getConsole().print("  walk");
        getConsole().print("  fly");
        getConsole().print("  noclip");
    } else if (strcmp(cmd, "mode") == 0) {
        getConsole().print("mode-Command Usage:");
        getConsole().print("  mode MODE");
        getConsole().print("Where MODE is one of the following:");
        getConsole().print("  wireframe");
        getConsole().print("  solid");
        getConsole().print("  texture");
        getConsole().print("  vertexlight");
        getConsole().print("  titlescreen");
    } else if (strcmp(cmd, "animate") == 0) {
        getConsole().print("animate-Command Usage:");
        getConsole().print("  animate [n|p|BOOL]");
        getConsole().print("Where the commands have the following meaning:");
        getConsole().print("  BOOL to (de)activate animating all models");
        getConsole().print("  n to step all models to their next animation");
        getConsole().print("  p to step all models to their previous animation");
    } else {
        getConsole().print("No help available for %s", cmd);
        return -1;
    }

    return 0;
}

