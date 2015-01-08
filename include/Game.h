/*!
 * \file include/Game.h
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#ifndef _GAME_H_
#define _GAME_H_

#include <string>
#include <vector>

#include "Entity.h"

class Game {
  public:
    Game();

    int initialize();
    void destroy();

    bool isLoaded() { return mLoaded; }
    int loadLevel(const char* level);

    void display();
    void handleAction(ActionEvents action, bool isFinished);
    void handleMouseMotion(int xrel, int yrel, int xabs, int yabs);
    void handleControllerAxis(float value, KeyboardButton axis);
    void handleControllerButton(KeyboardButton button, bool released);

    Entity& getLara();
    void setLara(long lara);

  private:
    bool mLoaded;
    long mLara;
    bool activeEvents[ActionEventCount];
};

Game& getGame();

#endif

