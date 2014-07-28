/*!
 * \file src/main.cpp
 * \brief Where main() is
 *
 * \author xythobuz
 */

#include <iostream>
#include <cstdlib>
#include <cstring>

#include "global.h"
#include "Camera.h"
#include "Console.h"
#include "FontManager.h"
#include "Game.h"
#include "Menu.h"
#include "OpenRaider.h"
#include "Render.h"
#include "TextureManager.h"
#include "World.h"
#include "commander/commander.h"
#include "utils/strings.h"
#include "utils/time.h"

#ifdef USING_AL
#include "SoundAL.h"
#else
#include "SoundNull.h"
#endif

#ifdef USING_SDL
#include "WindowSDL.h"
#else
#error No Windowing Library selected!
#endif

Camera &getCamera() {
    static Camera gCamera;
    return gCamera;
}

Console &getConsole() {
    static Console gConsole;
    return gConsole;
}

Font &getFont() {
    static FontManager gFont;
    return gFont;
}

Game &getGame() {
    static Game gGame;
    return gGame;
}

Menu &getMenu() {
    static Menu gMenu;
    return gMenu;
}

OpenRaider &getOpenRaider() {
    static OpenRaider gOpenRaider;
    return gOpenRaider;
}

Render &getRender() {
    static Render gRender;
    return gRender;
}

Sound &getSound() {
#ifdef USING_AL
    static SoundAL gSound;
#else
    static SoundNull gSound;
#endif
    return gSound;
}

TextureManager &getTextureManager() {
    static TextureManager gTextureManager;
    return gTextureManager;
}

Window &getWindow() {
#ifdef USING_SDL
    static WindowSDL gWindow;
#endif
    return gWindow;
}

World &getWorld() {
    static World gWorld;
    return gWorld;
}

static void cleanupHandler(void) {
#ifdef DEBUG
    std::cout << std::endl;
    std::cout << "Thanks for testing " << VERSION << std::endl;
    std::cout << "Build date: " << __DATE__ << " @ " << __TIME__ << std::endl;
    std::cout << "Build host: " << BUILD_HOST << std::endl;
    std::cout << "Web site  : http://github.com/xythobuz/OpenRaider" << std::endl;
    std::cout << "Contact   : xythobuz@xythobuz.de" << std::endl;
#endif
}

static bool configFileWasSpecified = false;
static void configFileCallback(command_t *self) {
    getOpenRaider().loadConfig(self->arg);
    configFileWasSpecified = true;
}

int main(int argc, char *argv[]) {
    command_t cmd;
    command_init(&cmd, argv[0], VERSION);
    //command_option(&cmd, "-v", "--verbose", "enable verbose output", functionPointer);
    command_option(&cmd, "-c", "--config <file>", "select config file to use", configFileCallback);
    command_parse(&cmd, argc, argv);

    if (!configFileWasSpecified) {
        if (getOpenRaider().loadConfig(DEFAULT_CONFIG_FILE) != 0) {
            getOpenRaider().loadConfig(DEFAULT_CONFIG_PATH "/" DEFAULT_CONFIG_FILE);
        }
    }

#ifdef DEBUG
    getConsole().print("Initializing %s", VERSION);
#endif

    atexit(cleanupHandler);

    int error = getOpenRaider().initialize();
    if (error != 0) {
        std::cout << "Could not initialize OpenRaider (" << error << ")!" << std::endl;
        return 2;
    }

    command_free(&cmd);

    getConsole().print("Starting %s", VERSION);
    getOpenRaider().run();

    return 0;
}

#if defined(HAVE_EXECINFO_H) && defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS) && (!defined(NDEBUG))

#include <execinfo.h>

void assertImplementation(const char *exp, const char *file, int line) {
    const unsigned int maxSize = 128;
    void *callstack[maxSize];
    int frames = backtrace(callstack, maxSize);
    char **strs = backtrace_symbols(callstack, frames);

    std::cout << std::endl << "assertion failed:" << std::endl;
    std::cout << "\t" << exp << std::endl;
    std::cout << "in " << file << ":" << line << std::endl << std::endl;

    for (int i = 0; i < frames; i++)
        std::cout << strs[i] << std::endl;

    delete [] strs;
    abort();
}

