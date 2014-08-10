/*!
 * \file src/Command.cpp
 * \brief OpenRaider command implementation
 *
 * \author xythobuz
 */

#include <fstream>
#include <sstream>

#include "global.h"
#include "Camera.h"
#include "Console.h"
#include "Entity.h"
#include "Font.h"
#include "Game.h"
#include "math/math.h"
#include "Menu.h"
#include "Render.h"
#include "Sound.h"
#include "TombRaider.h"
#include "Window.h"
#include "World.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "OpenRaider.h"

int OpenRaider::loadConfig(const char *config) {
    assert(config != NULL);
    assert(config[0] != '\0');

    char *configFile = fullPath(config, 0);
    getConsole() << "Loading config from \"" << configFile << "\"..." << Console::endl;

    std::ifstream file(configFile);
    if (!file) {
        getConsole() << "Could not open file!" << Console::endl;
        return -1;
    }

    for (std::string line; std::getline(file, line);) {
        if (line.length() == 0)
            continue;

        int error = command(line);
        if (error != 0)
            getConsole() << "Error Code: " << error << Console::endl;
    }

    file.close();

    return 0;
}

int OpenRaider::command(std::string c) {
    // Remove comment, if any
    size_t comment = c.find_first_of('#');
    if (comment != std::string::npos)
        c.erase(comment);

    // Execute command
    std::stringstream command(c);
    std::string cmd;
    command >> cmd;
    command >> std::boolalpha >> std::ws;

    if (cmd.length() == 0)
        return 0;

    if (cmd.compare("set") == 0) {
        return set(command);
    } else if (cmd.compare("bind") == 0) {
        std::string a, b;
        if (!(command >> a >> b)) {
            getConsole() << "Invalid use of bind-command" << Console::endl;
            return -1;
        } else {
            return bind(a.c_str(), b.c_str());
        }
    } else if (cmd.compare("quit") == 0) {
        exit(0);
    } else if (cmd.compare("load") == 0) {
        if (!mRunning) {
            getConsole() << "Use load command interactively!" << Console::endl;
            return -999;
        }
        std::string temp;
        command >> temp;
        int error = getGame().loadLevel(temp.c_str());
        return error;
    } else if (cmd.compare("help") == 0) {
        std::string tmp;
        if (!(command >> tmp)) {
            getConsole() << "Available commands:" << Console::endl;
            getConsole() << "  load      - load a level" << Console::endl;
            getConsole() << "  set       - set a parameter" << Console::endl;
            getConsole() << "  bind      - bind a keyboard/mouse action" << Console::endl;
            getConsole() << "  animate   - [BOOL|n|p] - Animate models" << Console::endl;
            getConsole() << "  move      - [walk|fly|noclip]" << Console::endl;
/*
            getConsole() << "  sshot     - make a screenshot" << Console::endl;
            getConsole() << "  sound     - INT - Test play sound" << Console::endl;
            getConsole() << "  mode      - MODE - Render mode" << Console::endl;
            getConsole() << "  light     - BOOL - GL Lights" << Console::endl;
            getConsole() << "  fog       - BOOL - GL Fog" << Console::endl;
            getConsole() << "  viewmodel - INT - Change Laras model" << Console::endl;
            getConsole() << "  pos       - Print position info" << Console::endl;
            getConsole() << "  ralpha    - BOOL - Room Alpha" << Console::endl;
            getConsole() << "  upf       - BOOL - Update Room List Per Frame" << Console::endl;
            getConsole() << "  entmodel  - BOOL" << Console::endl;
            getConsole() << "  ponytail  - BOOL" << Console::endl;
            getConsole() << "  pigtail   - BOOL" << Console::endl;
            getConsole() << "  ponypos   - FLOAT FLOAT FLOAT FLOAT - x y z angle" << Console::endl;
*/
            getConsole() << "  help      - print command help" << Console::endl;
            getConsole() << "  quit      - exit OpenRaider" << Console::endl;
            getConsole() << "Use help COMMAND to get additional info" << Console::endl;
            getConsole() << "Pass BOOLs as true or false" << Console::endl;
        } else {
            return help(tmp);
        }
    } else if (cmd.compare("animate") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole() << "Use animate command interactively!" << Console::endl;
            return -999;
        }
        if (command.peek() == 'n') {
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
                getConsole() << "Animations need to be enabled!" << Console::endl;
            }
        } else if (command.peek() == 'p') {
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
                getConsole() << "Animations need to be enabled!" << Console::endl;
            }
        } else {
            // Enable or disable animating all skeletal models
            bool b = false;
            if (!(command >> b)) {
                getConsole() << "Pass BOOL to animate command!" << Console::endl;
                return -2;
            }
            if (b)
                getRender().setFlags(Render::fAnimateAllModels);
            else
                getRender().clearFlags(Render::fAnimateAllModels);
            getConsole() << (b ? "Animating all models" : "No longer animating all models") << Console::endl;
        }
    } else if (cmd.compare("move") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole() << "Use move command interactively!" << Console::endl;
            return -999;
        }
        std::string temp;
        command >> temp;
        if (temp.compare("walk") == 0) {
            getGame().getLara().setMoveType(Entity::MoveTypeWalk);
        } else if (temp.compare("fly") == 0) {
            getGame().getLara().setMoveType(Entity::MoveTypeFly);
        } else if (temp.compare("noclip") == 0) {
            getGame().getLara().setMoveType(Entity::MoveTypeNoClipping);
        } else {
            getConsole() << "Invalid use of move command (" << temp.c_str() << ")!" << Console::endl;
            return -9;
        }
        getConsole() << temp.c_str()  << "ing" << Console::endl;

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
        getConsole() << "Unknown command: " << cmd.c_str() << Console::endl;
        return -50;
    }

    return 0;
}

