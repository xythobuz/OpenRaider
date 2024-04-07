/*!
 * \file src/main.cpp
 * \brief Main Entry Point
 *
 * \author xythobuz
 */

#include <iostream>
#include <memory>

#include "global.h"
#include "Camera.h"
#include "Log.h"
#include "Menu.h"
#include "Render.h"
#include "RunTime.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "UI.h"
#include "World.h"
#include "commands/Command.h"
#include "system/Shader.h"
#include "system/Sound.h"
#include "system/Window.h"
#include "utils/time.h"

#include <glbinding/Binding.h>
#include <ezoptionparser/ezOptionParser.hpp>

// TODO broken
#ifdef ENABLE_GL_DEBUGGING
#include <glbinding/CallbackMask.h>
#include <glbinding/Meta.h>
#endif // ENABLE_GL_DEBUGGING

static std::string configFileToUse;

#ifdef ENABLE_GL_DEBUGGING
static void glErrorCallback(const glbinding::FunctionCall& call) {
    RunTime::incrementCallCount();

    gl::GLenum error = gl::glGetError();
    if (error == gl::GL_NO_ERROR) {
        return;
    }

    auto& log = Log::get(LOG_DEBUG);
    if (glbinding::Meta::stringsByGL()) {
        log << "OpenGL Error: " << glbinding::Meta::getString(error) << Log::endl;
    } else {
        log << "OpenGL Error: "
            << static_cast<std::underlying_type<gl::GLenum>::type>(error)
            << Log::endl;
    }
    log << call.function->name() << "(";

    for (int i = 0; i < call.parameters.size(); i++) {
        log << call.parameters[i]->asString();
        if (i < (call.parameters.size() - 1)) {
            log << ", ";
        }
    }

    log << ")";

    if (call.returnValue) {
        log << " -> " << call.returnValue->asString();
    }

    log << Log::endl;
}

static void glUnresolvedCallback(const glbinding::AbstractFunction& func) {
    Log::get(LOG_ERROR) << "Unresolved OpenGL call: \"" << func.name() << "\"!" << Log::endl;
    orAssert(func.isResolved());
}
#endif // ENABLE_GL_DEBUGGING

int main(int argc, const char* argv[]) {
    ez::ezOptionParser opt;
    opt.overview = "Open Tomb Raider Game Engine";
    opt.syntax = "OpenRaider [OPTIONS]";
    opt.example = "OpenRaider --config ~/.OpenRaider/OpenRaider.ini";
    opt.footer = VERSION;

    opt.add("", 0, 0, 0, "Display usage instructions.", "-h", "-help", "--help", "--usage");
    opt.add("", 0, 1, 0, "Config file to use", "-c", "--conf", "--config");

    opt.parse(argc, argv);

    if (opt.isSet("-h")) {
        std::string usage;
        opt.getUsage(usage);
        std::cout << usage << std::endl;
        return -1;
    }

    std::vector<std::string> badOptions;
    if (!opt.gotRequired(badOptions)) {
        for (int i = 0; i < badOptions.size(); i++) {
            std::cout << "ERROR: Missing required option " << badOptions[i] << "." << std::endl;
        }
        std::string usage;
        opt.getUsage(usage);
        std::cout << usage << std::endl;
        return -2;
    }

    if (!opt.gotExpected(badOptions)) {
        for (int i = 0; i < badOptions.size(); i++) {
            std::cout << "ERROR: Got unexpected number of arguments for option " << badOptions[i] << "." <<
                      std::endl;
        }
        std::string usage;
        opt.getUsage(usage);
        std::cout << usage << std::endl;
        return -3;
    }

    if (opt.isSet("-c")) {
        std::cout << "Test!" << std::endl;
        opt.get("-c")->getString(configFileToUse);
    }

    glbinding::Binding::initialize(nullptr);
    Log::initialize();
    RunTime::initialize(); // RunTime is required by other constructors
    Command::fillCommandList();

#ifdef ENABLE_GL_DEBUGGING
    // Register global OpenGL after-callback for all GL functions except glGetError
    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After
                                     | glbinding::CallbackMask::ParametersAndReturnValue,
    { "glGetError" });
    glbinding::setAfterCallback(glErrorCallback);
    glbinding::setUnresolvedCallback(glUnresolvedCallback);
