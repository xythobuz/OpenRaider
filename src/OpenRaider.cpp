/*!
 * \file src/OpenRaider.cpp
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#include <cstdio>
#include <cstring>
#include <assert.h>
#include <dirent.h>

#include "WindowSDL.h"

#include "config.h"
#include "Console.h"
#include "Game.h"
#include "main.h"
#include "math/math.h"
#include "Menu.h"
#include "Sound.h"
#include "TombRaider.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "OpenRaider.h"

OpenRaider::OpenRaider() {
    mRunning = false;
    mFPS = false;
    mBaseDir = NULL;
    mPakDir = NULL;
    mAudioDir = NULL;
    mDataDir = NULL;
    mMapListFilled = false;

    for (int i = 0; i < ActionEventCount; i++)
        keyBindings[i] = unknown;

    mCameraRotationDeltaX = OR_DEG_TO_RAD(1.0f);
    mCameraRotationDeltaY = OR_DEG_TO_RAD(1.0f);
}

OpenRaider::~OpenRaider() {
    if (mBaseDir)
        delete mBaseDir;

    if (mPakDir)
        delete mPakDir;

    if (mAudioDir)
        delete mAudioDir;

    if (mDataDir)
        delete mDataDir;

    while (mMapList.size() > 0) {
        delete [] mMapList.back();
        mMapList.pop_back();
    }
}

int OpenRaider::loadConfig(const char *config) {
    assert(config != NULL);
    assert(config[0] != '\0');

    char *configFile = fullPath(config, 0);
    getConsole().print("Loading config from \"%s\"...", configFile);

    FILE *f = fopen(configFile, "r");
    if (f == NULL) {
        getConsole().print("Could not open file!");
        return -1;
    }

    char buffer[256];
    while (fgets(buffer, 256, f) != NULL) {
        int error = command(buffer);
        if (error != 0) {
            getConsole().print("Error Code: %d", error);
        }
    }

    fclose(f);

    return 0;
}

int OpenRaider::command(const char *command) {
    assert(command != NULL);
    assert(command[0] != '\0');

    int returnValue = 0;
    char *cmd = bufferString("%s", command);
    size_t length = strlen(cmd);

    // Command ends at '\n' or # when a comment begins
    for (size_t i = 0; i < length; i++) {
        if (cmd[i] == '\n' || cmd[i] == '#') {
            cmd[i] = '\0';
            break;
        }
    }

    char *token = strtok(cmd, " \t");
    if (token != NULL) {
        // token is the command to execute
        // get arguments
        std::vector<char *> args;
        char *next;
        while ((next = strtok(NULL, " \t")) != NULL) {
            args.push_back(next);
        }

        // Execute
        returnValue = this->command(token, &args);
    }

    free(cmd);
    return returnValue;
}

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
        char *tmp = bufferString("%s/%s", mPakDir, args->at(0));
        int error = getGame().loadLevel(tmp);
        delete [] tmp;
        return error;
    } else if (strcmp(command, "sshot") == 0) {
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
                if (getGame().mLoaded) {
                    getRender().setMode(Render::modeWireframe);
                    getConsole().print("Wireframe mode");
                } else {
                    getConsole().print("Load a level to set this mode!");
                    return -3;
                }
            } else if (strcmp(mode, "solid") == 0) {
                if (getGame().mLoaded) {
                    getRender().setMode(Render::modeSolid);
                    getConsole().print("Solid mode");
                } else {
                    getConsole().print("Load a level to set this mode!");
                    return -4;
                }
            } else if (strcmp(mode, "texture") == 0) {
                if (getGame().mLoaded) {
                    getRender().setMode(Render::modeTexture);
                    getConsole().print("Texture mode");
                } else {
                    getConsole().print("Load a level to set this mode!");
                    return -5;
                }
            } else if (strcmp(mode, "vertexlight") == 0) {
                if (getGame().mLoaded) {
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
        if (args->size() > 0) {
            if (getGame().mLoaded) {
                char *move = args->at(0);
                if (strcmp(move, "walk") == 0) {
                    getGame().mLara->moveType = worldMoveType_walk;
                    getConsole().print("Lara is walking...");
                } else if (strcmp(move, "fly") == 0) {
                    getGame().mLara->moveType = worldMoveType_fly;
                    getConsole().print("Lara is flying...");
                } else if (strcmp(move, "noclip") == 0) {
                    getGame().mLara->moveType = worldMoveType_noClipping;
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
        if (args->size() > 0) {
            getSound().play(atoi(args->at(0)));
        } else {
            getConsole().print("Invalid use of sound command!");
            return -12;
        }
    } else if (strcmp(command, "animate") == 0) {
        if (args->size() > 0) {
            char c = args->at(0)[0];
            if (c == 'n') {
                // Step all skeletal models to their next animation
                if (getRender().getFlags() & Render::fAnimateAllModels) {
                    for (unsigned int i = 0; i < getRender().mModels.size(); i++) {
                        SkeletalModel *m = getRender().mModels[i];
                        if (m->getAnimation() < ((int)m->model->animation.size() - 1))
                            m->setAnimation(m->getAnimation() + 1);
                        else
                            if (m->getAnimation() != 0)
                                m->setAnimation(0);
                    }
                } else {
                    getConsole().print("Animations need to be enabled!");
                }
            } else if (c == 'p') {
                // Step all skeletal models to their previous animation
                if (getRender().getFlags() & Render::fAnimateAllModels) {
                    for (unsigned int i = 0; i < getRender().mModels.size(); i++) {
                        SkeletalModel *m = getRender().mModels[i];
                        if (m->getAnimation() > 0)
                            m->setAnimation(m->getAnimation() - 1);
                        else
                            if (m->model->animation.size() > 0)
                                m->setAnimation(m->model->animation.size() - 1);
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
    } else if (strcmp(command, "hop") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to hop command!");
                return -19;
            }
            if (b)
                getWorld().setFlag(World::fEnableHopping);
            else
                getWorld().clearFlag(World::fEnableHopping);
            getConsole().print("Room hopping is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of hop-command!");
            return -20;
        }
    } else if (strcmp(command, "viewmodel") == 0)  {
        if (getGame().mLara) {
            SkeletalModel *smdl = static_cast<SkeletalModel *>(getGame().mLara->tmpHook);
            skeletal_model_t *mdl = getWorld().getModel(atoi(args->at(0)));
            if (smdl)
                smdl->setModel(mdl);
        }
        //m_render.ViewModel(LARA, atoi(cmd));
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
            getConsole().print("  hop       - BOOL - Room hop");
            getConsole().print("  viewmodel - INT - Change Laras model");
            getConsole().print("  help      - print command help");
            getConsole().print("  quit      - exit OpenRaider");
            getConsole().print("Use help COMMAND to get additional info");
        } else if (args->size() == 1) {
            return help(args->at(0));
        } else {
            getConsole().print("Invalid use of help-command");
            return -21;
        }
    } else {
        getConsole().print("Unknown command: %s ", command);
        return -22;
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

char *OpenRaider::expandDirectoryNames(const char *s) {
    const char *base = "$(basedir)";
    const char *pak = "$(pakdir)";
    const char *audio = "$(audiodir)";
    const char *data = "$(datadir)";

    assert(s != NULL);
    assert(s[0] != '\0');

    if (mBaseDir != NULL) {
        if (strstr(s, base) != NULL) {
            return stringReplace(s, base, mBaseDir);
        }
    }

    if (mPakDir != NULL) {
        if (strstr(s, pak) != NULL) {
            return stringReplace(s, pak, mPakDir);
        }
    }

    if (mAudioDir != NULL) {
        if (strstr(s, audio) != NULL) {
            return stringReplace(s, audio, mAudioDir);
        }
    }

    if (mDataDir != NULL) {
        if (strstr(s, data) != NULL) {
            return stringReplace(s, data, mDataDir);
        }
    }

    return NULL;
}

#define CHANGE_DIR_WITH_EXPANSION(a) do {     \
    char *quotes = stringRemoveQuotes(value); \
    char *tmp = expandDirectoryNames(quotes); \
    if (tmp == NULL) {                        \
        a = fullPath(quotes, 0);              \
    } else {                                  \
        a = fullPath(tmp, 0);                 \
        delete [] tmp;                        \
    }                                         \
    delete [] quotes;                         \
} while(false)

int OpenRaider::set(const char *var, const char *value) {
    assert(var != NULL);
    assert(var[0] != '\0');
    assert(value != NULL);
    assert(value[0] != '\0');

    if (strcmp(var, "size") == 0) {
        // value has format like "\"1024x768\""
        unsigned int w = DEFAULT_WIDTH, h = DEFAULT_HEIGHT;
        if (sscanf(value, "\"%dx%d\"", &w, &h) != 2) {
            getConsole().print("set-size-Error: Invalid value (%s)", value);
            return -2;
        }
        getWindow().setSize(w, h);
    } else if (strcmp(var, "fullscreen") == 0) {
        bool fullscreen = false;
        if (readBool(value, &fullscreen) != 0) {
            getConsole().print("set-fullscreen-Error: Invalid value (%s)", value);
            return -3;
        }
        getWindow().setFullscreen(fullscreen);
    } else if (strcmp(var, "gldriver") == 0) {
        getWindow().setDriver(value);
    } else if (strcmp(var, "audio") == 0) {
        bool audio = false;
        if (readBool(value, &audio) != 0) {
            getConsole().print("set-audio-Error: Invalid value (%s)", value);
            return -4;
        }
        getSound().setEnabled(audio);
    } else if (strcmp(var, "volume") == 0) {
        float vol = 1.0f;
        if (sscanf(value, "%f", &vol) != 1) {
            getConsole().print("set-volume-Error: Invalid value (%s)", value);
            return -5;
        }
        getSound().setVolume(vol);
    } else if (strcmp(var, "mouse_x") == 0) {
        float sense = 1.0f;
        if (sscanf(value, "%f", &sense) != 1) {
            getConsole().print("set-mouse_x-Error: Invalid value (%s)", value);
            return -6;
        }
        mCameraRotationDeltaX = OR_DEG_TO_RAD(sense);
    } else if (strcmp(var, "mouse_y") == 0) {
        float sense = 1.0f;
        if (sscanf(value, "%f", &sense) != 1) {
            getConsole().print("set-mouse_y-Error: Invalid value (%s)", value);
            return -7;
        }
        mCameraRotationDeltaY = OR_DEG_TO_RAD(sense);
    } else if (strcmp(var, "fps") == 0) {
        bool fps = false;
        if (readBool(value, &fps) != 0) {
            getConsole().print("set-fps-Error: Invalid value (%s)", value);
            return -8;
        }
        mFPS = fps;
    } else if (strcmp(var, "basedir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(mBaseDir);
    } else if (strcmp(var, "pakdir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(mPakDir);
    } else if (strcmp(var, "audiodir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(mAudioDir);
    } else if (strcmp(var, "datadir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(mDataDir);
    } else if (strcmp(var, "font") == 0) {
        char *quotes = stringReplace(value, "\"", "");
        char *tmp = expandDirectoryNames(quotes);
        if (tmp == NULL) {
            getWindow().setFont(quotes);
        } else {
            getWindow().setFont(tmp);
            delete [] tmp;
        }
        delete [] quotes;
    } else {
        getConsole().print("set-Error: Unknown variable (%s = %s)", var, value);
        return -1;
    }

    return 0;
}

int OpenRaider::bind(const char *action, const char *key) {
    assert(action != NULL);
    assert(action[0] != '\0');
    assert(key != NULL);
    assert(key[0] != '\0');

    const char *tmp = action;
    if (action[0] == '+')
        tmp++;

    if (strcmp(tmp, "menu") == 0) {
        return bind(menuAction, key);
    } else if (strcmp(tmp, "console") == 0) {
        return bind(consoleAction, key);
    } else if (strcmp(tmp, "forward") == 0) {
        return bind(forwardAction, key);
    } else if (strcmp(tmp, "backward") == 0) {
        return bind(backwardAction, key);
    } else if (strcmp(tmp, "left") == 0) {
        return bind(leftAction, key);
    } else if (strcmp(tmp, "right") == 0) {
        return bind(rightAction, key);
    } else if (strcmp(tmp, "jump") == 0) {
        return bind(jumpAction, key);
    } else if (strcmp(tmp, "crouch") == 0) {
        return bind(crouchAction, key);
    } else if (strcmp(tmp, "use") == 0) {
        return bind(useAction, key);
    } else if (strcmp(tmp, "holster") == 0) {
        return bind(holsterAction, key);
    } else {
        getConsole().print("bind-Error: Unknown action (%s --> %s)", key, action);
        return -1;
    }
}

int OpenRaider::bind(ActionEvents action, const char *key) {
    assert(action < ActionEventCount);
    assert(key != NULL);
    assert(key[0] != '\0');

    size_t length = strlen(key);
    if ((key[0] == '\'') && (key[length - 1] == '\'') && (length == 3)) {
        // Literal character like w, a, s, d, 0, 1...
        char c = key[1];
        if (((c >= '0') && (c <= '9'))
            || ((c >= 'a') && (c <= 'z'))) {
            keyBindings[action] = (KeyboardButton)c;
        } else {
            getConsole().print("bind-\'\'-Error: Unknown key (%s)", key);
            return -1;
        }
    } else if ((key[0] == '\"') && (key[length - 1] == '\"')) {
        // Special characters like tilde, esc, quote...
        char *tmp = stringRemoveQuotes(key);
        if (strcmp(tmp, "quote") == 0) {
            keyBindings[action] = quote;
        } else if (strcmp(tmp, "backslash") == 0) {
            keyBindings[action] = quote;
        } else if (strcmp(tmp, "backspace") == 0) {
            keyBindings[action] = backspace;
        } else if (strcmp(tmp, "capslock") == 0) {
            keyBindings[action] = capslock;
        } else if (strcmp(tmp, "comma") == 0) {
            keyBindings[action] = comma;
        } else if (strcmp(tmp, "del") == 0) {
            keyBindings[action] = del;
        } else if (strcmp(tmp, "up") == 0) {
            keyBindings[action] = up;
        } else if (strcmp(tmp, "down") == 0) {
            keyBindings[action] = down;
        } else if (strcmp(tmp, "left") == 0) {
            keyBindings[action] = left;
        } else if (strcmp(tmp, "right") == 0) {
            keyBindings[action] = right;
        } else if (strcmp(tmp, "end") == 0) {
            keyBindings[action] = end;
        } else if (strcmp(tmp, "equals") == 0) {
            keyBindings[action] = equals;
        } else if (strcmp(tmp, "escape") == 0) {
            keyBindings[action] = escape;
        } else if (strcmp(tmp, "f1") == 0) {
            keyBindings[action] = f1;
        } else if (strcmp(tmp, "f2") == 0) {
            keyBindings[action] = f2;
        } else if (strcmp(tmp, "f3") == 0) {
            keyBindings[action] = f3;
        } else if (strcmp(tmp, "f4") == 0) {
            keyBindings[action] = f4;
        } else if (strcmp(tmp, "f5") == 0) {
            keyBindings[action] = f5;
        } else if (strcmp(tmp, "f6") == 0) {
            keyBindings[action] = f6;
        } else if (strcmp(tmp, "f7") == 0) {
            keyBindings[action] = f7;
        } else if (strcmp(tmp, "f8") == 0) {
            keyBindings[action] = f8;
        } else if (strcmp(tmp, "f9") == 0) {
            keyBindings[action] = f9;
        } else if (strcmp(tmp, "f10") == 0) {
            keyBindings[action] = f10;
        } else if (strcmp(tmp, "f11") == 0) {
            keyBindings[action] = f11;
        } else if (strcmp(tmp, "f12") == 0) {
            keyBindings[action] = f12;
        } else if (strcmp(tmp, "backquote") == 0) {
            keyBindings[action] = backquote;
        } else if (strcmp(tmp, "home") == 0) {
            keyBindings[action] = home;
        } else if (strcmp(tmp, "insert") == 0) {
            keyBindings[action] = insert;
        } else if (strcmp(tmp, "leftalt") == 0) {
            keyBindings[action] = leftalt;
        } else if (strcmp(tmp, "leftctrl") == 0) {
            keyBindings[action] = leftctrl;
        } else if (strcmp(tmp, "leftbracket") == 0) {
            keyBindings[action] = leftbracket;
        } else if (strcmp(tmp, "leftgui") == 0) {
            keyBindings[action] = leftgui;
        } else if (strcmp(tmp, "leftshift") == 0) {
            keyBindings[action] = leftshift;
        } else if (strcmp(tmp, "minus") == 0) {
            keyBindings[action] = minus;
        } else if (strcmp(tmp, "numlock") == 0) {
            keyBindings[action] = numlock;
        } else if (strcmp(tmp, "pagedown") == 0) {
            keyBindings[action] = pagedown;
        } else if (strcmp(tmp, "pageup") == 0) {
            keyBindings[action] = pageup;
        } else if (strcmp(tmp, "pause") == 0) {
            keyBindings[action] = pause;
        } else if (strcmp(tmp, "dot") == 0) {
            keyBindings[action] = dot;
        } else if (strcmp(tmp, "rightalt") == 0) {
            keyBindings[action] = rightalt;
        } else if (strcmp(tmp, "rightctrl") == 0) {
            keyBindings[action] = rightctrl;
        } else if (strcmp(tmp, "enter") == 0) {
            keyBindings[action] = enter;
        } else if (strcmp(tmp, "rightgui") == 0) {
            keyBindings[action] = rightgui;
        } else if (strcmp(tmp, "rightbracket") == 0) {
            keyBindings[action] = rightbracket;
        } else if (strcmp(tmp, "rightshift") == 0) {
            keyBindings[action] = rightshift;
        } else if (strcmp(tmp, "scrolllock") == 0) {
            keyBindings[action] = scrolllock;
        } else if (strcmp(tmp, "semicolon") == 0) {
            keyBindings[action] = semicolon;
        } else if (strcmp(tmp, "slash") == 0) {
            keyBindings[action] = slash;
        } else if (strcmp(tmp, "space") == 0) {
            keyBindings[action] = space;
        } else if (strcmp(tmp, "tab") == 0) {
            keyBindings[action] = tab;
        } else if (strcmp(tmp, "leftmouse") == 0) {
            keyBindings[action] = leftmouse;
        } else if (strcmp(tmp, "middlemouse") == 0) {
            keyBindings[action] = middlemouse;
        } else if (strcmp(tmp, "rightmouse") == 0) {
            keyBindings[action] = rightmouse;
        } else {
            getConsole().print("bind-\"\"-Error: Unknown key (%s)", key);
            delete [] tmp;
            return -2;
        }
        delete [] tmp;
    } else {
        getConsole().print("bind-Error: Unknown key (%s)", key);
        return -3;
    }
    return 0;
}

void OpenRaider::loadPakFolderRecursive(const char *dir) {
    struct dirent *ep;
    DIR *pakDir;

    assert(dir != NULL);
    assert(dir[0] != '\0');
    assert(mRunning == true);

    pakDir = opendir(dir);
    if (pakDir != NULL) {
        while ((ep = readdir(pakDir)) != NULL) {
            if (ep->d_type == DT_DIR) {
                if ((strcmp(".", ep->d_name) != 0)
                 && (strcmp("..", ep->d_name) != 0)) {
                    char *tmp = bufferString("%s%s", dir, ep->d_name);
                    char *next = fullPath(tmp, '/');
                    loadPakFolderRecursive(next);
                    delete next;
                    delete tmp;
                }
            } else {
                char *fullPathMap = bufferString("%s%s", dir, ep->d_name);

                char *lowerPath = bufferString("%s", fullPathMap);
                for (char *p = lowerPath; *p; ++p) *p = (char)tolower(*p);

                // Check for valid extension
                if (stringEndsWith(lowerPath, ".phd")
                     || stringEndsWith(lowerPath, ".tr2")
                     || stringEndsWith(lowerPath, ".tr4")
                     || stringEndsWith(lowerPath, ".trc")) {
                    int error = TombRaider::checkMime(fullPathMap);
                    if (error == 0) {
                        // Just load relative filename
                        mMapList.push_back(bufferString("%s", (fullPathMap + strlen(mPakDir) + 1)));
                    } else {
                        getConsole().print("Error: pak file '%s' %s",
                                fullPathMap, (error == -1) ? "not found" : "invalid");
                    }
                }

                delete [] lowerPath;
                delete [] fullPathMap;
            }
        }
        closedir(pakDir);
    } else {
        getConsole().print("Could not open PAK dir %s!", dir);
    }
}

void OpenRaider::fillMapList() {
    assert(mRunning == true);

    char *tmp = fullPath(mPakDir, '/');
    loadPakFolderRecursive(tmp);
    delete [] tmp;
    mMapListFilled = true;
}

void OpenRaider::run() {
    assert(mRunning == false);

    mRunning = true;
    while (mRunning) {
        frame();
    }
}

void OpenRaider::frame() {
    assert(mRunning == true);

    static clock_t fpsSum = 0, fpsCount = 0;
    static int fps = 0;
    clock_t startTime = systemTimerGet();

    // Get keyboard and mouse input
    getWindow().eventHandling();

    // Clear screen
    glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw game scene
    getRender().display();

    // Draw 2D overlays (console and menu)
    getWindow().glEnter2D();

    getConsole().display();
    getMenu().display();

    // Draw FPS counter
    if (mFPS)
        getWindow().drawText(10, getWindow().mHeight - 20, 0.5f, OR_BLUE, "%dFPS", fps);

    getWindow().glExit2D();

    // Put new frame on screen
    getWindow().swapBuffersGL();

    // Fill map list after first render pass,
    // so menu *loading screen* is visible
    if (!mMapListFilled)
        fillMapList();

    // Calculate FPS display value
    fpsCount++;
    fpsSum += (systemTimerGet() - startTime);
    if (fpsSum >= 500) {
        // Update every 500ms
        fps = (int)((float)fpsCount * (1000.0f / (float)fpsSum));
        fpsCount = fpsSum = 0;
    }
}

void OpenRaider::handleKeyboard(KeyboardButton key, bool pressed) {
    assert(key < unknown);
    assert(mRunning == true);

    if ((keyBindings[menuAction] == key) && pressed) {
        getMenu().setVisible(!getMenu().isVisible());
    } else if (!getMenu().isVisible()) {
        if ((keyBindings[consoleAction] == key) && pressed) {
            getConsole().setVisible(!getConsole().isVisible());
        } else if (!getConsole().isVisible()) {
            for (int i = forwardAction; i < ActionEventCount; i++) {
                if (keyBindings[i] == key) {
                    getGame().handleAction((ActionEvents)i, pressed);
                }
            }
        } else {
            getConsole().handleKeyboard(key, pressed);
        }
    } else {
        getMenu().handleKeyboard(key, pressed);
    }

    getWindow().setMousegrab(!(getMenu().isVisible() || getConsole().isVisible()));
}

void OpenRaider::handleText(char *text, bool notFinished) {
    assert(text != NULL);
    assert(text[0] != '\0');
    assert(mRunning == true);

    if ((getConsole().isVisible()) && (!getMenu().isVisible())) {
        getConsole().handleText(text, notFinished);
    }
}

void OpenRaider::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    assert(button < unknown);
    assert(mRunning == true);

    if (getMenu().isVisible()) {
        getMenu().handleMouseClick(x, y, button, released);
    } else if (!getConsole().isVisible()) {
        for (int i = forwardAction; i < ActionEventCount; i++) {
            if (keyBindings[i] == button) {
                getGame().handleAction((ActionEvents)i, !released);
            }
        }
    }
}

void OpenRaider::handleMouseMotion(int xrel, int yrel) {
    assert((xrel != 0) || (yrel != 0));
    assert(mRunning == true);

    if ((!getConsole().isVisible()) && (!getMenu().isVisible())) {
        getGame().handleMouseMotion(xrel, yrel);
    }
}

void OpenRaider::handleMouseScroll(int xrel, int yrel) {
    assert((xrel != 0) || (yrel != 0));
    assert(mRunning == true);

    if ((getConsole().isVisible()) && (!getMenu().isVisible())) {
        getConsole().handleMouseScroll(xrel, yrel);
    }
}

