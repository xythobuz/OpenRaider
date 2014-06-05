/*!
 * \file src/Command.cpp
 * \brief OpenRaider command implementation
 *
 * \author xythobuz
 */

#include <fstream>

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

int OpenRaider::loadConfig(const char *config) {
    assert(config != NULL);
    assert(config[0] != '\0');

    char *configFile = fullPath(config, 0);
    getConsole().print("Loading config from \"%s\"...", configFile);

    std::ifstream file(configFile);
    if (!file) {
        getConsole().print("Could not open file!");
        return -1;
    }

    for (std::string line; std::getline(file, line);) {
        if (line.length() == 0)
            continue;

        int error = command(line);
        if (error != 0)
            getConsole().print("Error Code: %d", error);
    }

    file.close();

    return 0;
}

int OpenRaider::command(const char *command) {
    std::string tmp(command);
    return this->command(tmp);
}

int OpenRaider::command(std::string &c) {
    // Remove comment, if any
    size_t comment = c.find_first_of('#');
    if (comment != std::string::npos)
        c.erase(comment);

    // Execute command
    std::stringstream command(c);
    std::string cmd;
    command >> cmd;

    if (cmd.length() == 0)
        return 0;

    if (cmd.compare("set") == 0) {
        return set(command);
    } else if (cmd.compare("bind") == 0) {
        std::string a, b;
        if (!(command >> a >> b)) {
            getConsole().print("Invalid use of bind-command");
            return -1;
        } else {
            return bind(a.c_str(), b.c_str());
        }
    } else if (cmd.compare("quit") == 0) {
        exit(0);
    } else if (cmd.compare("load") == 0) {
        if (!mRunning) {
            getConsole().print("Use load command interactively!");
            return -999;
        }
        std::string temp;
        command >> temp;
        char *tmp = bufferString("%s/%s", mPakDir, temp.c_str());
        int error = getGame().loadLevel(tmp);
        delete [] tmp;
        return error;
    } else if (cmd.compare("help") == 0) {
        std::string tmp;
        if (!(command >> tmp)) {
            getConsole().print("Available commands:");
            getConsole().print("  load      - load a level");
            getConsole().print("  set       - set a parameter");
            getConsole().print("  bind      - bind a keyboard/mouse action");
/*
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
*/
            getConsole().print("  help      - print command help");
            getConsole().print("  quit      - exit OpenRaider");
            getConsole().print("Use help COMMAND to get additional info");
        } else {
            return help(tmp);
        }
/*
    } else if (cmd.compare("mode") == 0) {
        std::string mode;
        command >> mode;
        if (!getGame().isLoaded()) {
            getConsole().print("Load a level to set the mode!");
            return -1;
        }
        if (mode.compare("wireframe") == 0) {
            getRender().setMode(Render::modeWireframe);
            getConsole().print("Wireframe mode");
        } else if (mode.compare("solid") == 0) {
            getRender().setMode(Render::modeSolid);
            getConsole().print("Solid mode");
        } else if (mode.compare("texture") == 0) {
            getRender().setMode(Render::modeTexture);
            getConsole().print("Texture mode");
        } else if (mode.compare("vertexlight") == 0) {
            getRender().setMode(Render::modeVertexLight);
            getConsole().print("Vertexlight mode");
        } else if (mode.compare("titlescreen") == 0) {
            getRender().setMode(Render::modeLoadScreen);
            getConsole().print("Titlescreen mode");
        } else {
            getConsole().print("Invalid use of mode command (%s)!", mode.c_str());
            return -1;
        }
    } else if (cmd.compare("light") == 0) {
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
    } else if (cmd.compare("fog") == 0) {
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
    } else if (cmd.compare("vmodel") == 0) {
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
    } else if (cmd.compare("ralpha") == 0) {
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
    } else if (cmd.compare("vis") == 0) {
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
    } else if (cmd.compare("upf") == 0) {
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
    } else if (cmd.compare("entmodel") == 0) {
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
    } else if (cmd.compare("oneroom") == 0) {
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
    } else if (cmd.compare("allrooms") == 0) {
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
    } else if (cmd.compare("ponytail") == 0) {
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
    } else if (cmd.compare("sshot") == 0) {
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
    } else if (cmd.compare("move") == 0) {
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
    } else if (cmd.compare("sound") == 0) {
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
    } else if (cmd.compare("animate") == 0) {
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
    } else if (cmd.compare("viewmodel") == 0) {
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
    } else if (cmd.compare("pos") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole().print("Use pos command interactively!");
            return -21;
        }
        getGame().getLara().print();
    } else if (cmd.compare("pigtail") == 0) {
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
    } else if (cmd.compare("ponypos") == 0) {
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
*/
    } else {
        getConsole().print("Unknown command: %s", cmd.c_str());
        return -50;
    }

    return 0;
}

