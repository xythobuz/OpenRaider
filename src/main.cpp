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
#include "Exception.h"
#include "FontManager.h"
#include "Game.h"
#include "MenuFolder.h"
#include "OpenRaider.h"
#include "Render.h"
#include "TextureManager.h"
#include "World.h"
#include "commander/commander.h"
#include "utils/strings.h"
#include "utils/time.h"

#ifndef UNIT_TEST

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
    static MenuFolder gMenu;
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

namespace {
    bool configFileWasSpecified = false;

    void configFileCallback(command_t *self) {
        getOpenRaider().loadConfig(self->arg);
        configFileWasSpecified = true;
    }

    void cleanupHandler(void) {
#ifdef DEBUG
        std::cout << std::endl;
        std::cout << "Thanks for testing " << VERSION << std::endl;
        std::cout << "Build date: " << __DATE__ << " @ " << __TIME__ << std::endl;
        std::cout << "Build host: " << BUILD_HOST << std::endl;
        std::cout << "Web site  : http://github.com/xythobuz/OpenRaider" << std::endl;
        std::cout << "Contact   : xythobuz@xythobuz.de" << std::endl;
#endif
    }
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
    getConsole() << "Initializing " << VERSION << Console::endl;
#endif

    atexit(cleanupHandler);

    int error = getOpenRaider().initialize();
    if (error != 0) {
        std::cout << "Could not initialize OpenRaider (" << error << ")!" << std::endl;
        return 2;
    }

    command_free(&cmd);

    getConsole() << "Starting " << VERSION << Console::endl;
    getOpenRaider().run();

    return 0;
}

#endif // UNIT_TEST

#if defined(HAVE_EXECINFO_H) && defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS)
#ifndef NDEBUG

#include <exception>
#include <execinfo.h>

namespace {
    extern std::terminate_handler oldTerminateHandler;

    [[noreturn]] void terminateHandler() {
        const unsigned int maxSize = 128;
        void *callstack[maxSize];
        int frames = backtrace(callstack, maxSize);
        char **strs = backtrace_symbols(callstack, frames);

        std::cout << std::endl;
        for (int i = 0; i < frames; i++)
            std::cout << strs[i] << std::endl;

        delete [] strs;

        std::cout << std::endl << "Last custom Exception:" << std::endl;
        std::cout << "    " << Exception::getLastException() << std::endl << std::endl;

        oldTerminateHandler();
        abort();
    }

    std::terminate_handler oldTerminateHandler = std::set_terminate(terminateHandler);
}

#endif // NDEBUG
#endif // HAVE_EXECINFO_H && HAVE_BACKTRACE && HAVE_BACKTRACE_SYMBOLS

