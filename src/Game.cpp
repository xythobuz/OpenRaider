/*!
 * \file src/Game.cpp
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <algorithm>
#include <map>
#include <vector>
#include <cstdlib>

#include "main.h"
#include "Console.h"
#include "Game.h"
#include "utils/strings.h"

#include "games/TombRaider1.h"

// Old Code compatibility
#define TexelScale 256.0f
#define TextureLimit 24
std::map<int, int> gMapTex2Bump;
skeletal_model_t *gLaraModel;
std::vector<unsigned int> gColorTextureHACK;

Game::Game() {
    mLoaded = false;
    mName = NULL;
    mLara = NULL;
    mFlags = 0;

    mTextureStart = 0;
    mTextureLevelOffset = 0;
    mTextureOffset = 0;

    mCamera = NULL;
    mRender = NULL;
}

Game::~Game() {
    destroy();

    if (mRender)
        delete mRender;

    if (mCamera)
        delete mCamera;
}

int Game::initialize() {
    // Set up Camera
    vec3_t up = {0.0f, -1.0f, 0.0f};
    mCamera = new Camera();
    mCamera->reset();
    mCamera->setSpeed(512);
    mCamera->setUp(up);

    // Set up Renderer
    mRender = new Render();
    mRender->initTextures(gOpenRaider->mDataDir, &mTextureStart, &mTextureLevelOffset);

    // Enable Renderer
    mRender->setMode(Render::modeLoadScreen);

    // Enable World Hopping
    mWorld.setFlag(World::fEnableHopping);

    return 0;
}

void Game::destroy() {
    if (mName)
        delete [] mName;

    mLoaded = false;
    mRender->setMode(Render::modeDisabled);

    mRender->ClearWorld();
    mWorld.destroy();
    gOpenRaider->mSound->clear(); // Remove all previously loaded sounds
}

int Game::loadLevel(const char *level) {
    if (mLoaded)
        destroy();

    mName = bufferString("%s", level);

    // Load the level pak into TombRaider
    gOpenRaider->mConsole->print("Loading %s", mName);
    int error = mTombRaider.Load(mName);
    if (error != 0) {
        return error;
    }

    // If required, load the external sound effect file MAIN.SFX into TombRaider
    if ((mTombRaider.getEngine() == TR_VERSION_2) || (mTombRaider.getEngine() == TR_VERSION_3)) {
        char *tmp = bufferString("%sMAIN.SFX", level); // Ensure theres enough space
        size_t length = strlen(tmp);
        size_t dir = 0;
        for (int i = length - 1; i >= 0; i--) {
            if ((tmp[i] == '/') || (tmp[i] == '\\')) {
                dir = i + 1; // Find where the filename (bla.tr2) starts
                break;
            }
        }
        strcpy(tmp + dir, "MAIN.SFX"); // overwrite the name itself with MAIN.SFX
        tmp[dir + 8] = '\0';
        error = mTombRaider.loadSFX(tmp);
        if (error != 0) {
            gOpenRaider->mConsole->print("Could not load %s", tmp);
        }
        delete [] tmp;
    }

    // Process data
    processTextures();
    processRooms();
    processModels();
    processSprites();
    processMoveables();
    processPakSounds();

    // Free pak file
    mTombRaider.reset();

    // Check if the level contains Lara
    if (mLara == NULL) {
        gOpenRaider->mConsole->print("Can't find Lara entity in level pak!");
        return -1;
    }

    mLoaded = true;
    mRender->setMode(Render::modeVertexLight);

    return 0;
}

void Game::handleAction(ActionEvents action, bool isFinished) {
    if (mLoaded) {
        if (action == forwardAction) {
            mWorld.moveEntity(mLara, 'f');
        } else if (action == backwardAction) {
            mWorld.moveEntity(mLara, 'b');
        } else if (action == leftAction) {
            mWorld.moveEntity(mLara, 'l');
        } else if (action == rightAction) {
            mWorld.moveEntity(mLara, 'r');
        }
    }
}

void Game::handleMouseMotion(int xrel, int yrel) {
    if (mLoaded) {
        // Move Camera on X Axis
        if (xrel > 0)
            while (xrel-- > 0)
                mCamera->command(CAMERA_ROTATE_RIGHT);
        else if (xrel < 0)
            while (xrel++ < 0)
                mCamera->command(CAMERA_ROTATE_LEFT);

        // Move Camera on Y Axis
        if (yrel > 0)
            while (yrel-- > 0)
                mCamera->command(CAMERA_ROTATE_UP);
        else if (yrel < 0)
            while (yrel++ < 0)
                mCamera->command(CAMERA_ROTATE_DOWN);

        // Fix Laras rotation
        mLara->angles[1] = mCamera->getRadianYaw();
        mLara->angles[2] = mCamera->getRadianPitch();
    }
}

void Game::display() {
    mRender->Display();
}

int Game::command(std::vector<char *> *args) {
    if (args->size() < 1) {
        gOpenRaider->mConsole->print("Invalid use of game-command!");
        return -1;
    }

    char *cmd = args->at(0);
    if (strcmp(cmd, "mode") == 0) {
        if (args->size() > 1) {
            char *mode = args->at(1);
            if (strcmp(mode, "wireframe") == 0) {
                if (mLoaded) {
                    mRender->setMode(Render::modeWireframe);
                    gOpenRaider->mConsole->print("Wireframe mode");
                } else {
                    gOpenRaider->mConsole->print("Load a level to set this mode!");
                    return -2;
                }
            } else if (strcmp(mode, "solid") == 0) {
                if (mLoaded) {
                    mRender->setMode(Render::modeSolid);
                    gOpenRaider->mConsole->print("Solid mode");
                } else {
                    gOpenRaider->mConsole->print("Load a level to set this mode!");
                    return -3;
                }
            } else if (strcmp(mode, "texture") == 0) {
                if (mLoaded) {
                    mRender->setMode(Render::modeTexture);
                    gOpenRaider->mConsole->print("Texture mode");
                } else {
                    gOpenRaider->mConsole->print("Load a level to set this mode!");
                    return -4;
                }
            } else if (strcmp(mode, "vertexlight") == 0) {
                if (mLoaded) {
                    mRender->setMode(Render::modeVertexLight);
                    gOpenRaider->mConsole->print("Vertexlight mode");
                } else {
                    gOpenRaider->mConsole->print("Load a level to set this mode!");
                    return -5;
                }
            } else if (strcmp(mode, "titlescreen") == 0) {
                mRender->setMode(Render::modeLoadScreen);
                gOpenRaider->mConsole->print("Titlescreen mode");
            } else {
                gOpenRaider->mConsole->print("Invalid use of mode command (%s)!", mode);
                return -6;
            }
        } else {
            gOpenRaider->mConsole->print("Invalid use of mode command!");
            return -7;
        }
    } else if (strcmp(cmd, "move") == 0) {
        if (args->size() > 1) {
            if (mLoaded) {
                char *move = args->at(1);
                if (strcmp(move, "walk") == 0) {
                    mLara->moveType = worldMoveType_walk;
                    gOpenRaider->mConsole->print("Lara is walking...");
                } else if (strcmp(move, "fly") == 0) {
                    mLara->moveType = worldMoveType_fly;
                    gOpenRaider->mConsole->print("Lara is flying...");
                } else if (strcmp(move, "noclip") == 0) {
                    mLara->moveType = worldMoveType_noClipping;
                    gOpenRaider->mConsole->print("Lara is noclipping...");
                } else {
                    gOpenRaider->mConsole->print("Invalid use of move command (%s)!", move);
                    return -8;
                }
            } else {
                gOpenRaider->mConsole->print("Load a level to change the movement type!");
                return -9;
            }
        } else {
            gOpenRaider->mConsole->print("Invalid use of move command!");
            return -10;
        }
    } else if (strcmp(cmd, "sound") == 0) {
        if (args->size() > 1) {
            gOpenRaider->mSound->play(atoi(args->at(1)));
        } else {
            gOpenRaider->mConsole->print("Invalid use of sound command!");
            return -11;
        }
    } else if (strcmp(cmd, "animate") == 0) {
        if (args->size() > 1) {
            char c = args->at(1)[0];
            if (c == 'n') {
                // Step all skeletal models to their next animation
                if (mRender->getFlags() & Render::fAnimateAllModels) {
                    for (unsigned int i = 0; i < mRender->mModels.size(); i++) {
                        SkeletalModel *m = mRender->mModels[i];
                        if (m->getAnimation() < ((int)m->model->animation.size() - 1))
                            m->setAnimation(m->getAnimation() + 1);
                        else
                            if (m->getAnimation() != 0)
                                m->setAnimation(0);
                    }
                } else {
                    gOpenRaider->mConsole->print("Animations need to be enabled!");
                }
            } else if (c == 'p') {
                // Step all skeletal models to their previous animation
                if (mRender->getFlags() & Render::fAnimateAllModels) {
                    for (unsigned int i = 0; i < mRender->mModels.size(); i++) {
                        SkeletalModel *m = mRender->mModels[i];
                        if (m->getAnimation() > 0)
                            m->setAnimation(m->getAnimation() - 1);
                        else
                            if (m->model->animation.size() > 0)
                                m->setAnimation(m->model->animation.size() - 1);
                    }
                } else {
                    gOpenRaider->mConsole->print("Animations need to be enabled!");
                }
            } else {
                // Enable or disable animating all skeletal models
                bool b;
                if (readBool(args->at(1), &b) < 0) {
                    gOpenRaider->mConsole->print("Pass BOOL to animate command!");
                    return -12;
                }
                if (b)
                    mRender->setFlags(Render::fAnimateAllModels);
                else
                    mRender->clearFlags(Render::fAnimateAllModels);
                gOpenRaider->mConsole->print(b ? "Animating all models" : "No longer animating all models");
            }
        } else {
            gOpenRaider->mConsole->print("Invalid use of animate command!");
            return -13;
        }
    } else if (strcmp(cmd, "help") == 0) {
        if (args->size() < 2) {
            gOpenRaider->mConsole->print("game-command Usage:");
            gOpenRaider->mConsole->print("  game COMMAND");
            gOpenRaider->mConsole->print("Available commands:");
            gOpenRaider->mConsole->print("  move [walk|fly|noclip]");
            gOpenRaider->mConsole->print("  sound INT");
            gOpenRaider->mConsole->print("  mode MODE");
            gOpenRaider->mConsole->print("  animate [BOOL|n|p]");
        } else if (strcmp(args->at(1), "sound") == 0) {
            gOpenRaider->mConsole->print("game-sound-command Usage:");
            gOpenRaider->mConsole->print("  game sound INT");
            gOpenRaider->mConsole->print("Where INT is a valid sound ID integer");
        } else if (strcmp(args->at(1), "move") == 0) {
            gOpenRaider->mConsole->print("game-move-command Usage:");
            gOpenRaider->mConsole->print("  game move COMMAND");
            gOpenRaider->mConsole->print("Where COMMAND is one of the following:");
            gOpenRaider->mConsole->print("  walk");
            gOpenRaider->mConsole->print("  fly");
            gOpenRaider->mConsole->print("  noclip");
        } else if (strcmp(args->at(1), "mode") == 0) {
            gOpenRaider->mConsole->print("game-mode-command Usage:");
            gOpenRaider->mConsole->print("  game mode MODE");
            gOpenRaider->mConsole->print("Where MODE is one of the following:");
            gOpenRaider->mConsole->print("  wireframe");
            gOpenRaider->mConsole->print("  solid");
            gOpenRaider->mConsole->print("  texture");
            gOpenRaider->mConsole->print("  vertexlight");
            gOpenRaider->mConsole->print("  titlescreen");
        } else if (strcmp(args->at(1), "animate") == 0) {
            gOpenRaider->mConsole->print("game-animate-command Usage:");
            gOpenRaider->mConsole->print("  game animate [n|p|BOOL]");
            gOpenRaider->mConsole->print("Where the commands have the following meaning:");
            gOpenRaider->mConsole->print("  BOOL to (de)activate animating all models");
            gOpenRaider->mConsole->print("  n to step all models to their next animation");
            gOpenRaider->mConsole->print("  p to step all models to their previous animation");
        } else {
            gOpenRaider->mConsole->print("No help available for game %s.", args->at(1));
            return -14;
        }
    } else {
        gOpenRaider->mConsole->print("Invalid use of game-command (%s)!", cmd);
        return -15;
    }

    return 0;
}

void Game::processPakSounds()
{
    unsigned char *riff;
    unsigned int riffSz;
    //tr2_sound_source_t *sound;
    //tr2_sound_details_t *detail;
    //float pos[3];
    unsigned int i;
    int id;

    /* detail
       short sample;
       short volume;
       short sound_range;
       short flags;         // bits 8-15: priority?, 2-7: number of sound samples
    // in this group, bits 0-1: channel number
    */

    printf("Processing pak sound files: ");

    for (i = 0; i < mTombRaider.getSoundSamplesCount(); ++i)
    {
        mTombRaider.getSoundSample(i, &riffSz, &riff);

        gOpenRaider->mSound->addWave(riff, riffSz, &id, gOpenRaider->mSound->SoundFlagsNone);

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
        //gOpenRaider->mSound->SourceAt(id, pos);

        //printf(".");
        //fflush(stdout);
    }

    printf("Done! Found %u files.\n", mTombRaider.getSoundSamplesCount());
}

