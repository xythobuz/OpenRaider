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

    bool isLoaded();

    int loadLevel(const char *level);

    void destroy();

    void handleAction(ActionEvents action, bool isFinished);

    void handleMouseMotion(int xrel, int yrel);

    unsigned int getTextureStart();
    unsigned int getTextureOffset();

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

    char *mName;
    bool mLoaded;

    TombRaider mTombRaider;

    unsigned int mTextureStart;
    unsigned int mTextureOffset;
};

#endif
