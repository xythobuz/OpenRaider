/*!
 * \file include/Game.h
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#ifndef _GAME_H_
#define _GAME_H_

#include "Camera.h"
#include "global.h"
#include "Render.h"
#include "TombRaider.h"
#include "World.h"

/*!
 * \brief Game abstraction
 */
class Game {
public:

    Game();

    ~Game();

    int initialize();

    int loadLevel(const char *level);

    void destroy();

    void handleAction(ActionEvents action, bool isFinished);

    void handleMouseMotion(int xrel, int yrel);

    void display();

    void percentCallback(int percent);

    World mWorld;
    entity_t *mLara;
    Render *mRender;
    Camera *mCamera;

private:

    bool mLoaded;
    char *mName;
    TombRaider mTombRaider;

    unsigned int mTextureStart;
    unsigned int mTextureLevelOffset;
    unsigned int mTextureOffset;
};

#endif
