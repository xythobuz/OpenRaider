/*!
 * \file src/main.cpp
 * \brief Where main() is
 *
 * \author xythobuz
 */

#include <iostream>
#include <memory>

#include "global.h"
#include "Exception.h"
#include "commander/commander.h"
#include "commands/Command.h"
#include "utils/time.h"

#ifndef UNIT_TEST

#include "Game.h"
#include "Log.h"
#include "MenuFolder.h"
#include "RunTime.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "UI.h"
#include "World.h"
#include "system/Font.h"
#include "system/Sound.h"
#include "system/Window.h"

static std::string configFileToUse;

static std::shared_ptr<Game> gGame;
static std::shared_ptr<Log> gLog;
static std::shared_ptr<MenuFolder> gMenu;
static std::shared_ptr<RunTime> gRunTime;
static std::shared_ptr<TextureManager> gTextureManager;
static std::shared_ptr<World> gWorld;

Game& getGame() {
    return *gGame;
}

Log& getLog() {
    return *gLog;
}

Menu& getMenu() {
    return *gMenu;
}

RunTime& getRunTime() {
    return *gRunTime;
}

TextureManager& getTextureManager() {
    return *gTextureManager;
}

World& getWorld() {
    return *gWorld;
}

int main(int argc, char* argv[]) {
    command_t cmd;
    command_init(&cmd, argv[0], VERSION);
    command_option(&cmd, "-c", "--config <file>", "select config file to use",
    [](command_t* self) {
        configFileToUse = self->arg;
    });
    command_parse(&cmd, argc, argv);
    command_free(&cmd);

    // RunTime is required by other constructors
    gRunTime.reset(new RunTime());

    gGame.reset(new Game());
    gLog.reset(new Log());
    gMenu.reset(new MenuFolder());
    gTextureManager.reset(new TextureManager());
    gWorld.reset(new World());

    Command::fillCommandList();

    getLog() << "Initializing " << VERSION << Log::endl;

    // Initialize Windowing
    int error = Window::initialize();
    if (error != 0) {
        std::cout << "Could not initialize Window/GL (" << error << ")!" << std::endl;
        return -1;
    }

    // Initialize Texture Manager
    error = getTextureManager().initialize();
    if (error != 0) {
        std::cout << "Could not initialize TextureManager (" << error << ")!" << std::endl;
        return -2;
    }

    if (configFileToUse == "") {
        if (Command::executeFile(DEFAULT_CONFIG_FILE) != 0) {
            if (Command::executeFile(std::string(DEFAULT_CONFIG_PATH) + "/" + DEFAULT_CONFIG_FILE) != 0) {
                std::string p = INSTALL_PREFIX;
                if (p != "/")
                    p += "/";
                p += "share/OpenRaider/";
                Command::executeFile(p + DEFAULT_CONFIG_FILE);
            }
        }
    } else {
        Command::executeFile(configFileToUse);
    }

    error = getTextureManager().initializeSplash();
    if (error != 0) {
        std::cout << "Coult not load Splash Texture (" << error << ")!" << std::endl;
        return -3;
    }

    // Initialize Sound
    error = Sound::initialize();
    if (error != 0) {
        std::cout << "Could not initialize Sound (" << error << ")!" << std::endl;
        return -4;
    }

    // Initialize Menu
    error = getMenu().initialize();
    if (error != 0) {
        std::cout << "Could not initialize Menu (" << error << ")!" << std::endl;
        return -5;
    }

    // Initialize Debug UI
    error = UI::initialize();
    if (error != 0) {
        std::cout << "Could not initialize Debug UI (" << error << ")!" << std::endl;
        return -6;
    }

    // Initialize Game Engine
    error = getGame().initialize();
    if (error != 0) {
        std::cout << "Could not initialize Game (" << error << ")!" << std::endl;
        return -7;
    }

    getLog() << "Starting " << VERSION << Log::endl;
    getMenu().setVisible(true);
    systemTimerReset();
    getRunTime().setRunning(true);

    while (getRunTime().isRunning()) {
        Window::eventHandling();
        renderFrame();
    }

    UI::shutdown();
    Font::shutdown();
    Sound::shutdown();
    Window::shutdown();

#ifdef DEBUG
    std::cout << std::endl;
    std::cout << "Thanks for testing " << VERSION << std::endl;
    std::cout << "Build date: " << __DATE__ << " @ " << __TIME__ << std::endl;
    std::cout << "Build host: " << BUILD_HOST << std::endl;
    std::cout << "Web site  : http://github.com/xythobuz/OpenRaider" << std::endl;
    std::cout << "Contact   : xythobuz@xythobuz.de" << std::endl;
#endif

    return 0;
}

void renderFrame() {
    getGame().display();
    getMenu().display();
    UI::display();
    Window::swapBuffers();
    getRunTime().updateFPS();
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
        void* callstack[maxSize];
        int frames = backtrace(callstack, maxSize);
        char** strs = backtrace_symbols(callstack, frames);

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

