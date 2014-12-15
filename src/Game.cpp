/*!
 * \file src/Game.cpp
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#include <algorithm>
#include <map>
#include <cstdlib>
#include <cstring>

#include "global.h"
#include "Camera.h"
#include "Game.h"
#include "loader/Loader.h"
#include "Log.h"
#include "Render.h"
#include "SoundManager.h"
#include "StaticMesh.h"
#include "system/Sound.h"
#include "TextureManager.h"
#include "UI.h"
#include "World.h"
#include "utils/strings.h"

Game::Game() {
    mLoaded = false;
    mLara = -1;
}

Game::~Game() {
}

int Game::initialize() {
    // Enable Renderer
    Render::setMode(RenderMode::LoadScreen);

    return 0;
}

void Game::display() {
    Render::display();
}

void Game::destroy() {
    mLoaded = false;
    mLara = -1;
    Render::setMode(RenderMode::LoadScreen);

    getWorld().destroy();
    Sound::clear(); // Remove all previously loaded sounds
    SoundManager::clear();
    getTextureManager().clear();
}

bool Game::isLoaded() {
    return mLoaded;
}

int Game::loadLevel(const char* level) {
    destroy();
    levelName = level;
    getLog() << "Loading " << levelName << Log::endl;
    auto loader = Loader::createLoader(level);
    if (loader) {
        // First Loader test
        getLog() << "Trying to load using new loader..." << Log::endl;
        int error = loader->load(level);
        if (error != 0) {
            getLog() << "Error while trying new loader (" << error << ")..." << Log::endl;
            destroy();
            return -2;
        } else {
            SoundManager::prepareSources();

            if (mLara == -1) {
                getLog() << "Can't find Lara entity in level?!" << Log::endl;
            } else {
                mLoaded = true;
                //Render::setMode(RenderMode::Texture);
            }

            UI::setVisible(true);
            return 0;
        }
    } else {
        getLog() << "No suitable loader for this level!" << Log::endl;
        return -1;
    }
}

void Game::handleAction(ActionEvents action, bool isFinished) {
    if (mLoaded && (!isFinished)) {
        if (action == forwardAction) {
            getLara().move('f');
        } else if (action == backwardAction) {
            getLara().move('b');
        } else if (action == leftAction) {
            getLara().move('l');
        } else if (action == rightAction) {
            getLara().move('r');
        } else if (action == jumpAction) {

        } else if (action == crouchAction) {

        } else if (action == useAction) {

        } else if (action == holsterAction) {

        } else if (action == walkAction) {

        }
    }
}

void Game::handleMouseMotion(int xrel, int yrel, int xabs, int yabs) {
    if (mLoaded) {
        Camera::handleMouseMotion(xrel, yrel);

        /*
        float angles[3] = { 0.0f, getCamera().getRadianYaw(), getCamera().getRadianPitch() };
        getLara().setAngles(angles);
        */
    }
}

Entity& Game::getLara() {
    assert(mLara >= 0);
    assert(mLara < (int)getWorld().sizeEntity());
    return getWorld().getEntity(mLara);
}

void Game::setLara(long lara) {
    assert(lara >= 0);
    assert(lara < getWorld().sizeEntity());
    mLara = lara;
}

