/*!
 * \file src/Game.cpp
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#include <algorithm>
#include <map>
#include <cstdlib>
#include <cstring>

#include "global.h"
#include "Camera.h"
#include "Console.h"
#include "Game.h"
#include "loader/Loader.h"
#include "OpenRaider.h"
#include "Render.h"
#include "Sound.h"
#include "StaticMesh.h"
#include "TextureManager.h"
#include "World.h"
#include "utils/strings.h"

#include "games/TombRaider1.h"

#ifdef MULTITEXTURE
std::map<int, int> gMapTex2Bump;
#endif

Game::Game() {
    mLoaded = false;
    mLara = -1;
    mTextureStart = 0;
    mTextureOffset = 0;
}

Game::~Game() {
    destroy();
}

unsigned int Game::getTextureStart() {
    return mTextureStart;
}

unsigned int Game::getTextureOffset() {
    return mTextureOffset;
}

int Game::initialize() {
    // Enable Renderer
    getRender().setMode(Render::modeLoadScreen);

    mTextureStart = getTextureManager().getTextureCount();

    return 0;
}

void Game::destroy() {
    mLoaded = false;
    mLara = -1;
    getRender().setMode(Render::modeDisabled);

    getWorld().destroy();
    getRender().ClearWorld();
    getSound().clear(); // Remove all previously loaded sounds
}

bool Game::isLoaded() {
    return mLoaded;
}

int Game::loadLevel(const char *level) {
    if (mLoaded)
        destroy();

    levelName = level;

    getConsole() << "Loading " << levelName << Console::endl;

    int error = 0;
    auto loader = Loader::createLoader(level);
    if (loader) {
        // First Loader test
        error = loader->load(level);
        if (error != 0) {
            return error;
        }

        // And now...?

        getConsole() << "Tried Loader..." << Console::endl;
    }

    if ((!loader) || (error == 0)) {
        // Old TombRaider level loader
        error = mTombRaider.Load(levelName.c_str());
        if (error != 0)
            return error;

        // If required, load the external sound effect file MAIN.SFX into TombRaider
        if ((mTombRaider.getEngine() == TR_VERSION_2) || (mTombRaider.getEngine() == TR_VERSION_3)) {
            std::string tmp(levelName);
            size_t pos = tmp.rfind('/');
            tmp.erase(pos + 1);
            tmp += "MAIN.SFX";
            error = mTombRaider.loadSFX(tmp.c_str());
            if (error != 0)
                getConsole() << "Could not load " << tmp << Console::endl;
        }

        // Process data
        processTextures();
        processRooms();
        processModels();
        processSprites();
        processMoveables();
        processPakSounds();

        mTombRaider.reset();

        if (mLara == -1) {
            //! \todo Cutscene support
            getConsole() << "Can't find Lara entity in level pak!" << Console::endl;
            destroy();
            return -1;
        } else {
            mLoaded = true;
            getRender().setMode(Render::modeVertexLight);
            return 0;
        }
    }
}

void Game::handleAction(ActionEvents action, bool isFinished) {
    if (mLoaded && (!isFinished)) {
        if (action == forwardAction) {
            getLara().move('f');
        } else if (action == backwardAction) {
            getLara().move('b');
        } else if (action == leftAction) {
            getLara().move('l');
        } else if (action == rightAction) {
            getLara().move('r');
        } else if (action == jumpAction) {

        } else if (action == crouchAction) {

        } else if (action == useAction) {

        } else if (action == holsterAction) {

        } else if (action == walkAction) {

        }
    }
}

void Game::handleMouseMotion(int xrel, int yrel) {
    if (mLoaded) {
        // Move Camera on X Axis
        if (xrel > 0)
            while (xrel-- > 0)
                getCamera().command(CAMERA_ROTATE_RIGHT);
        else if (xrel < 0)
            while (xrel++ < 0)
                getCamera().command(CAMERA_ROTATE_LEFT);

        // Move Camera on Y Axis
        if (yrel > 0)
            while (yrel-- > 0)
                getCamera().command(CAMERA_ROTATE_UP);
        else if (yrel < 0)
            while (yrel++ < 0)
                getCamera().command(CAMERA_ROTATE_DOWN);

        // Fix Laras rotation
        float angles[3] = { 0.0f, getCamera().getRadianYaw(), getCamera().getRadianPitch() };
        getLara().setAngles(angles);
    }
}

Entity &Game::getLara() {
    assert(mLara >= 0);
    assert(mLara < (int)getWorld().sizeEntity());
    return getWorld().getEntity(mLara);
}

void Game::processSprites() {
    for (int i = 0; i < (mTombRaider.NumItems() - 1); i++) {
        if ((mTombRaider.Engine() == TR_VERSION_1) && (mTombRaider.Item()[i].intensity1 == -1))
            continue;

        for (int j = 0; j < mTombRaider.NumSpriteSequences(); j++) {
            if (mTombRaider.SpriteSequence()[j].object_id == mTombRaider.Item()[i].object_id)
                getWorld().addSprite(*new SpriteSequence(mTombRaider, i, j));
        }
    }

    getConsole() << "Found " << mTombRaider.NumSpriteSequences() << " sprites." << Console::endl;
}

void Game::processRooms() {
    for (int index = 0; index < mTombRaider.NumRooms(); index++)
        getWorld().addRoom(*new Room(mTombRaider, index));

    getConsole() << "Found " << mTombRaider.NumRooms() << " rooms." << Console::endl;
}

void Game::processModels() {
    for (int index = 0; index < mTombRaider.getMeshCount(); index++)
        getWorld().addStaticMesh(*new StaticMesh(mTombRaider, index));

    getConsole() << "Found " << mTombRaider.getMeshCount() << " meshes." << Console::endl;
}

void Game::processPakSounds()
{
    unsigned char *riff;
    unsigned int riffSz;
    //tr2_sound_source_t *sound;
    //tr2_sound_details_t *detail;
    //float pos[3];
    unsigned int i;
    unsigned long id;

    /* detail
       short sample;
       short volume;
       short sound_range;
       short flags;         // bits 8-15: priority?, 2-7: number of sound samples
    // in this group, bits 0-1: channel number
    */

    for (i = 0; i < mTombRaider.getSoundSamplesCount(); ++i)
    {
        mTombRaider.getSoundSample(i, &riffSz, &riff);

        getSound().addWave(riff, riffSz, &id, Sound::SoundFlagsNone);

        //if (((i + 1) == TR_SOUND_F_PISTOL) && (id > 0))
        //{
            //m_testSFX = id;
        //}

        delete [] riff;

        // sound[i].sound_id; // internal sound index
        // sound[i].flags;  // 0x40, 0x80, or 0xc0
        //pos[0] = sound[i].x;
        //pos[1] = sound[i].y;
        //pos[2] = sound[i].z;
        //getSound().SourceAt(id, pos);
    }

    getConsole() << "Found " << mTombRaider.getSoundSamplesCount() << " sound samples." << Console::endl;
}

