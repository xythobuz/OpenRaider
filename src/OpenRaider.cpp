/*!
 * \file src/OpenRaider.cpp
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#include <cstdio>
#include <cstring>
#include <assert.h>

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

    for (int i = 0; i < ActionEventCount; i++)
        keyBindings[i] = unknownKey;
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
}

int OpenRaider::initialize() {
    // Initialize Windowing
    int error = getWindow().initialize();
    if (error != 0) {
        printf("Could not initialize Window (%d)!\n", error);
        return -1;
    }

    // Initialize OpenGL
    error = getWindow().initializeGL();
    if (error != 0) {
        printf("Could not initialize OpenGL (%d)!\n", error);
        return -2;
    }

    error = getWindow().initializeFont();
    if (error != 0) {
        printf("Could not initialize SDL-TTF (%d)!\n", error);
        return -3;
    }

    error = getSound().initialize();
    if (error != 0) {
        printf("Could not initialize Sound (%d)!\n", error);
        return -4;
    }

    // Initialize game engine
    error = getGame().initialize();
    if (error != 0) {
        printf("Could not initialize Game Engine (%d)!\n", error);
        return -5;
    }

#ifdef DEBUG
    mFPS = true;
#endif

    getMenu().setVisible(true);
    systemTimerReset();

    return 0;
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

    char *strtokState;
    char *token = strtok_r(cmd, " \t", &strtokState);
    if (token != NULL) {
        // token is the command to execute
        // get arguments
        std::vector<char *> args;
        char *next;
        while ((next = strtok_r(NULL, " \t", &strtokState)) != NULL) {
            args.push_back(next);
        }

        // Execute
        returnValue = this->command(token, &args);
    }

    free(cmd);
    return returnValue;
}

char *OpenRaider::expandDirectoryNames(const char *s) {
    assert(s != NULL);
    assert(s[0] != '\0');

    if (mBaseDir != NULL) {
        const char *base = "$(basedir)";
        if (strstr(s, base) != NULL) {
            return stringReplace(s, base, mBaseDir);
        }
    }

    if (mPakDir != NULL) {
        const char *pak = "$(pakdir)";
        if (strstr(s, pak) != NULL) {
            return stringReplace(s, pak, mPakDir);
        }
    }

    if (mAudioDir != NULL) {
        const char *audio = "$(audiodir)";
        if (strstr(s, audio) != NULL) {
            return stringReplace(s, audio, mAudioDir);
        }
    }

    if (mDataDir != NULL) {
        const char *data = "$(datadir)";
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
        if (sscanf(value, "\"%5dx%5d\"", &w, &h) != 2) {
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
        if (sscanf(value, "%5f", &vol) != 1) {
            getConsole().print("set-volume-Error: Invalid value (%s)", value);
            return -5;
        }
        getSound().setVolume(vol);
    } else if (strcmp(var, "mouse_x") == 0) {
        float sense = 1.0f;
        if (sscanf(value, "%5f", &sense) != 1) {
            getConsole().print("set-mouse_x-Error: Invalid value (%s)", value);
            return -6;
        }
        getCamera().setSensitivityX(OR_DEG_TO_RAD(sense));
    } else if (strcmp(var, "mouse_y") == 0) {
        float sense = 1.0f;
        if (sscanf(value, "%5f", &sense) != 1) {
            getConsole().print("set-mouse_y-Error: Invalid value (%s)", value);
            return -7;
        }
        getCamera().setSensitivityY(OR_DEG_TO_RAD(sense));
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
            keyBindings[action] = quoteKey;
        } else if (strcmp(tmp, "backslash") == 0) {
            keyBindings[action] = backslashKey;
        } else if (strcmp(tmp, "backspace") == 0) {
            keyBindings[action] = backspaceKey;
        } else if (strcmp(tmp, "capslock") == 0) {
            keyBindings[action] = capslockKey;
        } else if (strcmp(tmp, "comma") == 0) {
            keyBindings[action] = commaKey;
        } else if (strcmp(tmp, "del") == 0) {
            keyBindings[action] = delKey;
        } else if (strcmp(tmp, "up") == 0) {
            keyBindings[action] = upKey;
        } else if (strcmp(tmp, "down") == 0) {
            keyBindings[action] = downKey;
        } else if (strcmp(tmp, "left") == 0) {
            keyBindings[action] = leftKey;
        } else if (strcmp(tmp, "right") == 0) {
            keyBindings[action] = rightKey;
        } else if (strcmp(tmp, "end") == 0) {
            keyBindings[action] = endKey;
        } else if (strcmp(tmp, "equals") == 0) {
            keyBindings[action] = equalsKey;
        } else if (strcmp(tmp, "escape") == 0) {
            keyBindings[action] = escapeKey;
        } else if (strcmp(tmp, "f1") == 0) {
            keyBindings[action] = f1Key;
        } else if (strcmp(tmp, "f2") == 0) {
            keyBindings[action] = f2Key;
        } else if (strcmp(tmp, "f3") == 0) {
            keyBindings[action] = f3Key;
        } else if (strcmp(tmp, "f4") == 0) {
            keyBindings[action] = f4Key;
        } else if (strcmp(tmp, "f5") == 0) {
            keyBindings[action] = f5Key;
        } else if (strcmp(tmp, "f6") == 0) {
            keyBindings[action] = f6Key;
        } else if (strcmp(tmp, "f7") == 0) {
            keyBindings[action] = f7Key;
        } else if (strcmp(tmp, "f8") == 0) {
            keyBindings[action] = f8Key;
        } else if (strcmp(tmp, "f9") == 0) {
            keyBindings[action] = f9Key;
        } else if (strcmp(tmp, "f10") == 0) {
            keyBindings[action] = f10Key;
        } else if (strcmp(tmp, "f11") == 0) {
            keyBindings[action] = f11Key;
        } else if (strcmp(tmp, "f12") == 0) {
            keyBindings[action] = f12Key;
        } else if (strcmp(tmp, "backquote") == 0) {
            keyBindings[action] = backquoteKey;
        } else if (strcmp(tmp, "home") == 0) {
            keyBindings[action] = homeKey;
        } else if (strcmp(tmp, "insert") == 0) {
            keyBindings[action] = insertKey;
        } else if (strcmp(tmp, "leftalt") == 0) {
            keyBindings[action] = leftaltKey;
        } else if (strcmp(tmp, "leftctrl") == 0) {
            keyBindings[action] = leftctrlKey;
        } else if (strcmp(tmp, "leftbracket") == 0) {
            keyBindings[action] = leftbracketKey;
        } else if (strcmp(tmp, "leftgui") == 0) {
            keyBindings[action] = leftguiKey;
        } else if (strcmp(tmp, "leftshift") == 0) {
            keyBindings[action] = leftshiftKey;
        } else if (strcmp(tmp, "minus") == 0) {
            keyBindings[action] = minusKey;
        } else if (strcmp(tmp, "numlock") == 0) {
            keyBindings[action] = numlockKey;
        } else if (strcmp(tmp, "pagedown") == 0) {
            keyBindings[action] = pagedownKey;
        } else if (strcmp(tmp, "pageup") == 0) {
            keyBindings[action] = pageupKey;
        } else if (strcmp(tmp, "pause") == 0) {
            keyBindings[action] = pauseKey;
        } else if (strcmp(tmp, "dot") == 0) {
            keyBindings[action] = dotKey;
        } else if (strcmp(tmp, "rightalt") == 0) {
            keyBindings[action] = rightaltKey;
        } else if (strcmp(tmp, "rightctrl") == 0) {
            keyBindings[action] = rightctrlKey;
        } else if (strcmp(tmp, "enter") == 0) {
            keyBindings[action] = enterKey;
        } else if (strcmp(tmp, "rightgui") == 0) {
            keyBindings[action] = rightguiKey;
        } else if (strcmp(tmp, "rightbracket") == 0) {
            keyBindings[action] = rightbracketKey;
        } else if (strcmp(tmp, "rightshift") == 0) {
            keyBindings[action] = rightshiftKey;
        } else if (strcmp(tmp, "scrolllock") == 0) {
            keyBindings[action] = scrolllockKey;
        } else if (strcmp(tmp, "semicolon") == 0) {
            keyBindings[action] = semicolonKey;
        } else if (strcmp(tmp, "slash") == 0) {
            keyBindings[action] = slashKey;
        } else if (strcmp(tmp, "space") == 0) {
            keyBindings[action] = spaceKey;
        } else if (strcmp(tmp, "tab") == 0) {
            keyBindings[action] = tabKey;
        } else if (strcmp(tmp, "leftmouse") == 0) {
            keyBindings[action] = leftmouseKey;
        } else if (strcmp(tmp, "middlemouse") == 0) {
            keyBindings[action] = middlemouseKey;
        } else if (strcmp(tmp, "rightmouse") == 0) {
            keyBindings[action] = rightmouseKey;
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

    // Draw game scene
    getRender().display();

    // Draw 2D overlays (console and menu)
    getWindow().glEnter2D();

    getConsole().display();
    getMenu().display();

    // Draw FPS counter
    if (mFPS)
        getWindow().drawText(10, getWindow().mHeight - 20, 0.5f, OR_BLUE, "%dFPS", fps);

#ifdef DEBUG
    // Draw debug infos
    if (getGame().isLoaded()) {
        for (int i = 0; i < 3; i++) {
            getWindow().drawText(10, getWindow().mHeight - ((4 - i) * 20), 0.5f, OR_BLUE, "%.2f (%.2f)",
                getGame().mLara->pos[i] / 256.0f, getGame().mLara->angles[i]);
        }
    }
#endif

    getWindow().glExit2D();

    // Put new frame on screen
    getWindow().swapBuffersGL();

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
    assert(key < unknownKey);
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
    assert(button < unknownKey);
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

