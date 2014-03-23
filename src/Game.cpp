/*!
 * \file src/Game.cpp
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "main.h"
#include "Console.h"
#include "Game.h"
#include "utils/strings.h"

void percentCallbackTrampoline(int percent, void *data) {
    Game *self = static_cast<Game *>(data);
    self->percentCallback(percent);
}

void Game::percentCallback(int percent) {

}

// Throw exception with negative integer error code if fails
Game::Game(const char *level) {
    mName = bufferString("%s", level);

    // Load the level pak into TombRaider
    gOpenRaider->mConsole->print("Loading %s", mName);
    int error = mTombRaider.Load(mName, percentCallbackTrampoline, this);
    if (error != 0) {
        throw error;
    }

    // If required, load the external sound effect file MAIN.SFX into TombRaider
    if ((mTombRaider.getEngine() == TR_VERSION_2) || (mTombRaider.getEngine() == TR_VERSION_3)) {
        char *tmp = bufferString("%sMAIN.SFX", level); // Ensure theres enough space
        size_t length = strlen(tmp);
        size_t dir = 0;
        for (int i = length - 1; i >= 0; i--) {
            if ((tmp[i] == '/') || (tmp[i] == '\\')) {
                dir = i + 1; // Find where the filename (bla.tr2) starts
                break;
            }
        }
        strcpy(tmp + dir, "MAIN.SFX\0"); // overwrite the name itself with MAIN.SFX
        error = mTombRaider.loadSFX(tmp);
        if (error != 0) {
            gOpenRaider->mConsole->print("Could not load %s", tmp);
        }
        delete [] tmp;
    }
}

Game::~Game() {
    if (mName)
        delete [] mName;
}

void Game::handleAction(ActionEvents action, bool isFinished) {

}

void Game::handleMouseMotion(int xrel, int yrel) {

}

void Game::display() {
    glClearColor(0.00f, 0.70f, 0.00f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

