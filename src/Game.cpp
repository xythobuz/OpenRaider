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
#include "Window.h"
#include "Game.h"

Game::Game(const char *level) {
}

Game::~Game() {
}

void Game::handleAction(ActionEvents action, bool isFinished) {

}

void Game::handleMouseMotion(int xrel, int yrel) {

}

void Game::display() {
    unsigned char color[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    gOpenRaider->mWindow->drawText(10, 10, 1.50f, color, "Game");
}