#endif // ENABLE_GL_DEBUGGING

    Log::get(LOG_INFO) << "Initializing " << VERSION << Log::endl;

    // Initialize Windowing
    int error = Window::initialize();
    if (error != 0) {
        std::cout << "Could not initialize Window (" << error << ")!" << std::endl;
        return -4;
    }

    error = Shader::initialize();
    if (error != 0) {
        std::cout << "Could not initialize OpenGL (" << error << ")!" << std::endl;
        return -5;
    }

    // Initialize Texture Manager
    error = TextureManager::initialize();
    if (error != 0) {
        std::cout << "Could not initialize TextureManager (" << error << ")!" << std::endl;
        return -6;
    }

    if (configFileToUse == "") {
        if (Command::executeFile(DEFAULT_CONFIG_FILE) != 0) {
            if (Command::executeFile(std::string(DEFAULT_CONFIG_PATH) + "/" + DEFAULT_CONFIG_FILE) != 0) {
                std::string p = INSTALL_PREFIX;
                if (p != "/")
                    p += "/";
                p += "share/OpenRaider/";
                if (Command::executeFile(p + DEFAULT_CONFIG_FILE) != 0) {
                    std::cout << "Could not find any config files. Trying to continue..." << std::endl;
                }
            }
        }
    } else {
        if (Command::executeFile(configFileToUse) != 0) {
            std::cout << "Could not find specified config file!" << std::endl;
            return -7;
        }
    }

    error = TextureManager::initializeSplash();
    if (error != 0) {
        std::cout << "Coult not load Splash Texture (" << error << ")!" << std::endl;
        return -8;
    }

    // Initialize Sound
    error = Sound::initialize();
    if (error != 0) {
        std::cout << "Could not initialize Sound (" << error << ")!" << std::endl;
        return -9;
    }

    // Initialize Debug UI
    error = UI::initialize();
    if (error != 0) {
        std::cout << "Could not initialize Debug UI (" << error << ")!" << std::endl;
        return -10;
    }

    // Initialize Menu
    error = Menu::initialize();
    if (error != 0) {
        std::cout << "Could not initialize Menu (" << error << ")!" << std::endl;
        return -11;
    }

    Log::get(LOG_INFO) << "Starting " << VERSION << Log::endl;
    Camera::setSize(Window::getSize());
    Menu::setVisible(true);
    systemTimerReset();
    RunTime::setRunning(true);
    Render::setMode(RenderMode::LoadScreen);

    while (RunTime::isRunning()) {
        Window::eventHandling();
        renderFrame();
    }

    World::destroy();
    Menu::shutdown();
    UI::shutdown();
    Sound::shutdown();
    Shader::shutdown();
    Window::shutdown();

#ifdef DEBUG
    std::cout << std::endl;
    std::cout << "Thanks for testing " << VERSION << std::endl;
#if defined(__DATE__) && defined(__TIME__)
    std::cout << "Build date: " << __DATE__ << " @ " << __TIME__ << std::endl;
#endif
    std::cout << "Build host: " << BUILD_HOST << std::endl;
    std::cout << "Web site  : http://github.com/xythobuz/OpenRaider" << std::endl;
    std::cout << "Contact   : xythobuz@xythobuz.de" << std::endl;
#endif

    return 0;
}

void renderFrame() {
    Render::display();
    UI::display();
    Window::swapBuffers();
    RunTime::updateFPS();
}

#if defined(HAVE_EXECINFO_H) && defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS)
#ifndef NDEBUG

#include <exception>
#include <execinfo.h>

[[noreturn]] static void terminateHandler();
static std::terminate_handler oldTerminateHandler = std::set_terminate(terminateHandler);

[[noreturn]] static void terminateHandler() {
    const unsigned int maxSize = 128;
    void* callstack[maxSize];
    int frames = backtrace(callstack, maxSize);
    char** strs = backtrace_symbols(callstack, frames);

    std::cout << std::endl;
    for (int i = frames; i > 0; i++)
        std::cout << strs[i - 1] << std::endl;

    delete [] strs;

    oldTerminateHandler();
    abort();
}

#endif // NDEBUG
#endif // HAVE_EXECINFO_H && HAVE_BACKTRACE && HAVE_BACKTRACE_SYMBOLS

