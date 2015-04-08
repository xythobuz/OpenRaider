/*!
 * \file src/Game.cpp
 * \brief Gameplay Handler
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "Console.h"
#include "Game.h"
#include "loader/Loader.h"
#include "Log.h"
#include "Menu.h"
#include "Render.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "UI.h"
#include "World.h"

bool Game::mLoaded = false;
long Game::mLara = -1;
bool Game::activeEvents[ActionEventCount];

void Game::destroy() {
    mLoaded = false;
    mLara = -1;

    for (int i = 0; i < ActionEventCount; i++) {
        activeEvents[i] = false;
    }

    Render::setMode(RenderMode::LoadScreen);
    Camera::reset();
    Render::clearRoomList();
    SoundManager::clear();
    TextureManager::clear();
    World::destroy();
}

int Game::loadLevel(std::string level) {
    destroy();

    Log::get(LOG_INFO) << "Loading " << level << Log::endl;
    auto loader = Loader::createLoader(level);
    if (loader) {
        int error = loader->load(level);
        if (error != 0) {
            Log::get(LOG_ERROR) << "Error loading level (" << error << ")..." << Log::endl;
            destroy();
            return -2;
        }

        for (int i = 0; i < World::sizeMesh(); i++) {
            World::getMesh(i).prepare();
        }

        for (int i = 0; i < World::sizeRoom(); i++) {
            World::getRoom(i).prepare();
        }

        SoundManager::prepareSources();
        TextureManager::prepare();

        mLoaded = true;
        Render::setMode(RenderMode::Texture);
        Menu::setVisible(false);

        if (mLara == -1) {
            Log::get(LOG_WARNING) << "Can't find Lara entity in level?!" << Log::endl;
        } else {
            Camera::setPosition(glm::vec3(getLara().getPosition().x,
                                          getLara().getPosition().y - 1024.0f,
                                          getLara().getPosition().z));
            Camera::setRoom(getLara().getRoom());
        }
    } else {
        Log::get(LOG_ERROR) << "No suitable loader for this level!" << Log::endl;
        return -1;
    }

    return 0;
}

void Game::handleAction(ActionEvents action, bool isFinished) {
    orAssertLessThan(action, ActionEventCount);

    if (!mLoaded)
        return;

    if (isFinished) {
        if (activeEvents[action])
            Camera::handleAction(action, isFinished);
        activeEvents[action] = false;
    } else {
        if (!activeEvents[action])
            Camera::handleAction(action, isFinished);
        activeEvents[action] = true;
    }
}

void Game::handleMouseMotion(int xrel, int yrel, int xabs, int yabs) {
    if (!mLoaded)
        return;

    Camera::handleMouseMotion(xrel, yrel);
}

void Game::handleControllerAxis(float value, KeyboardButton axis) {
    if (!mLoaded)
        return;

    Camera::handleControllerAxis(value, axis);
}

Entity& Game::getLara() {
    orAssertGreaterThanEqual(mLara, 0);
    orAssertLessThan(mLara, World::sizeEntity());
    return World::getEntity(mLara);
}

void Game::setLara(long lara) {
    orAssertGreaterThanEqual(lara, 0);
    orAssertLessThan(lara, World::sizeEntity());
    mLara = lara;
}

