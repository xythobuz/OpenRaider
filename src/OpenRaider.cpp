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
#include "Menu.h"
#include "Sound.h"
#include "Window.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "OpenRaider.h"

#define MAX_MS_PER_FRAME (1000 / MAXIMUM_FPS)

OpenRaider::OpenRaider() {
    mInit = false;
    mRunning = false;
    mBaseDir = NULL;
    mPakDir = NULL;
    mAudioDir = NULL;
    mDataDir = NULL;
    mMapListFilled = false;

    mMenu = new Menu();
    mConsole = new Console();
    mSound = new Sound();
    mWindow = new WindowSDL();
    mGame = NULL;

    for (int i = 0; i < ActionEventCount; i++)
        keyBindings[i] = unknown;
}

OpenRaider::~OpenRaider() {
    if (mGame)
        delete mGame;

    if (mMenu)
        delete mMenu;

    if (mConsole)
        delete mConsole;

    if (mSound)
        delete mSound;

    if (mWindow)
        delete mWindow;

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
    mConsole->print("Loading config from \"%s\"...", configFile);

    FILE *f = fopen(configFile, "r");
    if (f == NULL) {
        mConsole->print("Could not open file!");
        return -1;
    }

    char buffer[256];
    while (fgets(buffer, 256, f) != NULL) {
        int error = command(buffer);
        if (error != 0) {
            mConsole->print("Error Code: %d", error);
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
            mConsole->print("Invalid use of set-command");
            return -2;
        } else {
            return set(args->at(0), args->at(1));
        }
    } else if (strcmp(command, "bind") == 0) {
        if (args->size() != 2) {
            mConsole->print("Invalid use of bind-command");
            return -3;
        } else {
            return bind(args->at(0), args->at(1));
        }
    } else if (strcmp(command, "quit") == 0) {
        exit(0);
    } else if (strcmp(command, "help") == 0) {
        if (args->size() == 0) {
            mConsole->print("Available commands:");
            mConsole->print("  load");
            mConsole->print("  set");
            mConsole->print("  bind");
            mConsole->print("  help");
            mConsole->print("  quit");
            mConsole->print("Use help COMMAND to get additional info");
        } else if (args->size() == 1) {
            return help(args->at(0));
        } else {
            mConsole->print("Invalid use of help-command");
            return -4;
        }
    } else if (strcmp(command, "load") == 0) {
        if (mGame)
            delete mGame;
        char *tmp = bufferString("%s%s", mPakDir, args->at(0));
        try {
            mGame = new Game(tmp);
            delete [] tmp;
            return 0;
        } catch (int error) {
            delete [] tmp;
            return error;
        }
    } else {
        mConsole->print("Unknown command: %s ", command);
        return -1;
    }

    return 0;
}

int OpenRaider::help(const char *cmd) {
    if (strcmp(cmd, "set") == 0) {
        mConsole->print("set-Command Usage:");
        mConsole->print("  set VAR VAL");
        mConsole->print("Available Variables:");
        mConsole->print("  basedir     STRING");
        mConsole->print("  pakdir      STRING");
        mConsole->print("  audiodir    STRING");
        mConsole->print("  datadir     STRING");
        mConsole->print("  font        STRING");
        mConsole->print("  gldriver    STRING");
        mConsole->print("  size        \"INTxINT\"");
        mConsole->print("  fullscreen  BOOL");
        mConsole->print("  audio       BOOL");
        mConsole->print("  volume      BOOL");
        mConsole->print("  mouse_x     FLOAT");
        mConsole->print("  mouse_y     FLOAT");
        mConsole->print("Enclose STRINGs with \"\"!");
        mConsole->print("size expects a STRING in the specified format");
    } else if (strcmp(cmd, "bind") == 0) {
        mConsole->print("bind-Command Usage:");
        mConsole->print("  bind ACTION KEY");
        mConsole->print("Available Actions:");
        mConsole->print("  menu");
        mConsole->print("  console");
        mConsole->print("  forward");
        mConsole->print("  backward");
        mConsole->print("  left");
        mConsole->print("  right");
        mConsole->print("  jump");
        mConsole->print("  crouch");
        mConsole->print("  use");
        mConsole->print("  holster");
        mConsole->print("Key-Format:");
        mConsole->print("  'a' or '1'    for character/number keys");
        mConsole->print("  \"leftctrl\"  for symbols and special keys");
    } else if (strcmp(cmd, "load") == 0) {
        mConsole->print("load-Command Usage:");
        mConsole->print("  load levelfile.name");
    } else {
        mConsole->print("No help available for %s", cmd);
        return -1;
    }

    return 0;
}