int OpenRaider::help(std::string &cmd) {
    if (cmd.compare("set") == 0) {
        getConsole() << "set-Command Usage:" << Console::endl;
        getConsole() << "  set VAR VAL" << Console::endl;
        getConsole() << "Available Variables:" << Console::endl;
        getConsole() << "  basedir    STRING" << Console::endl;
        getConsole() << "  pakdir     STRING" << Console::endl;
        getConsole() << "  audiodir   STRING" << Console::endl;
        getConsole() << "  datadir    STRING" << Console::endl;
        getConsole() << "  font       STRING" << Console::endl;
        getConsole() << "  size       INT INT" << Console::endl;
        getConsole() << "  fullscreen BOOL" << Console::endl;
        getConsole() << "  audio      BOOL" << Console::endl;
        getConsole() << "  volume     BOOL" << Console::endl;
        getConsole() << "  mouse_x    FLOAT" << Console::endl;
        getConsole() << "  mouse_y    FLOAT" << Console::endl;
        getConsole() << "  fps        BOOL" << Console::endl;
        getConsole() << "Enclose STRINGs with \"\"!" << Console::endl;
    } else if (cmd.compare("bind") == 0) {
        getConsole() << "bind-Command Usage:" << Console::endl;
        getConsole() << "  bind ACTION KEY" << Console::endl;
        getConsole() << "Available Actions:" << Console::endl;
        getConsole() << "  menu" << Console::endl;
        getConsole() << "  console" << Console::endl;
        getConsole() << "  forward" << Console::endl;
        getConsole() << "  backward" << Console::endl;
        getConsole() << "  left" << Console::endl;
        getConsole() << "  right" << Console::endl;
        getConsole() << "  jump" << Console::endl;
        getConsole() << "  crouch" << Console::endl;
        getConsole() << "  use" << Console::endl;
        getConsole() << "  holster" << Console::endl;
        getConsole() << "  walk" << Console::endl;
        getConsole() << "Key-Format:" << Console::endl;
        getConsole() << "  'a' or '1'    for character/number keys" << Console::endl;
        getConsole() << "  \"leftctrl\"  for symbols and special keys" << Console::endl;
    } else if (cmd.compare("load") == 0) {
        getConsole() << "load-Command Usage:" << Console::endl;
        getConsole() << "  load /path/to/level" << Console::endl;
/*
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
*/
    } else if (cmd.compare("animate") == 0) {
        getConsole() << "animate-Command Usage:" << Console::endl;
        getConsole() << "  animate [n|p|BOOL]" << Console::endl;
        getConsole() << "Where the commands have the following meaning:" << Console::endl;
        getConsole() << "  BOOL to (de)activate animating all models" << Console::endl;
        getConsole() << "  n to step all models to their next animation" << Console::endl;
        getConsole() << "  p to step all models to their previous animation" << Console::endl;
    } else {
        getConsole() << "No help available for " << cmd.c_str() << Console::endl;
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
    command >> var;

    if (var.compare("size") == 0) {
        unsigned int w = DEFAULT_WIDTH, h = DEFAULT_HEIGHT;
        if (!(command >> w >> h)) {
            getConsole() << "set-size-Error: Invalid value(s)" << Console::endl;
            return -2;
        }
        getWindow().setSize(w, h);
    } else if (var.compare("fullscreen") == 0) {
        bool fullscreen = false;
        if (!(command >> fullscreen)) {
            getConsole() << "set-fullscreen-Error: Invalid value" << Console::endl;
            return -3;
        }
        getWindow().setFullscreen(fullscreen);
    } else if (var.compare("audio") == 0) {
        bool audio = false;
        if (!(command >> audio)) {
            getConsole() << "set-audio-Error: Invalid value" << Console::endl;
            return -4;
        }
        getSound().setEnabled(audio);
    } else if (var.compare("volume") == 0) {
        float vol = 1.0f;
        if (!(command >> vol)) {
            getConsole() << "set-volume-Error: Invalid value" << Console::endl;
            return -5;
        }
        getSound().setVolume(vol);
    } else if (var.compare("mouse_x") == 0) {
        float sense = 1.0f;
        if (!(command >> sense)) {
            getConsole() << "set-mouse_x-Error: Invalid value" << Console::endl;
            return -6;
        }
        getCamera().setSensitivityX(OR_DEG_TO_RAD(sense));
    } else if (var.compare("mouse_y") == 0) {
        float sense = 1.0f;
        if (!(command >> sense)) {
            getConsole() << "set-mouse_y-Error: Invalid value" << Console::endl;
            return -7;
        }
        getCamera().setSensitivityY(OR_DEG_TO_RAD(sense));
    } else if (var.compare("fps") == 0) {
        bool fps = false;
        if (!(command >> fps)) {
            getConsole() << "set-fps-Error: Invalid value" << Console::endl;
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
        getFont().setFont(tmp);
        delete [] tmp;
        delete [] quotes;
    } else {
        getConsole() << "set-Error: Unknown variable (" << var.c_str() << ")" << Console::endl;
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
        getConsole() << "bind-Error: Unknown action (" << key << " --> " << action << ")" << Console::endl;
        return -1;
    }

    KeyboardButton c = stringToKeyboardButton(key);
    if (c == unknownKey) {
        getConsole() << "bind-Error: Unknown key (" << key << ")" << Console::endl;
        return -2;
    }

    keyBindings[e] = c;
    return 0;
}