int OpenRaider::help(std::string &cmd) {
    if (cmd.compare("set") == 0) {
        getConsole().print("set-Command Usage:");
        getConsole().print("  set VAR VAL");
        getConsole().print("Available Variables:");
        getConsole().print("  basedir    STRING");
        getConsole().print("  pakdir     STRING");
        getConsole().print("  audiodir   STRING");
        getConsole().print("  datadir    STRING");
        getConsole().print("  font       STRING");
        getConsole().print("  gldriver   STRING");
        getConsole().print("  size       INT INT");
        getConsole().print("  fullscreen BOOL");
        getConsole().print("  audio      BOOL");
        getConsole().print("  volume     BOOL");
        getConsole().print("  mouse_x    FLOAT");
        getConsole().print("  mouse_y    FLOAT");
        getConsole().print("  fps        BOOL");
        getConsole().print("Enclose STRINGs with \"\"!");
    } else if (cmd.compare("bind") == 0) {
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
    } else if (cmd.compare("load") == 0) {
        getConsole().print("load-Command Usage:");
        getConsole().print("  load levelfile.name");
/*
    } else if (cmd.compare("game") == 0) {
        getConsole().print("Use \"game help\" for more info");
    } else if (cmd.compare("sshot") == 0) {
        getConsole().print("sshot-Command Usage:");
        getConsole().print("  sshot [console|menu]");
        getConsole().print("Add console/menu to capture them too");
    } else if (cmd.compare("sound") == 0) {
        getConsole().print("sound-Command Usage:");
        getConsole().print("  sound INT");
        getConsole().print("Where INT is a valid sound ID integer");
    } else if (cmd.compare("move") == 0) {
        getConsole().print("move-Command Usage:");
        getConsole().print("  move COMMAND");
        getConsole().print("Where COMMAND is one of the following:");
        getConsole().print("  walk");
        getConsole().print("  fly");
        getConsole().print("  noclip");
    } else if (cmd.compare("mode") == 0) {
        getConsole().print("mode-Command Usage:");
        getConsole().print("  mode MODE");
        getConsole().print("Where MODE is one of the following:");
        getConsole().print("  wireframe");
        getConsole().print("  solid");
        getConsole().print("  texture");
        getConsole().print("  vertexlight");
        getConsole().print("  titlescreen");
    } else if (cmd.compare("animate") == 0) {
        getConsole().print("animate-Command Usage:");
        getConsole().print("  animate [n|p|BOOL]");
        getConsole().print("Where the commands have the following meaning:");
        getConsole().print("  BOOL to (de)activate animating all models");
        getConsole().print("  n to step all models to their next animation");
        getConsole().print("  p to step all models to their previous animation");
*/
    } else {
        getConsole().print("No help available for %s", cmd.c_str());
        return -1;
    }

    return 0;
}

char *OpenRaider::expandDirectoryNames(const char *s) {
    assert(s != NULL);
    assert(s[0] != '\0');

    char *result = bufferString("%s", s);

    if (mPakDir != NULL) {
        char *tmp = stringReplace(s, "$(pakdir)", mPakDir);
        delete [] result;
        result = tmp;
    }

    if (mAudioDir != NULL) {
        char *tmp = stringReplace(s, "$(audiodir)", mAudioDir);
        delete [] result;
        result = tmp;
    }

    if (mDataDir != NULL) {
        char *tmp = stringReplace(s, "$(datadir)", mDataDir);
        delete [] result;
        result = tmp;
    }

    if (mBaseDir != NULL) {
        char *tmp = stringReplace(result, "$(basedir)", mBaseDir);
        delete [] result;
        result = tmp;
    }

    return result;
}