void Game::processTextures()
{
    unsigned char *image;
    unsigned char *bumpmap;
    int i;

    printf("Processing TR textures: ");

    //if ( mTombRaider.getNumBumpMaps())
    //  gBumpMapStart = mTombRaider.NumTextures();

    for (i = 0; i < mTombRaider.NumTextures(); ++i)
    {
        mTombRaider.Texture(i, &image, &bumpmap);

        // Overwrite any previous level textures on load
        mRender->loadTexture(image, 256, 256, mTextureLevelOffset + i);

        gMapTex2Bump[mTextureLevelOffset + i] = -1;

        if (bumpmap)
        {
            gMapTex2Bump[mTextureLevelOffset + i] = mTextureLevelOffset + i +
                    mTombRaider.NumTextures();
            mRender->loadTexture(bumpmap, 256, 256, mTextureLevelOffset + i +
                    mTombRaider.NumTextures());
        }

        if (image)
            delete [] image;

        if (bumpmap)
            delete [] bumpmap;

        //printf(".");
        //fflush(stdout);
    }

    mTextureOffset = mTextureLevelOffset + mTombRaider.NumTextures();

    printf("Done! Found %d textures.\n", mTombRaider.NumTextures());
}

void Game::processSprites()
{
    int i, j, k, l, x, y, s_index, width, height;
    float scale, width2, height2;
    tr2_sprite_texture_t *sprite;
    tr2_sprite_texture_t *sprite_textures;
    tr2_sprite_sequence_t *sprite_sequence;
    sprite_seq_t *r_mesh;
    tr2_item_t *item;


    item = mTombRaider.Item();
    sprite_textures = mTombRaider.Sprite();
    sprite_sequence = mTombRaider.SpriteSequence();
    scale = 4.0;

    printf("Processing sprites: ");

    for (i = 0; i < mTombRaider.NumItems() - 1; ++i)
    {
        // It's a mesh, skip it
        if (mTombRaider.Engine() == TR_VERSION_1 && item[i].intensity1 == -1)
            continue;

        k = item[i].object_id;

        // Search the SpriteSequence list
        //  (if we didn't already decide that it's a mesh)
        for (j = 0; j < (int)mTombRaider.NumSpriteSequences(); ++j)
        {
            if (sprite_sequence[j].object_id == k)
            {
                k = item[i].object_id;

                s_index = sprite_sequence[j].offset;

                r_mesh = new sprite_seq_t;
                mWorld.addSprite(r_mesh);
                r_mesh->num_sprites = -sprite_sequence[j].negative_length;
                r_mesh->sprite = new sprite_t[r_mesh->num_sprites];

                for (l = 0; l < r_mesh->num_sprites; ++l)
                {
                    sprite = &sprite_textures[s_index];

                    width = sprite->width >> 8;
                    height = sprite->height >> 8;
                    x = sprite->x;
                    y = sprite->y;
                    width2 = width * scale;
                    height2 = height * scale;

                    // For vising use
                    r_mesh->sprite[l].pos[0] = item[i].x;
                    r_mesh->sprite[l].pos[1] = item[i].y;
                    r_mesh->sprite[l].pos[2] = item[i].z;
                    r_mesh->sprite[l].texture = sprite->tile + mTextureStart;

                    r_mesh->sprite[l].radius = width2 / 2.0f;

                    r_mesh->sprite[l].vertex[0].pos[0] = -width2 / 2.0f;
                    r_mesh->sprite[l].vertex[1].pos[0] = -width2 / 2.0f;
                    r_mesh->sprite[l].vertex[2].pos[0] = width2 / 2.0f;
                    r_mesh->sprite[l].vertex[3].pos[0] = width2 / 2.0f;

                    r_mesh->sprite[l].vertex[0].pos[1] = 0;
                    r_mesh->sprite[l].vertex[1].pos[1] = -height2;
                    r_mesh->sprite[l].vertex[2].pos[1] = -height2;
                    r_mesh->sprite[l].vertex[3].pos[1] = 0;

                    r_mesh->sprite[l].vertex[0].pos[2] = 0;
                    r_mesh->sprite[l].vertex[1].pos[2] = 0;
                    r_mesh->sprite[l].vertex[2].pos[2] = 0;
                    r_mesh->sprite[l].vertex[3].pos[2] = 0;

                    r_mesh->sprite[l].texel[3].st[0] = (vec_t)(x+width)/TexelScale;
                    r_mesh->sprite[l].texel[3].st[1] = (vec_t)(y+height)/TexelScale;

                    r_mesh->sprite[l].texel[2].st[0] = (vec_t)(x+width)/TexelScale;
                    r_mesh->sprite[l].texel[2].st[1] = (vec_t)(y)/TexelScale;

                    r_mesh->sprite[l].texel[1].st[0] = (vec_t)(x) /TexelScale;
                    r_mesh->sprite[l].texel[1].st[1] = (vec_t)(y) /TexelScale;

                    r_mesh->sprite[l].texel[0].st[0] = (vec_t)(x) / TexelScale;
                    r_mesh->sprite[l].texel[0].st[1] = (vec_t)(y+height)/TexelScale;

                    //printf(".");
                    //fflush(stdout);
                }
            }
        }
    }

    printf("Done! Found %d sprites.\n", mTombRaider.NumSpriteSequences());
}

