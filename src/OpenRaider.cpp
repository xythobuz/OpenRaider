/*!
 * \file src/OpenRaider.cpp
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "global.h"
#include "commands/CommandAnimate.h"
#include "commands/CommandBind.h"
#include "commands/CommandEngine.h"
#include "commands/CommandGame.h"
#include "commands/CommandMove.h"
#include "commands/CommandRender.h"
#include "commands/CommandSet.h"
#include "commands/CommandSound.h"
#include "Console.h"
#include "Font.h"
#include "Game.h"
#include "Menu.h"
#include "Render.h"
#include "Sound.h"
#include "TextureManager.h"
#include "TombRaider.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "Window.h"
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

    commands.push_back(std::shared_ptr<Command>(new CommandLoad()));
    commands.push_back(std::shared_ptr<Command>(new CommandBind()));
    commands.push_back(std::shared_ptr<Command>(new CommandSet()));
    commands.push_back(std::shared_ptr<Command>(new CommandScreenshot()));
    commands.push_back(std::shared_ptr<Command>(new CommandAnimate()));
    commands.push_back(std::shared_ptr<Command>(new CommandMove()));
    commands.push_back(std::shared_ptr<Command>(new CommandMode()));
    commands.push_back(std::shared_ptr<Command>(new CommandFog()));
    commands.push_back(std::shared_ptr<Command>(new CommandLight()));
    commands.push_back(std::shared_ptr<Command>(new CommandSound()));
    commands.push_back(std::shared_ptr<Command>(new CommandPos()));
    commands.push_back(std::shared_ptr<Command>(new CommandViewmodel()));
    commands.push_back(std::shared_ptr<Command>(new CommandQuit()));
}

OpenRaider::~OpenRaider() {
    delete mBaseDir;
    mBaseDir = NULL;

    delete mPakDir;
    mPakDir = NULL;

    delete mAudioDir;
    mAudioDir = NULL;

    delete mDataDir;
    mDataDir = NULL;
}

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

    // Print help
    if (cmd.compare("help") == 0) {
        std::string arg;
        command >> arg;
        if (arg.length() == 0) {
            // List all available commands
            getConsole() << "Available commands:" << Console::endl;
            getConsole() << std::right << std::setw(11);
            getConsole() << "help" << " - print command help" << Console::endl;
            for (auto &x : commands) {
                if (x) {
                    getConsole() << std::right << std::setw(11);
                    getConsole() << x->name() << " - " << x->brief() << Console::endl;
                }
            }
            getConsole() << "Use help COMMAND to get additional info" << Console::endl;
            getConsole() << "Pass BOOLs as true or false" << Console::endl;
            return 0;
        } else {
            // Show help for a specific command
            for (auto &x : commands) {
                if (x) {
                    if (x->name().compare(arg) == 0) {
                        x->printHelp();
                        return 0;
                    }
                }
            }
            getConsole() << "Unknown command: \"" << arg << "\"" << Console::endl;
            return -1;
        }
    }

    // Execute command
    for (auto &x : commands) {
        if (x) {
            if (x->name().compare(cmd) == 0) {
                return x->execute(command);
            }
        }
    }

    getConsole() << "Unknown command: \"" << cmd << "\"" << Console::endl;
    return -1;
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

    // Initialize Font
    error = getFont().initialize();
    if (error != 0) {
        printf("Could not initialize Font (%d)!\n", error);
        return -3;
    }

    // Initialize Sound
    error = getSound().initialize();
    if (error != 0) {
        printf("Could not initialize Sound (%d)!\n", error);
        return -4;
    }

    // Initialize Texture Manager
    error = getTextureManager().initialize();
    if (error != 0) {
        printf("Could not initialize Textures (%d)!\n", error);
        return -5;
    }

    // Initialize game engine
    error = getGame().initialize();
    if (error != 0) {
        printf("Could not initialize Game (%d)!\n", error);
        return -6;
    }

    // Initialize main menu
    error = getMenu().initialize();
    if (error != 0) {
        printf("Could not initialize Menu (%d)!\n", error);
        return -7;
    }

#ifdef DEBUG
    mFPS = true;
#endif

    getMenu().setVisible(true);
    systemTimerReset();

    return 0;
}

void OpenRaider::run() {
    assert(mRunning == false);
    mRunning = true;
    while (mRunning)
        frame();
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
    if (mFPS) {
        std::ostringstream fpsText;
        fpsText << fps << "FPS";
        getFont().drawText(10, getWindow().getHeight() - 20, 0.5f, BLUE, fpsText.str());
    }

#ifdef DEBUG
    // Draw debug infos
    if (getGame().isLoaded() && (!getMenu().isVisible())) {
        for (int i = 0; i < 3; i++) {
            std::ostringstream axis;
            axis << getGame().getLara().getPos(i) / 256.0f << " (" << getGame().getLara().getAngle(i) << ")";
            getFont().drawText(10, getWindow().getHeight() - ((4 - i) * 20), 0.5f, BLUE, axis.str());
        }
    }
#endif

    getWindow().glExit2D();

    // Put new frame on screen
    getWindow().swapBuffersGL();

    // Calculate FPS display value
    fpsCount++;
    fpsSum += (systemTimerGet() - startTime);
    if (fpsSum >= 250) {
        // Update every 250ms
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
                    getGame().handleAction((ActionEvents)i, !pressed);
                }
            }
        } else {
            getConsole().handleKeyboard(key, pressed);
        }
    } else {
        getMenu().handleKeyboard(key, pressed);
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void OpenRaider::handleText(char *text, bool notFinished) {
    assert(text != NULL);
    assert(text[0] != '\0');
    assert(mRunning == true);

    if ((getConsole().isVisible()) && (!getMenu().isVisible())) {
        getConsole().handleText(text, notFinished);
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void OpenRaider::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    assert(button < unknownKey);
    assert(mRunning == true);

    if (getMenu().isVisible()) {
        getMenu().handleMouseClick(x, y, button, released);
    } else if (!getConsole().isVisible()) {
        for (int i = forwardAction; i < ActionEventCount; i++) {
            if (keyBindings[i] == button) {
                getGame().handleAction((ActionEvents)i, released);
            }
        }
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void OpenRaider::handleMouseMotion(int xrel, int yrel) {
    assert((xrel != 0) || (yrel != 0));
    assert(mRunning == true);

    if ((!getConsole().isVisible()) && (!getMenu().isVisible())) {
        getGame().handleMouseMotion(xrel, yrel);
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void OpenRaider::handleMouseScroll(int xrel, int yrel) {
    assert((xrel != 0) || (yrel != 0));
    assert(mRunning == true);

    if (getMenu().isVisible()) {
        getMenu().handleMouseScroll(xrel, yrel);
    } else if (getConsole().isVisible()) {
        getConsole().handleMouseScroll(xrel, yrel);
    }

    bool mousegrab = !(getMenu().isVisible() || getConsole().isVisible());
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

