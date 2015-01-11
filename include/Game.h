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
    static void destroy();

    static bool isLoaded() { return mLoaded; }
    static int loadLevel(const char* level);

    static void handleAction(ActionEvents action, bool isFinished);
    static void handleMouseMotion(int xrel, int yrel, int xabs, int yabs);
    static void handleControllerAxis(float value, KeyboardButton axis);
    static void handleControllerButton(KeyboardButton button, bool released);

    static Entity& getLara();
    static void setLara(long lara);

  private:
    static bool mLoaded;
    static long mLara;
    static bool activeEvents[ActionEventCount];
};

#endif

