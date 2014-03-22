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

    gOpenRaider->mConsole->print("Loading %s", mName);
    int error = mTombRaider.Load(mName, percentCallbackTrampoline, this);
    if (error != 0) {
        throw error;
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