void Game::processTextures()
{
    unsigned char *image;
    unsigned char *bumpmap;
    int i;

    //if ( mTombRaider.getNumBumpMaps())
    //  gBumpMapStart = mTombRaider.NumTextures();

    for (i = 0; i < mTombRaider.NumTextures(); ++i)
    {
        mTombRaider.Texture(i, &image, &bumpmap);

        // Overwrite any previous level textures on load
        getTextureManager().loadBufferSlot(image, 256, 256,
                RGBA, 32, (mTextureStart - 1) + i);

#ifdef MULTITEXTURE
        gMapTex2Bump[(mTextureStart - 1) + i] = -1;
#endif

        if (bumpmap)
        {
#ifdef MULTITEXTURE
            gMapTex2Bump[(mTextureStart - 1) + i] = (mTextureStart - 1) + i +
                    mTombRaider.NumTextures();
#endif
            getTextureManager().loadBufferSlot(bumpmap, 256, 256,
                    RGBA, 32,
                    (mTextureStart - 1) + i + mTombRaider.NumTextures());
        }

        if (image)
            delete [] image;

        if (bumpmap)
            delete [] bumpmap;
    }

    mTextureOffset = (mTextureStart - 1) + mTombRaider.NumTextures();

    getConsole() << "Found " << mTombRaider.NumTextures() << " textures." << Console::endl;
}

