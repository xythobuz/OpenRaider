/*!
 * \file src/OpenRaider.cpp
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#include <fstream>
#include <sstream>

#include "global.h"
#include "commands/Command.h"
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

    for (int i = 0; i < ActionEventCount; i++)
        keyBindings[i] = unknownKey;
}

std::string OpenRaider::getBaseDir() {
    return baseDir;
}

void OpenRaider::setBaseDir(std::string dir) {
    baseDir = dir;
}

std::string OpenRaider::getPakDir() {
    return pakDir;
}

void OpenRaider::setPakDir(std::string dir) {
    pakDir = dir;
}

std::string OpenRaider::getAudioDir() {
    return audioDir;
}

void OpenRaider::setAudioDir(std::string dir) {
    audioDir = dir;
}

std::string OpenRaider::getDataDir() {
    return dataDir;
}

void OpenRaider::setDataDir(std::string dir) {
    dataDir = dir;
}

int OpenRaider::loadConfig(std::string config) {
    std::string configFile = expandHomeDirectory(config);
    getConsole() << "Loading config from \"" << configFile << "\"..." << Console::endl;

    std::ifstream file(configFile);
    if (!file) {
        getConsole() << "Could not open file!" << Console::endl;
        return -1;
    }

    for (std::string line; std::getline(file, line);) {
        if (line.length() == 0)
            continue;

        int error = Command::command(line);
        if (error != 0)
            getConsole() << "Error Code: " << error << Console::endl;
    }

    file.close();

    return 0;
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

    // Initialize UIs
    error = UI::passInitialize();
    if (error != 0) {
        printf("Could not initialize UIs (%d)!\n", error);
        return -6;
    }

#ifdef DEBUG
    mFPS = true;
#endif

    getMenu().moveToTop();
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

    ImGui::SetNewWindowDefaultPos(ImVec2(50, 50));
    bool show_test_window = true;
    ImGui::ShowTestWindow(&show_test_window);

    UI::passDisplay();

    getWindow().glEnter2D();

    // Draw FPS counter
    if (mFPS) {
        std::ostringstream fpsText;
        fpsText << fps << "FPS";
        getFont().drawText(10, getWindow().getHeight() - 20, 0.5f, BLUE, fpsText.str());
    }

#ifdef DEBUG
    // Draw debug infos
    if (getGame().isLoaded() && (!getMenu().isOnTop())) {
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

