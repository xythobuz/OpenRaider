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

Game::Game() {
    mLoaded = false;
    mName = NULL;
    mLara = NULL;

    mTextureStart = 0;
    mTextureLevelOffset = 0;
    mTextureOffset = 0;

    mCamera = NULL;
    mRender = NULL;
}

Game::~Game() {
    destroy();

    if (mRender)
        delete mRender;

    if (mCamera)
        delete mCamera;
}

int Game::initialize() {
    mCamera = new Camera();
    // TODO setup Camera

    mRender = new Render();
    mRender->initTextures(gOpenRaider->mDataDir, &mTextureStart, &mTextureLevelOffset);

    return 0;
}

void percentCallbackTrampoline(int percent, void *data) {
    Game *self = static_cast<Game *>(data);
    self->percentCallback(percent);
}

void Game::percentCallback(int percent) {

}

int Game::loadLevel(const char *level) {
    destroy();

    mName = bufferString("%s", level);

    // Load the level pak into TombRaider
    gOpenRaider->mConsole->print("Loading %s", mName);
    int error = mTombRaider.Load(mName, percentCallbackTrampoline, this);
    if (error != 0) {
        return error;
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

    mLoaded = true;
    //mRender->setMode(Render::modeTexture);
    // TODO enable Renderer
    return 0;
}

void Game::destroy() {
    if (mName)
        delete [] mName;

    mLoaded = false;
    mRender->setMode(Render::modeDisabled);
}

void Game::handleAction(ActionEvents action, bool isFinished) {
    if (mLoaded) {

    }
}

void Game::handleMouseMotion(int xrel, int yrel) {
    if (mLoaded) {

    }
}

void Game::display() {
    if (mLoaded) {
        //glClearColor(0.00f, 0.70f, 0.00f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

        mRender->Display();
    }
}

