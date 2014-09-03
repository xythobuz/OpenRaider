/*!
 * \file src/main.cpp
 * \brief Where main() is
 *
 * \author xythobuz
 */

#include <iostream>
#include <memory>

#include "global.h"
#include "Console.h"
#include "Exception.h"
#include "commander/commander.h"
#include "commands/Command.h"
#include "utils/time.h"

#ifndef UNIT_TEST

#include "Camera.h"
#include "Debug.h"
#include "FontManager.h"
#include "Game.h"
#include "MenuFolder.h"
#include "Render.h"
#include "RunTime.h"
#include "TextureManager.h"
#include "UI.h"
#include "Window.h"
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

static std::string configFileToUse;

static std::shared_ptr<Camera> gCamera;
static std::shared_ptr<Console> gConsole;
static std::shared_ptr<Debug> gDebug;
static std::shared_ptr<FontManager> gFont;
static std::shared_ptr<Game> gGame;
static std::shared_ptr<MenuFolder> gMenu;
static std::shared_ptr<Render> gRender;
static std::shared_ptr<RunTime> gRunTime;
static std::shared_ptr<Sound> gSound;
static std::shared_ptr<TextureManager> gTextureManager;
static std::shared_ptr<Window> gWindow;
static std::shared_ptr<World> gWorld;

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

Render &getRender() {
    return *gRender;
}

RunTime &getRunTime() {
    return *gRunTime;
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
    command_t cmd;
    command_init(&cmd, argv[0], VERSION);
    command_option(&cmd, "-c", "--config <file>", "select config file to use",
            [](command_t *self) {
        configFileToUse = self->arg;
    });
    command_parse(&cmd, argc, argv);
    command_free(&cmd);

    gCamera.reset(new Camera());
    gConsole.reset(new Console());
    gDebug.reset(new Debug());
    gFont.reset(new FontManager());
    gGame.reset(new Game());
    gMenu.reset(new MenuFolder());
    gRender.reset(new Render());
    gRunTime.reset(new RunTime());
    gTextureManager.reset(new TextureManager());
    gWorld.reset(new World());

#ifdef USING_AL
    gSound.reset(new SoundAL());
#else
    gSound.reset(new SoundNull());
#endif

#ifdef USING_SDL
    gWindow.reset(new WindowSDL());
#endif

    atexit(cleanupHandler);
    Command::fillCommandList();

    if (configFileToUse == "") {
        if (Command::executeFile(DEFAULT_CONFIG_FILE) != 0) {
            Command::executeFile(DEFAULT_CONFIG_PATH "/" DEFAULT_CONFIG_FILE);
        }
    } else {
        Command::executeFile(configFileToUse);
    }

    getConsole() << "Initializing " << VERSION << Console::endl;

    // Initialize Windowing
    int error = getWindow().initialize();
    if (error != 0) {
        std::cout << "Could not initialize Window (" << error << ")!" << std::endl;
        return -1;
    }

    // Initialize OpenGL
    error = getWindow().initializeGL();
    if (error != 0) {
        std::cout << "Could not initialize OpenGL (" << error << ")!" << std::endl;
        return -2;
    }

    // Initialize Font
    error = getFont().initialize();
    if (error != 0) {
        std::cout << "Could not initialize Font (" << error << ")!" << std::endl;
        return -3;
    }

    // Initialize Sound
    error = getSound().initialize();
    if (error != 0) {
        std::cout << "Could not initialize Sound (" << error << ")!" << std::endl;
        return -4;
    }

    // Initialize Texture Manager
    error = getTextureManager().initialize();
    if (error != 0) {
        std::cout << "Could not initialize TextureManager (" << error << ")!" << std::endl;
        return -5;
    }

    // Initialize UIs
    error = UI::passInitialize();
    if (error != 0) {
        std::cout << "Could not initialize UIs (" << error << ")!" << std::endl;
        return -6;
    }

    getConsole() << "Starting " << VERSION << Console::endl;
    getMenu().moveToTop();
    systemTimerReset();
    getRunTime().start();

    while (getRunTime().isRunning()) {
        renderFrame();
    }

    return 0;
}

void renderFrame() {
    // Get keyboard and mouse input
    getWindow().eventHandling();

    ImGui::SetNewWindowDefaultPos(ImVec2(50, 50));
    bool show_test_window = false;
    ImGui::ShowTestWindow(&show_test_window);

    // Render everything
    UI::passDisplay();

    // Put new frame on screen
    getWindow().swapBuffersGL();
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