#endif

ActionEvents stringToActionEvent(const char *action) {
    if (strcmp(action, "menu") == 0) {
        return menuAction;
    } else if (strcmp(action, "console") == 0) {
        return consoleAction;
    } else if (strcmp(action, "forward") == 0) {
        return forwardAction;
    } else if (strcmp(action, "backward") == 0) {
        return backwardAction;
    } else if (strcmp(action, "left") == 0) {
        return leftAction;
    } else if (strcmp(action, "right") == 0) {
        return rightAction;
    } else if (strcmp(action, "jump") == 0) {
        return jumpAction;
    } else if (strcmp(action, "crouch") == 0) {
        return crouchAction;
    } else if (strcmp(action, "use") == 0) {
        return useAction;
    } else if (strcmp(action, "holster") == 0) {
        return holsterAction;
    } else if (strcmp(action, "walk") == 0) {
        return walkAction;
    } else {
        getConsole().print("Unknown action: \"%s\"", action);
        return ActionEventCount;
    }
}

KeyboardButton stringToKeyboardButton(const char *key) {
    size_t length = strlen(key);
    if ((key[0] == '\'') && (key[length - 1] == '\'') && (length == 3)) {
        // Literal character like w, a, s, d, 0, 1...
        char c = key[1];
        if (((c >= '0') && (c <= '9'))
            || ((c >= 'a') && (c <= 'z')))
            return (KeyboardButton)c;
    } else if ((key[0] == '\"') && (key[length - 1] == '\"')) {
        // Special characters like tilde, esc, quote...
        char *tmp = stringRemoveQuotes(key);
        if (strcmp(tmp, "quote") == 0) {
            delete [] tmp;
            return quoteKey;
        } else if (strcmp(tmp, "backslash") == 0) {
            delete [] tmp;
            return backslashKey;
        } else if (strcmp(tmp, "backspace") == 0) {
            delete [] tmp;
            return backspaceKey;
        } else if (strcmp(tmp, "capslock") == 0) {
            delete [] tmp;
            return capslockKey;
        } else if (strcmp(tmp, "comma") == 0) {
            delete [] tmp;
            return commaKey;
        } else if (strcmp(tmp, "del") == 0) {
            delete [] tmp;
            return delKey;
        } else if (strcmp(tmp, "up") == 0) {
            delete [] tmp;
            return upKey;
        } else if (strcmp(tmp, "down") == 0) {
            delete [] tmp;
            return downKey;
        } else if (strcmp(tmp, "left") == 0) {
            delete [] tmp;
            return leftKey;
        } else if (strcmp(tmp, "right") == 0) {
            delete [] tmp;
            return rightKey;
        } else if (strcmp(tmp, "end") == 0) {
            delete [] tmp;
            return endKey;
        } else if (strcmp(tmp, "equals") == 0) {
            delete [] tmp;
            return equalsKey;
        } else if (strcmp(tmp, "escape") == 0) {
            delete [] tmp;
            return escapeKey;
        } else if (strcmp(tmp, "f1") == 0) {
            delete [] tmp;
            return f1Key;
        } else if (strcmp(tmp, "f2") == 0) {
            delete [] tmp;
            return f2Key;
        } else if (strcmp(tmp, "f3") == 0) {
            delete [] tmp;
            return f3Key;
        } else if (strcmp(tmp, "f4") == 0) {
            delete [] tmp;
            return f4Key;
        } else if (strcmp(tmp, "f5") == 0) {
            delete [] tmp;
            return f5Key;
        } else if (strcmp(tmp, "f6") == 0) {
            delete [] tmp;
            return f6Key;
        } else if (strcmp(tmp, "f7") == 0) {
            delete [] tmp;
            return f7Key;
        } else if (strcmp(tmp, "f8") == 0) {
            delete [] tmp;
            return f8Key;
        } else if (strcmp(tmp, "f9") == 0) {
            delete [] tmp;
            return f9Key;
        } else if (strcmp(tmp, "f10") == 0) {
            delete [] tmp;
            return f10Key;
        } else if (strcmp(tmp, "f11") == 0) {
            delete [] tmp;
            return f11Key;
        } else if (strcmp(tmp, "f12") == 0) {
            delete [] tmp;
            return f12Key;
        } else if (strcmp(tmp, "backquote") == 0) {
            delete [] tmp;
            return backquoteKey;
        } else if (strcmp(tmp, "home") == 0) {
            delete [] tmp;
            return homeKey;
        } else if (strcmp(tmp, "insert") == 0) {
            delete [] tmp;
            return insertKey;
        } else if (strcmp(tmp, "leftalt") == 0) {
            delete [] tmp;
            return leftaltKey;
        } else if (strcmp(tmp, "leftctrl") == 0) {
            delete [] tmp;
            return leftctrlKey;
        } else if (strcmp(tmp, "leftbracket") == 0) {
            delete [] tmp;
            return leftbracketKey;
        } else if (strcmp(tmp, "leftgui") == 0) {
            delete [] tmp;
            return leftguiKey;
        } else if (strcmp(tmp, "leftshift") == 0) {
            delete [] tmp;
            return leftshiftKey;
        } else if (strcmp(tmp, "minus") == 0) {
            delete [] tmp;
            return minusKey;
        } else if (strcmp(tmp, "numlock") == 0) {
            delete [] tmp;
            return numlockKey;
        } else if (strcmp(tmp, "pagedown") == 0) {
            delete [] tmp;
            return pagedownKey;
        } else if (strcmp(tmp, "pageup") == 0) {
            delete [] tmp;
            return pageupKey;
        } else if (strcmp(tmp, "pause") == 0) {
            delete [] tmp;
            return pauseKey;
        } else if (strcmp(tmp, "dot") == 0) {
            delete [] tmp;
            return dotKey;
        } else if (strcmp(tmp, "rightalt") == 0) {
            delete [] tmp;
            return rightaltKey;
        } else if (strcmp(tmp, "rightctrl") == 0) {
            delete [] tmp;
            return rightctrlKey;
        } else if (strcmp(tmp, "enter") == 0) {
            delete [] tmp;
            return enterKey;
        } else if (strcmp(tmp, "rightgui") == 0) {
            delete [] tmp;
            return rightguiKey;
        } else if (strcmp(tmp, "rightbracket") == 0) {
            delete [] tmp;
            return rightbracketKey;
        } else if (strcmp(tmp, "rightshift") == 0) {
            delete [] tmp;
            return rightshiftKey;
        } else if (strcmp(tmp, "scrolllock") == 0) {
            delete [] tmp;
            return scrolllockKey;
        } else if (strcmp(tmp, "semicolon") == 0) {
            delete [] tmp;
            return semicolonKey;
        } else if (strcmp(tmp, "slash") == 0) {
            delete [] tmp;
            return slashKey;
        } else if (strcmp(tmp, "space") == 0) {
            delete [] tmp;
            return spaceKey;
        } else if (strcmp(tmp, "tab") == 0) {
            delete [] tmp;
            return tabKey;
        } else if (strcmp(tmp, "leftmouse") == 0) {
            delete [] tmp;
            return leftmouseKey;
        } else if (strcmp(tmp, "middlemouse") == 0) {
            delete [] tmp;
            return middlemouseKey;
        } else if (strcmp(tmp, "rightmouse") == 0) {
            delete [] tmp;
            return rightmouseKey;
        } else if (strcmp(tmp, "fourthmouse") == 0) {
            delete [] tmp;
            return fourthmouseKey;
        } else if (strcmp(tmp, "fifthmouse") == 0) {
            delete [] tmp;
            return fifthmouseKey;
        }
        delete [] tmp;
    }

    getConsole().print("Unknown key: %s", key);
    return unknownKey;
}

