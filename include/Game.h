/*!
 * \file include/Game.h
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#ifndef _GAME_H_
#define _GAME_H_

#include "global.h"

/*!
 * \brief Game abstraction
 */
class Game {
public:

    Game(const char *level);

    ~Game();

    void handleAction(ActionEvents action, bool isFinished);

    void handleMouseMotion(int xrel, int yrel);

    void display();

private:

};

#endif