char *OpenRaider::expandDirectoryNames(const char *s) {
    const char *base = "$(basedir)";
    const char *pak = "$(pakdir)";
    const char *audio = "$(audiodir)";
    const char *data = "$(datadir)";

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
    if (strcmp(var, "size") == 0) {
        // value has format like "\"1024x768\""
        unsigned int w = DEFAULT_WIDTH, h = DEFAULT_HEIGHT;
        if (sscanf(value, "\"%dx%d\"", &w, &h) != 2) {
            mConsole->print("set-size-Error: Invalid value (%s)", value);
            return -2;
        }
        mWindow->setSize(w, h);
    } else if (strcmp(var, "fullscreen") == 0) {
        bool fullscreen = false;
        if (readBool(value, &fullscreen) != 0) {
            mConsole->print("set-fullscreen-Error: Invalid value (%s)", value);
            return -3;
        }
        mWindow->setFullscreen(fullscreen);
    } else if (strcmp(var, "gldriver") == 0) {
        mWindow->setDriver(value);
    } else if (strcmp(var, "audio") == 0) {
        bool audio = false;
        if (readBool(value, &audio) != 0) {
            mConsole->print("set-audio-Error: Invalid value (%s)", value);
            return -4;
        }
        mSound->setEnabled(audio);
    } else if (strcmp(var, "volume") == 0) {
        float vol = 1.0f;
        if (sscanf(value, "%f", &vol) != 1) {
            mConsole->print("set-volume-Error: Invalid value (%s)", value);
            return -5;
        }
        mSound->setVolume(vol);
    } else if (strcmp(var, "mouse_x") == 0) {
        float sense = 1.0f;
        if (sscanf(value, "%f", &sense) != 1) {
            mConsole->print("set-mouse_x-Error: Invalid value (%s)", value);
            return -6;
        }
        //! \todo mouse support
    } else if (strcmp(var, "mouse_y") == 0) {
        float sense = 1.0f;
        if (sscanf(value, "%f", &sense) != 1) {
            mConsole->print("set-mouse_y-Error: Invalid value (%s)", value);
            return -7;
        }
        //! \todo mouse support
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
            mWindow->setFont(quotes);
        } else {
            mWindow->setFont(tmp);
            delete [] tmp;
        }
        delete [] quotes;
    } else {
        mConsole->print("set-Error: Unknown variable (%s = %s)", var, value);
        return -1;
    }

    return 0;
}

int OpenRaider::bind(const char *action, const char *key) {
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
        mConsole->print("bind-Error: Unknown action (%s --> %s)", key, action);
        return -1;
    }
}

int OpenRaider::bind(ActionEvents action, const char *key) {
    assert(action != ActionEventCount);
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
            mConsole->print("bind-\'\'-Error: Unknown key (%s)", key);
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
            keyBindings[action] = KeyboardButton::left;
        } else if (strcmp(tmp, "right") == 0) {
            keyBindings[action] = KeyboardButton::right;
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
            mConsole->print("bind-\"\"-Error: Unknown key (%s)", key);
            delete [] tmp;
            return -2;
        }
        delete [] tmp;
    } else {
        mConsole->print("bind-Error: Unknown key (%s)", key);
        return -3;
    }
    return 0;
}

