/*!
 * \file include/Game.h
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#ifndef _GAME_H_
#define _GAME_H_

#include <vector>

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

    int command(std::vector<char *> *args);

    //! \fixme should be private
    entity_t *mLara;

private:

    void processPakSounds();
    void processTextures();
    void processSprites();
    void processMoveables();
    void processMoveable(int index, int i, int *ent,
                            std::vector<skeletal_model_t *> &cache2,
                            std::vector<unsigned int> &cache, int object_id);
    void processModels();
    void processRooms();
    void setupTextureColor(texture_tri_t *r_tri, float *colorf);

    bool mLoaded;
    char *mName;
    TombRaider mTombRaider;

    unsigned int mTextureStart;
    unsigned int mTextureLevelOffset;
    unsigned int mTextureOffset;
};

#endif
