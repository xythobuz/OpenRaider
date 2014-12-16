/*!
 * \file src/Game.cpp
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#include <algorithm>
#include <map>
#include <sstream>
#include <cstdlib>
#include <cstring>

#include "global.h"
#include "Camera.h"
#include "Game.h"
#include "loader/Loader.h"
#include "Log.h"
#include "Render.h"
#include "RunTime.h"
#include "SoundManager.h"
#include "StaticMesh.h"
#include "system/Font.h"
#include "system/Sound.h"
#include "system/Window.h"
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

    if (getRunTime().getShowFPS()) {
        std::ostringstream s;
        s << getRunTime().getFPS() << "FPS";
        Font::drawText(10, getWindow().getHeight() - 25, 0.6f, BLUE, s.str());

        s.str("");
        s << "X: " << Camera::getPosition().x << " (" << Camera::getRadianPitch() << ")";
        Font::drawText(10, getWindow().getHeight() - 70, 0.6f, BLUE, s.str());

        s.str("");
        s << "Y: " << Camera::getPosition().y << " (" << Camera::getRadianYaw() << ")";
        Font::drawText(10, getWindow().getHeight() - 55, 0.6f, BLUE, s.str());

        s.str("");
        s << "Z: " << Camera::getPosition().z;
        Font::drawText(10, getWindow().getHeight() - 40, 0.6f, BLUE, s.str());
    }
}

void Game::destroy() {
    mLoaded = false;
    mLara = -1;
    Render::setMode(RenderMode::LoadScreen);

    Camera::reset();
    SoundManager::clear();
    getTextureManager().clear();
    getWorld().destroy();
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
        int error = loader->load(level);
        if (error != 0) {
            getLog() << "Error loading level (" << error << ")..." << Log::endl;
            destroy();
            return -2;
        }

        for (int i = 0; i < getWorld().sizeRoom(); i++) {
            getWorld().getRoom(i).prepare();
        }

        SoundManager::prepareSources();

        if (mLara == -1) {
            getLog() << "Can't find Lara entity in level?!" << Log::endl;
            UI::setVisible(true);
        } else {
            mLoaded = true;
            Render::setMode(RenderMode::Texture);

            Camera::setPosition(glm::vec3(getLara().getPos(0),
                                          getLara().getPos(1) + 1024.0f,
                                          getLara().getPos(2)));
        }
    } else {
        getLog() << "No suitable loader for this level!" << Log::endl;
        return -1;
    }

    return 0;
}

void Game::handleAction(ActionEvents action, bool isFinished) {
    if (isFinished || (!mLoaded))
        return;

    Camera::handleAction(action, isFinished);
}

void Game::handleMouseMotion(int xrel, int yrel, int xabs, int yabs) {
    if (!mLoaded)
        return;

    Camera::handleMouseMotion(xrel, yrel);

    /* TODO
    float angles[3] = { 0.0f, getCamera().getRadianYaw(), getCamera().getRadianPitch() };
    getLara().setAngles(angles);
    */
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