void OpenRaider::loadPakFolderRecursive(const char *dir) {
    struct dirent *ep;
    DIR *pakDir;

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
                    //if (m_tombraider.checkMime(fullPathMap) == 0) {
                        // mConsole->print("Validated pak: '%s'", fullPathMap);

                        // Just load relative filename
                        mMapList.push_back(bufferString("%s", (fullPathMap + strlen(mPakDir))));
                    //} else {
                    //    mConsole->print("ERROR: pak file '%s' not found or invalid", fullPathMap);
                    //}
                }

                delete [] lowerPath;
                delete [] fullPathMap;
            }
        }
        closedir(pakDir);
    } else {
        mConsole->print("Could not open PAK dir %s!", dir);
    }
}

void OpenRaider::fillMapList() {
    char *tmp = fullPath(mPakDir, '/');
    loadPakFolderRecursive(tmp);
    delete [] tmp;
    mMapListFilled = true;
}

int OpenRaider::initialize() {
    assert(mInit == false);
    assert(mRunning == false);

    // Initialize Windowing
    if (mWindow->initialize() != 0)
        return -1;

    // Initialize OpenGL
    if (mWindow->initializeGL() != 0)
        return -2;

    // Initialize window font
    if (mWindow->initializeFont() != 0)
        return -3;

    // Initialize sound
    if (mSound->initialize() != 0)
        return -4;

    mMenu->setVisible(true);

    mInit = true;

    return 0;
}

void OpenRaider::run() {
    assert(mInit == true);
    assert(mRunning == false);

    mRunning = true;
    while (mRunning) {
        clock_t startTime = systemTimerGet();

        // Get keyboard and mouse input
        mWindow->eventHandling();

        // Clear screen
        glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw game scene
        if (mGame)
            mGame->display();

        // Draw 2D overlays (console and menu)
        mWindow->glEnter2D();
        mConsole->display();
        mMenu->display();
        mWindow->glExit2D();

        // Put new frame on screen
        mWindow->swapBuffersGL();

        // Fill map list after first render pass,
        // so menu *loading screen* is visible
        if (!mMapListFilled)
            fillMapList();

        // Check time it took to compute the last frame
        // and delay for an appropriate amount of time
        clock_t stopTime = systemTimerGet();
        if (MAX_MS_PER_FRAME > (stopTime - startTime))
            mWindow->delay(MAX_MS_PER_FRAME - (stopTime - startTime));
    }
}

void OpenRaider::handleKeyboard(KeyboardButton key, bool pressed) {
    if ((keyBindings[menuAction] == key) && pressed) {
        mMenu->setVisible(!mMenu->isVisible());
    } else if (!mMenu->isVisible()) {
        if ((keyBindings[consoleAction] == key) && pressed) {
            mConsole->setVisible(!mConsole->isVisible());
        } else if (!mConsole->isVisible()) {
            for (int i = forwardAction; i < ActionEventCount; i++) {
                if (keyBindings[i] == key) {
                    if (mGame)
                        mGame->handleAction((ActionEvents)i, pressed);
                }
            }
        } else {
            mConsole->handleKeyboard(key, pressed);
        }
    } else {
        mMenu->handleKeyboard(key, pressed);
    }
}

void OpenRaider::handleText(char *text, bool notFinished) {
    if ((mConsole->isVisible()) && (!mMenu->isVisible())) {
        mConsole->handleText(text, notFinished);
    }
}

void OpenRaider::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    if (mMenu->isVisible()) {
        mMenu->handleMouseClick(x, y, button, released);
    }
}

void OpenRaider::handleMouseMotion(int xrel, int yrel) {
    if ((!mConsole->isVisible()) && (!mMenu->isVisible())) {
        if (mGame)
            mGame->handleMouseMotion(xrel, yrel);
    }
}

void OpenRaider::handleMouseScroll(int xrel, int yrel) {
    if ((mConsole->isVisible()) && (!mMenu->isVisible())) {
        mConsole->handleMouseScroll(xrel, yrel);
    }
}

