/*!
 * \file src/main.cpp
 * \brief Where main() is
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
#include "Console.h"
#include "Exception.h"
#include "OpenRaider.h"
#include "commander/commander.h"
#include "commands/Command.h"

#include "Camera.h"
#include "Debug.h"
#include "FontManager.h"
#include "Game.h"
#include "MenuFolder.h"
#include "Render.h"
#include "TextureManager.h"
#include "World.h"

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

#ifndef UNIT_TEST

namespace {
    Camera* gCamera;
    Console* gConsole;
    Debug* gDebug;
    FontManager* gFont;
    Game* gGame;
    MenuFolder* gMenu;
    OpenRaider* gOpenRaider;
    Render* gRender;
    Sound* gSound;
    TextureManager* gTextureManager;
    Window* gWindow;
    World* gWorld;

    void createGlobals() {
        gOpenRaider = new OpenRaider();
        gCamera = new Camera();
        gConsole = new Console();
        gDebug = new Debug();
        gFont = new FontManager();
        gGame = new Game();
        gMenu = new MenuFolder();
        gRender = new Render();
        gTextureManager = new TextureManager();
        gWorld = new World();

#ifdef USING_AL
        gSound = new SoundAL();
#else
        gSound = new SoundNull();
#endif

#ifdef USING_SDL
        gWindow = new WindowSDL();
#endif
    }

    void deleteGlobals() {
        delete gCamera;
        delete gConsole;
        delete gFont;
        delete gGame;
        delete gMenu;
        delete gOpenRaider;
        delete gRender;
        delete gSound;
        delete gTextureManager;
        delete gWindow;
        delete gWorld;
    }

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

        deleteGlobals();
    }
}

Camera &getCamera() {
    return *gCamera;
}

Console &getConsole() {
    return *gConsole;
}

Debug& getDebug() {
    return *gDebug;
}

Font &getFont() {
    return *gFont;
}

Game &getGame() {
    return *gGame;
}

Menu &getMenu() {
    return *gMenu;
}

OpenRaider &getOpenRaider() {
    return *gOpenRaider;
}

Render &getRender() {
    return *gRender;
}

Sound &getSound() {
    return *gSound;
}

TextureManager &getTextureManager() {
    return *gTextureManager;
}

Window &getWindow() {
    return *gWindow;
}

World &getWorld() {
    return *gWorld;
}

int main(int argc, char* argv[]) {
    createGlobals();
    atexit(cleanupHandler);
    Command::fillCommandList();

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

    int error = getOpenRaider().initialize();
    if (error != 0) {
        std::cout << "Could not initialize OpenRaider (" << error << ")!" << std::endl;
        return -1;
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