void Game::processMoveables()
{
    unsigned int statCount = 0;

    tr2_moveable_t *moveable = mTombRaider.Moveable();
    tr2_item_t *item = mTombRaider.Item();
    tr2_sprite_sequence_t *sprite_sequence = mTombRaider.SpriteSequence();

    for (int i = 0; i < mTombRaider.NumItems(); ++i)
    {
        int j;
        int object_id = item[i].object_id;

        // It may not be a moveable, test for sprite
        if (!(mTombRaider.Engine() == TR_VERSION_1 && item[i].intensity1 == -1)) {
            for (j = 0; j < (int)mTombRaider.NumSpriteSequences(); ++j) {
                if (sprite_sequence[j].object_id == object_id)
                    break;
            }

            // It's not a moveable, skip sprite
            if (j < (int)mTombRaider.NumSpriteSequences())
                continue;
        }

        for (j = 0; j < (int)mTombRaider.NumMoveables(); ++j) {
            if ((int)moveable[j].object_id == object_id)
                break;
        }

        // It's not a moveable or even a sprite? Skip unknown
        if (j == (int)mTombRaider.NumMoveables())
            continue;

        processMoveable(j, i, object_id);
        statCount++;
    }

    /*
    // Get models that aren't items
    for (int i = 0; i < mTombRaider.NumMoveables(); ++i)
    {
        switch ((int)moveable[i].object_id)
        {
            case 30:
            case 2: // Which tr needs this as model again?
                processMoveable(i, i, (int)moveable[i].object_id);
                break;
            default:
                switch (mTombRaider.Engine())
                {
                    case TR_VERSION_1:
                        switch ((int)moveable[i].object_id)
                        {
                            case TombRaider1::LaraMutant:
                                processMoveable(i, i, (int)moveable[i].object_id);
                                break;
                        }
                        break;
                    case TR_VERSION_4:
                        switch ((int)moveable[i].object_id)
                        {
                            case TR4_PISTOLS_ANIM:
                            case TR4_UZI_ANIM:
                            case TR4_SHOTGUN_ANIM:
                            case TR4_CROSSBOW_ANIM:
                            case TR4_GRENADE_GUN_ANIM:
                            case TR4_SIXSHOOTER_ANIM:
                                processMoveable(i, i, (int)moveable[i].object_id);
                                break;
                        }
                        break;
                    case TR_VERSION_2:
                    case TR_VERSION_3:
                    case TR_VERSION_5:
                    case TR_VERSION_UNKNOWN:
                        break;
                }
        }
    }
    */

    getConsole() << "Found " << mTombRaider.NumMoveables() + statCount << " moveables." << Console::endl;
}

// index moveable, i item, sometimes both moveable
// object_id of item or moveable
void Game::processMoveable(int index, int i, int object_id) {
    // Check if the SkeletalModel is already cached
    bool cached = false;
    unsigned int model;
    for (model = 0; model < getWorld().sizeSkeletalModel(); model++) {
        if (getWorld().getSkeletalModel(model).getId() == object_id) {
            cached = true;
            break;
        }
    }

    // Create a new SkeletalModel, if needed
    if (!cached)
        getWorld().addSkeletalModel(*new SkeletalModel(mTombRaider, index, object_id));

    // Create a new Entity, using the cached or the new SkeletalModel
    Entity &entity = *new Entity(mTombRaider, index, i, model);
    getWorld().addEntity(entity);

    // Store reference to Lara
    if (entity.getObjectId() == 0)
        mLara = getWorld().sizeEntity() - 1;

    // Store reference to the SkyMesh
    if (i == mTombRaider.getSkyModelId())
        getRender().setSkyMesh(i, //moveable[i].starting_mesh,
                (mTombRaider.Engine() == TR_VERSION_2));
}