#define CHANGE_DIR_WITH_EXPANSION(a) do {     \
    std::string temp;                         \
    command >> temp;                          \
    const char *value = temp.c_str();         \
    char *quotes = stringRemoveQuotes(value); \
    char *tmp = expandDirectoryNames(quotes); \
    a = fullPath(tmp, 0);                     \
    delete [] tmp;                            \
    delete [] quotes;                         \
} while(false)

int OpenRaider::set(std::istream &command) {
    std::string var;
    command >> var >> std::boolalpha;

    if (var.compare("size") == 0) {
        unsigned int w = DEFAULT_WIDTH, h = DEFAULT_HEIGHT;
        if (!(command >> w >> h)) {
            getConsole().print("set-size-Error: Invalid value(s)");
            return -2;
        }
        getWindow().setSize(w, h);
    } else if (var.compare("fullscreen") == 0) {
        bool fullscreen = false;
        if (!(command >> fullscreen)) {
            getConsole().print("set-fullscreen-Error: Invalid value");
            return -3;
        }
        getWindow().setFullscreen(fullscreen);
    } else if (var.compare("gldriver") == 0) {
        std::string driver;
        command >> driver;
        getWindow().setDriver(driver.c_str());
    } else if (var.compare("audio") == 0) {
        bool audio = false;
        if (!(command >> audio)) {
            getConsole().print("set-audio-Error: Invalid value");
            return -4;
        }
        getSound().setEnabled(audio);
    } else if (var.compare("volume") == 0) {
        float vol = 1.0f;
        if (!(command >> vol)) {
            getConsole().print("set-volume-Error: Invalid value");
            return -5;
        }
        getSound().setVolume(vol);
    } else if (var.compare("mouse_x") == 0) {
        float sense = 1.0f;
        if (!(command >> sense)) {
            getConsole().print("set-mouse_x-Error: Invalid value");
            return -6;
        }
        getCamera().setSensitivityX(OR_DEG_TO_RAD(sense));
    } else if (var.compare("mouse_y") == 0) {
        float sense = 1.0f;
        if (!(command >> sense)) {
            getConsole().print("set-mouse_y-Error: Invalid value");
            return -7;
        }
        getCamera().setSensitivityY(OR_DEG_TO_RAD(sense));
    } else if (var.compare("fps") == 0) {
        bool fps = false;
        if (!(command >> fps)) {
            getConsole().print("set-fps-Error: Invalid value");
            return -8;
        }
        mFPS = fps;
    } else if (var.compare("basedir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(mBaseDir);
    } else if (var.compare("pakdir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(mPakDir);
    } else if (var.compare("audiodir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(mAudioDir);
    } else if (var.compare("datadir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(mDataDir);
    } else if (var.compare("font") == 0) {
        std::string temp;
        command >> temp;
        const char *value = temp.c_str();
        char *quotes = stringReplace(value, "\"", "");
        char *tmp = expandDirectoryNames(quotes);
        getWindow().setFont(tmp);
        delete [] tmp;
        delete [] quotes;
    } else {
        getConsole().print("set-Error: Unknown variable (%s)", var.c_str());
        return -1;
    }

    return 0;
}

int OpenRaider::bind(const char *action, const char *key) {
    assert(action != NULL);
    assert(action[0] != '\0');
    assert(key != NULL);
    assert(key[0] != '\0');

    ActionEvents e = stringToActionEvent(action);
    if (e == ActionEventCount) {
        getConsole().print("bind-Error: Unknown action (%s --> %s)", key, action);
        return -1;
    }

    KeyboardButton c = stringToKeyboardButton(key);
    if (c == unknownKey) {
        getConsole().print("bind-Error: Unknown key (%s)", key);
        return -2;
    }

    keyBindings[e] = c;
    return 0;
}

