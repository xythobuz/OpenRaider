/*!
 * \file src/main.cpp
 * \brief Where main() is
 *
 * \author xythobuz
 */

#include <iostream>
#include <memory>
#include <sstream>

#include "global.h"
#include "Exception.h"
#include "commander/commander.h"
#include "commands/Command.h"
#include "utils/time.h"

#ifndef UNIT_TEST

#include "Camera.h"
#include "Font.h"
#include "Game.h"
#include "Log.h"
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
#elif defined(USING_GLUT)
#include "WindowGLUT.h"
#else
#error No Windowing Library selected!
#endif

static std::string configFileToUse;

static std::shared_ptr<Camera> gCamera;
static std::shared_ptr<Game> gGame;
static std::shared_ptr<Log> gLog;
static std::shared_ptr<MenuFolder> gMenu;
static std::shared_ptr<Render> gRender;
static std::shared_ptr<RunTime> gRunTime;
static std::shared_ptr<Sound> gSound;
static std::shared_ptr<TextureManager> gTextureManager;
static std::shared_ptr<Window> gWindow;
static std::shared_ptr<World> gWorld;

Camera &getCamera() {
    return *gCamera;
}

Game &getGame() {
    return *gGame;
}

Log &getLog() {
    return *gLog;
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

    // RunTime is required by other constructors
    gRunTime.reset(new RunTime());

    gCamera.reset(new Camera());
    gGame.reset(new Game());
    gLog.reset(new Log());
    gMenu.reset(new MenuFolder());
    gRender.reset(new Render());
    gTextureManager.reset(new TextureManager());
    gWorld.reset(new World());

#ifdef USING_AL
    gSound.reset(new SoundAL());
#else
    gSound.reset(new SoundNull());
#endif

#ifdef USING_SDL
    gWindow.reset(new WindowSDL());
#elif defined(USING_GLUT)
    gWindow.reset(new WindowGLUT());
#endif

    Command::fillCommandList();

    getLog() << "Initializing " << VERSION << Log::endl;

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

    // Font initialization requires GL context, but is called from config file
    // So we need to initialize some things before executing the config
    if (configFileToUse == "") {
        if (Command::executeFile(DEFAULT_CONFIG_FILE) != 0) {
            if (Command::executeFile(std::string(DEFAULT_CONFIG_PATH) + "/" + DEFAULT_CONFIG_FILE) != 0) {
                std::string p = INSTALL_PREFIX;
                if (p == "/")
                    p += "etc/";
                else
                    p += "/etc/";
                Command::executeFile(p + DEFAULT_CONFIG_FILE);
            }
        }
    } else {
        Command::executeFile(configFileToUse);
    }

    // Initialize Font
#ifdef USING_SDL_FONT
    error = Font::initialize(getRunTime().getDataDir() + "/test.ttf");
#else
    error = Font::initialize(getRunTime().getDataDir() + "/font.pc");
#endif
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

    // Initialize Menu
    error = getMenu().initialize();
    if (error != 0) {
        std::cout << "Could not initialize Menu (" << error << ")!" << std::endl;
        return -6;
    }

    // Initialize Debug UI
    error = UI::initialize();
    if (error != 0) {
        std::cout << "Could not initialize Debug UI (" << error << ")!" << std::endl;
        return -7;
    }

    // Initialize Game Engine
    error = getGame().initialize();
    if (error != 0) {
        std::cout << "Could not initialize Game (" << error << ")!" << std::endl;
        return -8;
    }

    getLog() << "Starting " << VERSION << Log::endl;
    getMenu().setVisible(true);
    systemTimerReset();
    getRunTime().setRunning(true);

    while (getRunTime().isRunning()) {
        getWindow().eventHandling();
        renderFrame();
    }

    UI::shutdown();
    Font::shutdown();

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

    const static unsigned long fpsUpdate = 250;
    static unsigned long frameCount = 0;
    static unsigned long lastTime = 0;
    static unsigned long frameTimeSum = 0;
    static unsigned long fps = 0;

    if (getRunTime().getFPS()) {
        std::ostringstream s;
        s << fps << "FPS";
        getWindow().glEnter2D();
        Font::drawText(10, getWindow().getHeight() - 25, 0.6f, BLUE, s.str());
        getWindow().glExit2D();
    }

    getWindow().swapBuffersGL();

    frameCount++;
    frameTimeSum += (systemTimerGet() - lastTime);
    if (frameTimeSum >= fpsUpdate) {
        fps = frameCount * (1000 / frameTimeSum);
        frameCount = frameTimeSum = 0;
    }
    lastTime = systemTimerGet();
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

