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
    ~Game();

    int initialize();

    bool isLoaded();

    int loadLevel(const char* level);

    void destroy();

    void display();
    void handleAction(ActionEvents action, bool isFinished);
    void handleMouseMotion(int xrel, int yrel, int xabs, int yabs);

    Entity& getLara();
    void setLara(long lara);

  private:

    void processPakSounds();
    void processTextures();
    void processSprites();
    void processMoveables();
    void processMoveable(int index, int i, int object_id);
    void processModels();
    void processRooms();

    std::string levelName;
    bool mLoaded;

    long mLara;
};

Game& getGame();

#endif
