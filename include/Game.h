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
#include "UI.h"
#include "TombRaider.h"

/*!
 * \brief Game abstraction
 */
class Game : public UI {
public:

    Game();
    ~Game();

    int initialize();

    bool isLoaded();

    int loadLevel(const char *level);

    void destroy();

    virtual void display();
    virtual void handleAction(ActionEvents action, bool isFinished);
    virtual void handleMouseMotion(int xrel, int yrel);
    virtual void moveToTop() { }
    virtual void makeInvisible() { }

    unsigned int getTextureStart();
    unsigned int getTextureOffset();

    Entity &getLara();

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

    TombRaider mTombRaider;

    unsigned int mTextureStart;
    unsigned int mTextureOffset;

    long mLara;
};

Game &getGame();

#endif