void Game::processMoveables()
{
    std::vector<unsigned int> cache;
    std::vector<skeletal_model_t *> cache2;
    tr2_mesh_t *mesh = NULL;
    tr2_moveable_t *moveable = NULL;
    tr2_meshtree_t *meshtree = NULL;
    tr2_item_t *item = NULL;
    tr2_animation_t *animation = NULL;
    unsigned short *frame = NULL;
    tr2_sprite_sequence_t *sprite_sequence = NULL;
    tr2_object_texture_t *object_texture = NULL;
    int i, j, object_id;
    int ent = 0;
    unsigned int statCount = 0;

    frame = mTombRaider.Frame();
    moveable = mTombRaider.Moveable();
    meshtree = mTombRaider.MeshTree();
    mesh = mTombRaider.Mesh();
    object_texture = mTombRaider.ObjectTextures();
    item = mTombRaider.Item();
    animation = mTombRaider.Animation();
    sprite_sequence = mTombRaider.SpriteSequence();

    printf("Processing skeletal models: ");

    for (i = 0; i < mTombRaider.NumItems(); ++i)
    {
        object_id = item[i].object_id;

        // It may not be a moveable, test for sprite
        if (!(mTombRaider.Engine() == TR_VERSION_1 && item[i].intensity1 == -1))
        {
            for (j = 0; j < (int)mTombRaider.NumSpriteSequences(); ++j)
            {
                if (sprite_sequence[j].object_id == object_id)
                    break;
            }

            // It's not a moveable, skip sprite
            if (j != (int)mTombRaider.NumSpriteSequences())
            {
                //printf("s");
                //fflush(stdout);
                continue;
            }
        }

        for (j = 0; j < (int)mTombRaider.NumMoveables(); ++j)
        {
            if ((int)moveable[j].object_id == object_id)
                break;
        }

        // It's not a moveable or even a sprite?, skip unknown
        if (j == (int)mTombRaider.NumMoveables())
        {
            //printf("?"); // what the wolf?
            //fflush(stdout);
            continue;
        }

        processMoveable(j, i, &ent, cache2, cache, object_id);
        statCount++;
    }

    // Get models that aren't items
    for (i = 0; i < mTombRaider.NumMoveables(); ++i)
    {
        switch ((int)moveable[i].object_id)
        {
            case 30:
            case 2: // Which tr needs this as model again?
                processMoveable(i, i, &ent, cache2, cache,
                        (int)moveable[i].object_id);
                break;
            default:
                switch (mTombRaider.Engine())
                {
                    case TR_VERSION_1:
                        switch ((int)moveable[i].object_id)
                        {
                            case TombRaider1::LaraMutant:
                                processMoveable(i, i, &ent, cache2, cache,
                                        (int)moveable[i].object_id);
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
                                processMoveable(i, i, &ent, cache2, cache,
                                        (int)moveable[i].object_id);
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

    printf("Done! Found %d models.\n", mTombRaider.NumMoveables() + statCount);
}

void Game::processMoveable(int index, int i, int *ent,
        std::vector<skeletal_model_t *> &cache2,
        std::vector<unsigned int> &cache, int object_id)
{
    skeletal_model_t *r_model = NULL;
    skeletal_model_t *c_model = NULL;
    animation_frame_t *animation_frame = NULL;
    tr2_mesh_t *mesh = NULL;
    tr2_moveable_t *moveable = NULL;
    tr2_meshtree_t *meshtree = NULL;
    tr2_item_t *item = NULL;
    tr2_animation_t *animation = NULL;
    tr2_meshtree_t *mesh_tree = NULL;
    bone_frame_t *bone = NULL;
    bone_tag_t *tag = NULL;
    entity_t *thing = NULL;
    SkeletalModel *sModel = 0x0;
    unsigned short *frame;
    int j, k, a, frame_step;
    unsigned int l, frame_offset, frame_count, f;
    float pos[3];
    float yaw;
    bool lara = false;
    int skyMesh;

    skyMesh = mTombRaider.getSkyModelId();
    frame = mTombRaider.Frame();
    moveable = mTombRaider.Moveable();
    meshtree = mTombRaider.MeshTree();
    mesh = mTombRaider.Mesh();
    item = mTombRaider.Item();
    animation = mTombRaider.Animation();

    pos[0] = item[i].x;
    pos[1] = item[i].y;
    pos[2] = item[i].z;

    yaw = ((item[i].angle >> 14) & 0x03);
    yaw *= 90;

    thing = new entity_t;
    thing->id = (*ent)++;
    thing->type = 0x00;
    thing->pos[0] = item[i].x;
    thing->pos[1] = item[i].y;
    thing->pos[2] = item[i].z;
    thing->angles[1] = yaw;
    thing->objectId = moveable[index].object_id;
    thing->moving = false;
    thing->animate = false;

    sModel = new SkeletalModel();
    mRender->addSkeletalModel(sModel);
    thing->tmpHook = sModel; // temp hack to keep a running version during refactoring

    if (mTombRaider.Engine() == TR_VERSION_1)
    {
        switch (thing->objectId)
        {
            case TombRaider1::Wolf:
                thing->state = TombRaider1::WolfState_Lying;
                //thing->animate = true;
                sModel->setAnimation(3);
                sModel->setFrame(0);
                break;
        }
    }

    //! \fixme Check here and see if we already have one for object_id later
    // if (mWorld.isCachedSkeletalModel(moveable[index].object_id))
    // {
    //   thing->modelId = mRender->add(sModel);
    //   return;
    // }

    r_model = new skeletal_model_t;
    r_model->id = moveable[index].object_id;

    // Gather more info if this is lara
    if (moveable[index].object_id == 0)
    {
        gLaraModel = r_model; // hack to avoid broken system until new event sys

        lara = true;
        mCamera->translate(pos[0], pos[1] - 470, pos[2]);
        thing->type = 0x02;
        mLara = thing; // Mongoose 2002.03.22, Cheap hack for now
        mLara->master = 0x0;

        switch (mTombRaider.Engine())
        {
            case TR_VERSION_3:
                mLara->modelId = i;
                sModel->setAnimation(TR_ANIAMTION_RUN);
                sModel->setIdleAnimation(TR_ANIAMTION_STAND);
                r_model->tr4Overlay = false;
                break;
            case TR_VERSION_4:
                mLara->modelId = i;
                sModel->setAnimation(TR_ANIAMTION_RUN);
                sModel->setIdleAnimation(TR_ANIAMTION_STAND);
                // Only TR4 lara has 2 layer bone tags/meshes per bone frame
                r_model->tr4Overlay = true;
                break;
            case TR_VERSION_1:
            case TR_VERSION_2:
            case TR_VERSION_5:
            case TR_VERSION_UNKNOWN:
                mLara->modelId = index;
                sModel->setAnimation(TR_ANIAMTION_RUN);
                sModel->setIdleAnimation(TR_ANIAMTION_STAND);
                r_model->tr4Overlay = false;
                break;
        }

        r_model->ponytailId = 0;
    }
    else
    {
        lara = false;
        r_model->ponytailId = -1;
    }

    // Animation
    a = moveable[index].animation;

    frame_offset = animation[a].frame_offset / 2;
    frame_step = animation[a].frame_size;

    int frame_cycle = 0;

    if (a >= (int)mTombRaider.NumAnimations())
    {
        a = mTombRaider.NumFrames() - frame_offset;
    }
    else
    {
        a = (animation[a].frame_offset / 2) - frame_offset;
    }

    if (frame_step != 0)  // prevent divide-by-zero errors
        a /= frame_step;

    if (a != 0) // prevent divide-by-zero errors
        frame_offset += frame_step * (frame_cycle % a);

    if (a < 0)
    {
        //continue;
        return;
    }

    //! \fixme Might be better UID for each model, but this seems to work well
    j = object_id;

    // We only want one copy of the skeletal model in memory
    unsigned int foundIndex;
    bool found = false;
    for (foundIndex = 0; foundIndex < cache.size(); foundIndex++) {
        if ((int)cache[foundIndex] == j) {
            found = true;
            break;
        }
    }
    if (!found)
    {
        sModel->model = r_model;
        mWorld.addEntity(thing);

        k = mWorld.addModel(r_model);

        cache.push_back(j);
        cache2.push_back(r_model);

        switch (mTombRaider.Engine())
        {
            case TR_VERSION_4:
                if (mLara && moveable[index].object_id == 30)
                {
                    r_model->ponytailId = k;
                    r_model->ponytailMeshId = moveable[index].starting_mesh;
                    r_model->ponytailNumMeshes = ((moveable[index].num_meshes > 0) ?
                            moveable[index].num_meshes : 0);
                    r_model->ponytailAngle = -90.0f;
                    r_model->ponytail[0] = -3;
                    r_model->ponytail[1] = -22;
                    r_model->ponytail[2] = -20;
                    r_model->ponyOff = 40;
                    r_model->ponyOff2 = 32;
                    r_model->pigtails = false;

                    // Try to guess pigtails by looking for certian num verts in head
                    if (mesh[moveable[0].starting_mesh].num_vertices > 80)
                    {
                        r_model->pigtails = true;
                        r_model->ponyOff -= 20;
                        r_model->ponytail[1] -= 32;
                    }

                    mRender->setFlags(Render::fRenderPonytail);
                    gOpenRaider->mConsole->print("Found known ponytail");
                }
                break; // ?
            case TR_VERSION_1:
            case TR_VERSION_2:
            case TR_VERSION_3:
            case TR_VERSION_5:
            case TR_VERSION_UNKNOWN:
                if (mLara && moveable[index].object_id == 2)
                {
                    r_model->ponytailId = k;
                    r_model->ponytailMeshId = moveable[index].starting_mesh;
                    r_model->ponytailNumMeshes = ((moveable[index].num_meshes > 0) ?
                            moveable[index].num_meshes : 0);
                    r_model->ponytailAngle = -90.0f;
                    r_model->ponytail[0] = 0;
                    r_model->ponytail[1] = -20;
                    r_model->ponytail[2] = -20;
                    r_model->ponyOff = 40;
                    r_model->ponyOff2 = 0;

                    mRender->setFlags(Render::fRenderPonytail);
                    gOpenRaider->mConsole->print("Found ponytail?");
                }
                break;
        }
    }
    else
    {
        // Already cached
        delete r_model;
        c_model = cache2[foundIndex];
        sModel->model = c_model;
        mWorld.addEntity(thing);
        mWorld.addModel(c_model);
        printf("c");
        return;
    }

    int aloop = mTombRaider.getNumAnimsForMoveable(index);

#ifdef DEBUG
    if (mFlags & Flag_DebugModel)
    {
        printf("\nanimation = %i, num_animations = %i\n",
                moveable[index].animation, aloop);
        printf("\nitem[%i].flags = %i\nentity[%i]\n",
                i, item[i].flags, thing->id);
    }
#endif

    //a = moveable[index].animation;
    //frame_offset = animation[a].frame_offset / 2;
    //frame_step = animation[a].frame_size;

    for (; a < aloop; ++a,
            frame_offset = animation[a].frame_offset / 2,
            frame_step = animation[a].frame_size)
    {
        animation_frame = new animation_frame_t;
        r_model->animation.push_back(animation_frame);

        frame_count = (animation[a].frame_end - animation[a].frame_start) + 1;
        animation_frame->rate = animation[a].frame_rate;

#ifdef DEBUG
        if (mFlags & Flag_DebugModel)
        {
            printf("animation[%i] state and unknowns = %i, %i, %i, %i, %i\n",
                    a, animation[a].state_id, animation[a].unknown1,
                    animation[a].unknown2, animation[a].unknown3,
                    animation[a].unknown4);
            printf("animation[%i].frame_rate = %i\n",
                    a, animation[a].frame_rate);
            printf("animation[%i].next_animation = %i\n",
                    a, animation[a].next_animation);
            printf("animation[%i].frame_offset = %u\n",
                    a, animation[a].frame_offset);
            printf("animation[%i].anim_command = %i\n",
                    a, animation[a].anim_command);
            printf("animation[%i].num_anim_commands = %i\n",
                    a, animation[a].num_anim_commands);
            printf("animation[%i].state_change_offset = %i\n",
                    a, animation[a].state_change_offset);
            printf("              frame_offset = %u\n",
                    frame_offset);
        }
#endif

        // Get all the frames for aniamtion
        for (f = 0; f < frame_count; ++f, frame_offset += frame_step)
        {
            // HACK: Lara's ObjectID is 315, but her meshes start at 0, so make a
            // quick substitution (so she doesn't appear as a bunch of thighs)
            if (index == 0 && mTombRaider.Engine() == TR_VERSION_3)
            {
                for (j = 0; j < (int)mTombRaider.NumMoveables() && !index; ++j)
                {
                    if (moveable[j].object_id == 315)
                        index = j;
                }
            }

            // Fix Lara in TR4
            if (index == 0 && mTombRaider.Engine() == TR_VERSION_4)
            {
                for (j = 0; j < (int)mTombRaider.NumMoveables() && !index; ++j)
                {
                    // Body is ItemID 8, joints are ItemID 9
                    //  (TR4 demo: body is ItemID 10, joints are ItemID 11)
                    if (moveable[j].object_id == 8)
                        index = j;
                }
            }
            else if (moveable[index].object_id == 8 &&
                    mTombRaider.Engine() == TR_VERSION_4)
            {
                // KLUDGE to do "skinning"
                index = 0;

                for (j = 0; j < (int)mTombRaider.NumMoveables() && !index; ++j)
                {
                    // Body is ItemID 8, joints are ItemID 9
                    //  (TR4 demo: body is ItemID 10, joints are ItemID 11)
                    if (moveable[j].object_id == 9)
                        index = j;
                }
            }

#ifdef DEBUG
            if (mFlags & Flag_DebugModel)
            {
                printf("animation[%i].boneframe[%u] = offset %u, step %i\n",
                        a, f, frame_offset, frame_step);
            }
#endif

            // Mongoose 2002.08.15, Was
            //   if (frame_offset + 8 > _tombraider.NumFrames())
            if (frame_offset > mTombRaider.NumFrames())
            {
                gOpenRaider->mConsole->print("WARNING: Bad animation frame %i > %i",
                        frame_offset, mTombRaider.NumFrames());

                // Mongoose 2002.08.15, Attempt to skip more likely bad animation data
                gOpenRaider->mConsole->print("WARNING: Handling bad animation data...");
                return; //continue;
            }

            // Generate bone frames and tags per frame ////////////
            bone = new bone_frame_t;
            animation_frame->frame.push_back(bone);

            // Init translate for bone frame
            bone->pos[0] = (short)frame[frame_offset + 6];
            bone->pos[1] = (short)frame[frame_offset + 7];
            bone->pos[2] = (short)frame[frame_offset + 8];
            bone->yaw = yaw;

            //printf("%f %f %f\n", bone->pos[0], bone->pos[1], bone->pos[2]);

            l = 9;   // First angle offset in this Frame

            // Run through the tag and calculate the rotation and offset
            for (j = 0; j < (int)moveable[index].num_meshes; ++j)
            {
                tag = new bone_tag_t;
                bone->tag.push_back(tag);
                tag->off[0] = 0.0;
                tag->off[1] = 0.0;
                tag->off[2] = 0.0;
                tag->flag = 0x00;
                tag->rot[0] = 0.0;
                tag->rot[1] = 0.0;
                tag->rot[2] = 0.0;
                tag->mesh = moveable[index].starting_mesh + j;

                // Setup offsets to produce skeletion
                if (j == 0)
                {
                    // Since we use bone's offset, these aren't used
                    tag->off[0] = 0.0;
                    tag->off[1] = 0.0;
                    tag->off[2] = 0.0;

                    // Always push tag[0], this isn't really used either
                    tag->flag = 0x02;
                }
                else  // Nonprimary tag - position relative to first tag
                {
                    int *tree;
                    // Hack: moveable[index].mesh_tree is a byte offset
                    //       into mesh_tree[], so we have to convert to index
                    tree = (int *)(void *)meshtree;
                    mesh_tree = (tr2_meshtree_t *)&tree[moveable[index].mesh_tree
                        + ((j - 1) * 4)];

                    tag->off[0] = mesh_tree->x;
                    tag->off[1] = mesh_tree->y;
                    tag->off[2] = mesh_tree->z;
                    tag->flag = (char)mesh_tree->flags;
                }

                // Setup tag rotations
                mTombRaider.computeRotationAngles(&frame, &frame_offset, &l,
                        tag->rot, tag->rot+1, tag->rot+2);
            }
        }
    }

    if (i == skyMesh)
    {
        mRender->setSkyMesh(i, //moveable[i].starting_mesh,
                (mTombRaider.Engine() == TR_VERSION_2));
    }

    //printf(".");
    //fflush(stdout);
}

bool compareFaceTextureId(const void *voidA, const void *voidB)
{
    texture_tri_t *a = (texture_tri_t *)voidA, *b = (texture_tri_t *)voidB;

    if (!a || !b)
        return false; // error really

    return (a->texture < b->texture);
}

#ifdef EXPERIMENTAL
void Game::setupTextureColor(texture_tri_t *r_tri, float *colorf)
{
    unsigned char color[4];
    unsigned int colorI;

    color[0] = (unsigned char)(colorf[0]*255.0f);
    color[1] = (unsigned char)(colorf[1]*255.0f);
    color[2] = (unsigned char)(colorf[2]*255.0f);
    color[3] = (unsigned char)(colorf[3]*255.0f);

    ((unsigned char *)(&colorI))[3] = color[0];
    ((unsigned char *)(&colorI))[2] = color[1];
    ((unsigned char *)(&colorI))[1] = color[2];
    ((unsigned char *)(&colorI))[0] = color[3];

    bool found = false;
    unsigned int foundIndex = 0;
    for (foundIndex = 0; foundIndex < gColorTextureHACK.size(); foundIndex++) {
        if (gColorTextureHACK[foundIndex] == colorI) {
            found = true;
            break;
        }
    }
    if (!found)
    {
        gColorTextureHACK.push_back(colorI);
        r_tri->texture = mTextureOffset + gColorTextureHACK.size();

        mRender->loadTexture(Texture::generateColorTexture(color, 32, 32),
                32, 32,
                r_tri->texture);

#ifdef DEBUG_COLOR_TEXTURE_GEN
        printf("Color 0x%02x%02x%02x%02x | 0x%08xto texture[%u]?\n",
                color[0], color[1], color[2], color[3], colorI,
                gColorTextureHACK.size());
#endif
    }
    else
    {
        //printf("Color already loaded %i -> 0x%08x\n",
        //       gColorTextureHACK.getCurrentIndex(),
        //       gColorTextureHACK.current());

        r_tri->texture = mTextureOffset + foundIndex;
    }

    //r_tri->texture = white; // White texture
}
#endif

void Game::processModels()
{
    printf("Processing meshes: ");
    for (int index = 0; index < mTombRaider.getMeshCount(); index++) {
        int i, j, count, texture;
        int vertexIndices[6];
        float st[12];
        float color[4];
        unsigned short transparency;
        texture_tri_t *r_tri;

        // Assert common sense
        if (index < 0 || !mTombRaider.isMeshValid(index))
        {
            //! \fixme allow sparse lists with matching ids instead?
            mWorld.addMesh(NULL); // Filler, to make meshes array ids align
            //printf("x");
            //fflush(stdout);
            return;
        }

#ifndef EXPERIMENTAL
        // WHITE texture id
        int white = 0;
#endif

        model_mesh_t *mesh = new model_mesh_t;

        // Mongoose 2002.08.30, Testing support for 'shootable' models ( traceable )
        mTombRaider.getMeshCollisionInfo(index, mesh->center, &mesh->radius);

        //! \fixme Arrays don't work either  =)
        // Mesh geometery, colors, etc
        mTombRaider.getMeshVertexArrays(index,
                &mesh->vertexCount, &mesh->vertices,
                &mesh->normalCount, &mesh->normals,
                &mesh->colorCount,  &mesh->colors);

        // Textured Triangles
        count = mTombRaider.getMeshTexturedTriangleCount(index);
        mesh->texturedTriangles.reserve(count); // little faster

        for (i = 0; i < count; ++i)
        {
            r_tri = new texture_tri_t;

            mTombRaider.getMeshTexturedTriangle(index, i,
                    r_tri->index,
                    r_tri->st,
                    &r_tri->texture,
                    &r_tri->transparency);

            r_tri->texture += mTextureStart;

            // Add to face vector
            mesh->texturedTriangles.push_back(r_tri);
        }

        // Coloured Triangles
        count = mTombRaider.getMeshColoredTriangleCount(index);
        mesh->coloredTriangles.reserve(count); // little faster

        for (i = 0; i < count; i++)
        {
            r_tri = new texture_tri_t;

            mTombRaider.getMeshColoredTriangle(index, i,
                    r_tri->index,
                    color);
            r_tri->st[0] = color[0];
            r_tri->st[1] = color[1];
            r_tri->st[2] = color[2];
            r_tri->st[3] = color[3];
            r_tri->st[4] = 1.0;
            r_tri->st[5] = 1.0;

#ifdef EXPERIMENTAL
            setupTextureColor(r_tri, color);
#else
            r_tri->texture = white; // White texture
#endif
            r_tri->transparency = 0;

            // Add to face vector
            mesh->coloredTriangles.push_back(r_tri);
        }

        // Textured Rectangles
        count = mTombRaider.getMeshTexturedRectangleCount(index);
        mesh->texturedRectangles.reserve(count*2); // little faster

        for (i = 0; i < count; ++i)
        {
            mTombRaider.getMeshTexturedRectangle(index, i,
                    vertexIndices,
                    st,
                    &texture,
                    &transparency);

            r_tri = new texture_tri_t;

            for (j = 0; j < 3; ++j)
                r_tri->index[j] = vertexIndices[j];

            for (j = 0; j < 6; ++j)
                r_tri->st[j] = st[j];

            r_tri->texture = texture + mTextureStart;
            r_tri->transparency = transparency;

            // Add to face vector
            mesh->texturedRectangles.push_back(r_tri);

            r_tri = new texture_tri_t;

            for (j = 3; j < 6; ++j)
                r_tri->index[j-3] = vertexIndices[j];

            for (j = 6; j < 12; ++j)
                r_tri->st[j-6] = st[j];

            r_tri->texture = texture + mTextureStart;
            r_tri->transparency = transparency;

            // Add to face vector
            mesh->texturedRectangles.push_back(r_tri);
        }

        // Coloured Rectangles
        count = mTombRaider.getMeshColoredRectangleCount(index);
        mesh->coloredRectangles.reserve(count*2); // little faster

        for (i = 0; i < count; ++i)
        {
            mTombRaider.getMeshColoredRectangle(index, i,
                    vertexIndices,
                    color);

            r_tri = new texture_tri_t;

            for (j = 0; j < 3; ++j)
                r_tri->index[j] = vertexIndices[j];

            //for (j = 0; j < 6; ++j)
            //  r_tri->st[j] = st[j];

            r_tri->st[0] = color[0];
            r_tri->st[1] = color[1];
            r_tri->st[2] = color[2];
            r_tri->st[3] = color[3];
            r_tri->st[4] = 1.0;
            r_tri->st[5] = 1.0;

#ifdef EXPERIMENTAL
            //for (j = 6; j < 12; ++j)
            //  r_tri->st[j-6] = st[j];
            setupTextureColor(r_tri, color);
#else
            r_tri->texture = white; // White texture
#endif
            r_tri->transparency = 0;

            // Add to face vector
            mesh->coloredRectangles.push_back(r_tri);

            r_tri = new texture_tri_t;

            for (j = 3; j < 6; ++j)
                r_tri->index[j-3] = vertexIndices[j];

            //for (j = 6; j < 12; ++j)
            //  r_tri->st[j-6] = st[j];

            r_tri->st[0] = color[0];
            r_tri->st[1] = color[1];
            r_tri->st[2] = color[2];
            r_tri->st[3] = color[3];
            r_tri->st[4] = 1.0;
            r_tri->st[5] = 1.0;

#ifdef EXPERIMENTAL
            setupTextureColor(r_tri, color);
#else
            r_tri->texture = white; // White texture
#endif
            r_tri->transparency = 0;

            // Add to face vector
            mesh->coloredRectangles.push_back(r_tri);
        }

        // Sort faces by texture
        std::sort(mesh->texturedTriangles.begin(), mesh->texturedTriangles.end(), compareFaceTextureId);
        std::sort(mesh->coloredTriangles.begin(), mesh->coloredTriangles.end(), compareFaceTextureId);
        std::sort(mesh->texturedRectangles.begin(), mesh->texturedRectangles.end(), compareFaceTextureId);
        std::sort(mesh->coloredRectangles.begin(), mesh->coloredRectangles.end(), compareFaceTextureId);

        mWorld.addMesh(mesh);
        //printf(".");
        //fflush(stdout);
    }

    printf("Done! Found %d meshes.\n", mTombRaider.getMeshCount());
}

void Game::processRooms()
{
    printf("Processing rooms: ");
    for (int index = 0; index < mTombRaider.NumRooms(); index++) {
        unsigned int i, j, count;
        room_mesh_t *r_mesh = NULL;
        RenderRoom *rRoom = NULL;
        Matrix transform;

        if (!mTombRaider.isRoomValid(index))
        {
            gOpenRaider->mConsole->print("WARNING: Handling invalid vertex array in room");
            mWorld.addRoom(0x0);
            mRender->addRoom(0x0);

            //printf("x");
            //fflush(stdout);
            return;
        }

        rRoom = new RenderRoom();
        r_mesh = new room_mesh_t;
        r_mesh->id = index;

        mTombRaider.getRoomInfo(index, &r_mesh->flags, r_mesh->pos,
                r_mesh->bbox_min, r_mesh->bbox_max);

        // Adjust positioning for OR world coord translation
        r_mesh->bbox_min[0] += r_mesh->pos[0];
        r_mesh->bbox_max[0] += r_mesh->pos[0];
        r_mesh->bbox_min[2] += r_mesh->pos[2];
        r_mesh->bbox_max[2] += r_mesh->pos[2];

        // Mongoose 2002.04.03, Setup 3d transform
        transform.setIdentity();
        transform.translate(r_mesh->pos);

        // Setup portals
        float portalVertices[12];
        count = mTombRaider.getRoomPortalCount(index);

        //! \fixme OR wrongly uses a cached adj room list for rendering vis
        r_mesh->adjacentRooms.reserve(count + 1);

        // Current room is always first
        r_mesh->adjacentRooms.push_back(index);

        for (i = 0; i < count; ++i)
        {
            portal_t *portal = new portal_t;

            mTombRaider.getRoomPortal(index, i,
                    &portal->adjoining_room, portal->normal,
                    portalVertices);

            for (j = 0; j < 4; ++j)
            {
                portal->vertices[j][0] = portalVertices[j*3];
                portal->vertices[j][1] = portalVertices[j*3+1];
                portal->vertices[j][2] = portalVertices[j*3+2];

                // Relative coors in vis portals
                transform.multiply3v(portal->vertices[j], portal->vertices[j]);
            }

            r_mesh->adjacentRooms.push_back(portal->adjoining_room);
            r_mesh->portals.push_back(portal);
        }

        // Physics/gameplay use /////////////////////////////

        //! \fixme Use more of sector structure, boxes, and floordata

        // List of sectors in this room
        unsigned int sectorFlags;
        int floorDataIndex, boxIndex, roomBelow, roomAbove;
        count = mTombRaider.getRoomSectorCount(index, &r_mesh->numZSectors,
                &r_mesh->numXSectors);
        r_mesh->sectors.reserve(count);

        for (i = 0; i < count; ++i)
        {
            sector_t *sector = new sector_t;

            mTombRaider.getRoomSector(index, i, &sectorFlags,
                    &sector->ceiling, &sector->floor,
                    &floorDataIndex, &boxIndex, &roomBelow,
                    &roomAbove);

            if (sectorFlags & tombraiderSector_wall)
            {
                sector->wall = true;
            }
            else
            {
                sector->wall = false;
            }

            r_mesh->sectors.push_back(sector);
        }

        // Setup collision boxes ( Should use sectors, but this is a test )
        count = mTombRaider.getRoomBoxCount(index);
        r_mesh->boxes.reserve(count);

        //! fixme Only to be done only on room[0]?  I don't think so...
        for (i = 0; !index && i < count; ++i)
        {
            box_t *box = new box_t;

            mTombRaider.getRoomBox(index, i,
                    box->a.pos, box->b.pos, box->c.pos, box->d.pos);

            r_mesh->boxes.push_back(box);
        }

        // Setup room lights /////////////////////////////////////
        unsigned int lightFlags, lightType;
        count = mTombRaider.getRoomLightCount(index);
        rRoom->lights.reserve(count);

        for (i = 0; i < count; ++i)
        {
            Light *light = new Light();

            mTombRaider.getRoomLight(index, i,
                    light->mPos, light->mColor, light->mDir,
                    &light->mAtt, &light->mCutoff,
                    &lightType, &lightFlags);

            switch (lightType)
            {
                case tombraiderLight_typeDirectional:
                    light->mType = Light::typeDirectional;
                    break;
                case tombraiderLight_typeSpot:
                    light->mType = Light::typeSpot;
                    break;
                case tombraiderLight_typePoint:
                default:
                    light->mType = Light::typePoint;
            }

            rRoom->lights.push_back(light);
        }

        // Room geometery //////////////////////////////////

        //#define EXPERIMENTAL_UNFIFIED_ROOM_GEOMETERY
#ifdef EXPERIMENTAL_UNFIFIED_ROOM_GEOMETERY
        unsigned int vertexCount, normalCount, colorCount, triCount;
        vec_t *vertexArray;
        vec_t *normalArray;
        vec_t *colorArray;
        unsigned int *indices, *flags;
        float *texCoords;
        int *textures;

        mTombRaider.getRoomVertexArrays(index,
                &vertexCount, &vertexArray,
                &normalCount, &normalArray,
                &colorCount, &colorArray);

        rRoom->mesh.bufferVertexArray(vertexCount, (vec_t *)vertexArray);
        rRoom->mesh.bufferNormalArray(normalCount, (vec_t *)normalArray);
        rRoom->mesh.bufferColorArray(vertexCount, (vec_t *)colorArray);

        mTombRaider.getRoomTriangles(index, mTextureStart,
                &triCount, &indices, &texCoords, &textures,
                &flags);

        rRoom->mesh.bufferTriangles(triCount, indices, texCoords, textures, flags);
#else
        float rgba[4];
        float xyz[3];

        count = mTombRaider.getRoomVertexCount(index);
        rRoom->mesh.allocateVertices(count);
        rRoom->mesh.allocateNormals(0); // count
        rRoom->mesh.allocateColors(count);

        for (i = 0; i < count; ++i)
        {
            mTombRaider.getRoomVertex(index, i, xyz, rgba);

            rRoom->mesh.setVertex(i, xyz[0], xyz[1], xyz[2]);
            rRoom->mesh.setColor(i, rgba);
        }

        // Mongoose 2002.06.09, Setup allocation of meshes and polygons
        // Counters ( Textured polygon lists are allocated per texture)
        //          ( Textures are mapped to these meshes )
        int triangle_counter[TextureLimit];
        int triangle_counter_alpha[TextureLimit];
        int rectangle_counter[TextureLimit];
        int rectangle_counter_alpha[TextureLimit];
        int tris_mesh_map[TextureLimit];
        int rect_mesh_map[TextureLimit];

        for (i = 0; i < TextureLimit; ++i)
        {
            triangle_counter[i]        = 0;
            triangle_counter_alpha[i]  = 0;
            rectangle_counter[i]       = 0;
            rectangle_counter_alpha[i] = 0;

            tris_mesh_map[i] = -1;
            rect_mesh_map[i] = -1;
        }

        unsigned int numTris = 0;
        unsigned int numQuads = 0;

        int texture;
        unsigned int r, t, q, v, flags;
        unsigned int indices[4];
        float texCoords[8];

        count = mTombRaider.getRoomTriangleCount(index);

        // Mongoose 2002.08.15, Presort by alpha and texture and setup mapping
        for (t = 0; t < count; ++t)
        {
            mTombRaider.getRoomTriangle(index, t,
                    indices, texCoords, &texture, &flags);

            texture += mTextureStart;

            if (texture > (int)TextureLimit)
            {
                gOpenRaider->mConsole->print("Handling bad room[%i].tris[%i].texture = %i",
                        index, t, texture);
                texture = TextureLimit - 1;
            }

            // Counters set up polygon allocation
            if (flags & tombraiderFace_Alpha ||
                    flags & tombraiderFace_PartialAlpha)
            {
                triangle_counter_alpha[texture] += 1;
            }
            else
            {
                triangle_counter[texture] += 1;
            }

            // Counter sets up texture id to mesh id mapping
            if (tris_mesh_map[texture] == -1)
            {
                tris_mesh_map[texture] = ++numTris;
            }
        }

        count = mTombRaider.getRoomRectangleCount(index);

        for (r = 0; r < count; ++r)
        {
            mTombRaider.getRoomRectangle(index, r,
                    indices, texCoords, &texture, &flags);

            texture += mTextureStart;

            if (texture > (int)TextureLimit)
            {
                gOpenRaider->mConsole->print("Handling bad room[%i].quad[%i].texture = %i",
                        index, r, texture);
                texture = TextureLimit - 1;
            }

            if (flags & tombraiderFace_Alpha ||
                    flags & tombraiderFace_PartialAlpha)
            {
                rectangle_counter_alpha[texture] += 1;
            }
            else
            {
                rectangle_counter[texture] += 1;
            }

            if (rect_mesh_map[texture] == -1)
            {
                rect_mesh_map[texture] = ++numQuads;
            }
        }

        // Allocate indexed polygon meshes
        rRoom->mesh.allocateTriangles(numTris);
        rRoom->mesh.allocateRectangles(numQuads);

        for (i = 0, j = 0; i < TextureLimit; ++i)
        {
            if (tris_mesh_map[i] > 0)
            {
                j = tris_mesh_map[i] - 1;

                t = triangle_counter[i];

                rRoom->mesh.mTris[j].texture = i;
#ifdef MULTITEXTURE
                rRoom->mesh.mTris[j].bumpmap = gMapTex2Bump[i];
#endif
                rRoom->mesh.mTris[j].cnum_triangles = 0;
                rRoom->mesh.mTris[j].num_triangles = 0;
                rRoom->mesh.mTris[j].cnum_alpha_triangles = 0;
                rRoom->mesh.mTris[j].num_alpha_triangles = 0;
                rRoom->mesh.mTris[j].triangles = 0x0;
                rRoom->mesh.mTris[j].alpha_triangles = 0x0;
                rRoom->mesh.mTris[j].texcoors = 0x0;
                rRoom->mesh.mTris[j].texcoors2 = 0x0;

                if (t > 0)
                {
                    rRoom->mesh.mTris[j].num_triangles = t;
                    rRoom->mesh.mTris[j].triangles = new unsigned int[t*3];
                    rRoom->mesh.mTris[j].num_texcoors = t * 3;
                    rRoom->mesh.mTris[j].texcoors = new vec2_t[t * 3];
                }

                t = triangle_counter_alpha[i];

                if (t > 0)
                {
                    rRoom->mesh.mTris[j].num_alpha_triangles = t;
                    rRoom->mesh.mTris[j].alpha_triangles = new unsigned int[t*3];
                    rRoom->mesh.mTris[j].num_texcoors2 = t * 3;
                    rRoom->mesh.mTris[j].texcoors2 = new vec2_t[t * 3];
                }
            }

            ///////////////////////////////////////////

            if (rect_mesh_map[i] > 0)
            {
                j = rect_mesh_map[i] - 1;

                r = rectangle_counter[i];

                rRoom->mesh.mQuads[j].texture = i;
#ifdef MULTITEXTURE
                rRoom->mesh.mQuads[j].bumpmap = gMapTex2Bump[i];
#endif
                rRoom->mesh.mQuads[j].cnum_quads = 0;
                rRoom->mesh.mQuads[j].num_quads = 0;
                rRoom->mesh.mQuads[j].cnum_alpha_quads = 0;
                rRoom->mesh.mQuads[j].num_alpha_quads = 0;
                rRoom->mesh.mQuads[j].quads = 0x0;
                rRoom->mesh.mQuads[j].alpha_quads = 0x0;
                rRoom->mesh.mQuads[j].texcoors = 0x0;
                rRoom->mesh.mQuads[j].texcoors2 = 0x0;

                if (r > 0)
                {
                    rRoom->mesh.mQuads[j].num_quads = r;
                    rRoom->mesh.mQuads[j].quads = new unsigned int[r*4];
                    rRoom->mesh.mQuads[j].num_texcoors = r * 4;
                    rRoom->mesh.mQuads[j].texcoors = new vec2_t[r * 4];
                }

                r = rectangle_counter_alpha[i];

                if (r > 0)
                {
                    rRoom->mesh.mQuads[j].num_alpha_quads = r;
                    rRoom->mesh.mQuads[j].alpha_quads = new unsigned int[r*4];
                    rRoom->mesh.mQuads[j].num_texcoors2 = r * 4;
                    rRoom->mesh.mQuads[j].texcoors2 = new vec2_t[r * 4];
                }
            }
        }

        // Generate textured triangles
        count = mTombRaider.getRoomTriangleCount(index);

        for (t = 0; t < count; ++t)
        {
            mTombRaider.getRoomTriangle(index, t,
                    indices, texCoords, &texture, &flags);

            // Adjust texture id using mTextureStart to map into
            // correct textures
            texture += mTextureStart;

            j = tris_mesh_map[texture] - 1;

            // Setup per vertex
            for (i = 0; i < 3; ++i)
            {
                // Get vertex index {(0, a), (1, b), (2, c)}
                v = indices[i];

                if ((flags & tombraiderFace_Alpha ||
                            flags & tombraiderFace_PartialAlpha) &&
                        rRoom->mesh.mTris[j].num_alpha_triangles > 0)
                {
                    q = rRoom->mesh.mTris[j].cnum_alpha_triangles*3+i;

                    rRoom->mesh.mTris[j].alpha_triangles[q] = v;

                    rRoom->mesh.mTris[j].texcoors2[q][0] = texCoords[i*2];
                    rRoom->mesh.mTris[j].texcoors2[q][1] = texCoords[i*2+1];
                }
                else if (rRoom->mesh.mTris[j].num_triangles > 0)
                {
                    q = rRoom->mesh.mTris[j].cnum_triangles*3+i;

                    rRoom->mesh.mTris[j].triangles[q] = v;

                    rRoom->mesh.mTris[j].texcoors[q][0] = texCoords[i*2];
                    rRoom->mesh.mTris[j].texcoors[q][1] = texCoords[i*2+1];
                }

                // Partial alpha hack
                if (flags & tombraiderFace_PartialAlpha)
                {
                    //rRoom->mesh.colors[v].rgba[3] = 0.45;
                }
            }

            if (flags & tombraiderFace_Alpha ||
                    flags & tombraiderFace_PartialAlpha)
            {
                rRoom->mesh.mTris[j].cnum_alpha_triangles++;
            }
            else
            {
                rRoom->mesh.mTris[j].cnum_triangles++;
            }
        }

        // Generate textured quads
        count = mTombRaider.getRoomRectangleCount(index);

        for (r = 0; r < count; ++r)
        {
            mTombRaider.getRoomRectangle(index, r,
                    indices, texCoords, &texture, &flags);

            // Adjust texture id using mTextureStart to map into
            // correct textures
            texture += mTextureStart;

            if (texture > (int)TextureLimit)
            {
                texture = TextureLimit - 1;
            }

            j = rect_mesh_map[texture] - 1;

            if (rRoom->mesh.mQuads[j].num_quads <= 0 &&
                    rRoom->mesh.mQuads[j].num_alpha_quads <= 0)
                continue;

            // Setup per vertex
            for (i = 0; i < 4; ++i)
            {
                // Get vertex index {(0, a), (1, b), (2, c), (3, d)}
                v = indices[i];

                if ((flags & tombraiderFace_Alpha ||
                            flags & tombraiderFace_PartialAlpha) &&
                        rRoom->mesh.mQuads[j].num_alpha_quads > 0)
                {
                    q = rRoom->mesh.mQuads[j].cnum_alpha_quads*4+i;

                    rRoom->mesh.mQuads[j].alpha_quads[q] = v;

                    rRoom->mesh.mQuads[j].texcoors2[q][0] = texCoords[i*2];
                    rRoom->mesh.mQuads[j].texcoors2[q][1] = texCoords[i*2+1];
                }
                else if (rRoom->mesh.mQuads[j].num_quads > 0)
                {
                    q = rRoom->mesh.mQuads[j].cnum_quads*4+i;

                    rRoom->mesh.mQuads[j].quads[q] = v;

                    rRoom->mesh.mQuads[j].texcoors[q][0] = texCoords[i*2];
                    rRoom->mesh.mQuads[j].texcoors[q][1] = texCoords[i*2+1];
                }

                // Partial alpha hack
                if (flags & tombraiderFace_PartialAlpha)
                {
                    //rRoom->mesh.colors[v].rgba[3] = 0.45;
                }
            }

            if (flags & tombraiderFace_Alpha ||
                    flags & tombraiderFace_PartialAlpha)
            {
                rRoom->mesh.mQuads[j].cnum_alpha_quads++;
            }
            else
            {
                rRoom->mesh.mQuads[j].cnum_quads++;
            }
        }
#endif

        // Room models
        count = mTombRaider.getRoomModelCount(index);
        r_mesh->models.reserve(count);

        for (i = 0; i < count; ++i)
        {
            static_model_t *model = new static_model_t;

            mTombRaider.getRoomModel(index, i,
                    &model->index, model->pos, &model->yaw);

            r_mesh->models.push_back(model);
        }

        // Room sprites
        float spriteVertices[12];
        float spriteTexCoords[8];
        count = mTombRaider.getRoomSpriteCount(index);
        r_mesh->sprites.reserve(count);

        for (i = 0; i < count; ++i)
        {
            sprite_t *sprite = new sprite_t;

            mTombRaider.getRoomSprite(index, i,
                    10.0f, &sprite->texture, sprite->pos,
                    spriteVertices, spriteTexCoords);

            sprite->texture += mTextureStart; // OpenRaider preloads some textures

            sprite->vertex[0].pos[0] = spriteVertices[0];
            sprite->vertex[0].pos[1] = spriteVertices[1];
            sprite->vertex[0].pos[2] = spriteVertices[2];
            sprite->vertex[1].pos[0] = spriteVertices[3];
            sprite->vertex[1].pos[1] = spriteVertices[4];
            sprite->vertex[1].pos[2] = spriteVertices[5];
            sprite->vertex[2].pos[0] = spriteVertices[6];
            sprite->vertex[2].pos[1] = spriteVertices[7];
            sprite->vertex[2].pos[2] = spriteVertices[8];
            sprite->vertex[3].pos[0] = spriteVertices[9];
            sprite->vertex[3].pos[1] = spriteVertices[10];
            sprite->vertex[3].pos[2] = spriteVertices[11];

            sprite->texel[0].st[0] = spriteTexCoords[0];
            sprite->texel[0].st[1] = spriteTexCoords[1];
            sprite->texel[1].st[0] = spriteTexCoords[2];
            sprite->texel[1].st[1] = spriteTexCoords[3];
            sprite->texel[2].st[0] = spriteTexCoords[4];
            sprite->texel[2].st[1] = spriteTexCoords[5];
            sprite->texel[3].st[0] = spriteTexCoords[6];
            sprite->texel[3].st[1] = spriteTexCoords[7];

            r_mesh->sprites.push_back(sprite);
        }

        mWorld.addRoom(r_mesh);

        rRoom->room = r_mesh;
        mRender->addRoom(rRoom);

        //printf(".");
        //fflush(stdout);
    }

    printf("Done! Found %d rooms.\n", mTombRaider.NumRooms());
}

