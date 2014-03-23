/*!
 * \file include/Game.h
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#ifndef _GAME_H_
#define _GAME_H_

#include "global.h"
#include "TombRaider.h"
#include "World.h"

/*!
 * \brief Game abstraction
 */
class Game {
public:

    // Throw exception with negative integer error code if fails
    Game(const char *level);

    ~Game();

    void handleAction(ActionEvents action, bool isFinished);

    void handleMouseMotion(int xrel, int yrel);

    void display();

    void percentCallback(int percent);

    World mWorld;
    entity_t *mLara;

private:

    char *mName;
    TombRaider mTombRaider;

};

#endif
