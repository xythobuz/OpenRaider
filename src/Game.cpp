/*!
 * \file src/Game.cpp
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#include <algorithm>
#include <map>
#include <cstdlib>

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "OpenRaider.h"
#include "Sound.h"
#include "utils/strings.h"

#include "games/TombRaider1.h"

#ifdef EXPERIMENTAL
std::vector<unsigned int> gColorTextureHACK;
#endif

#ifdef MULTITEXTURE
std::map<int, int> gMapTex2Bump;
#endif

Game::Game() {
    mLoaded = false;
    mName = NULL;
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
    mTextureStart = getRender().initTextures(getOpenRaider().mDataDir);
    getRender().setMode(Render::modeLoadScreen);

    return 0;
}

void Game::destroy() {
    if (mName)
        delete [] mName;
    mName = NULL;
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

    mName = bufferString("%s", level);

    // Load the level pak into TombRaider
    getConsole().print("Loading %s", mName);
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
            getConsole().print("Could not load %s", tmp);
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
    if (mLara == -1) {
        getConsole().print("Can't find Lara entity in level pak!");
        return -1;
    }

    mLoaded = true;
    getRender().setMode(Render::modeVertexLight);

    return 0;
}

void Game::handleAction(ActionEvents action, bool isFinished) {
    if (mLoaded) {
        if (action == forwardAction) {
            getLara().move('f');
        } else if (action == backwardAction) {
            getLara().move('b');
        } else if (action == leftAction) {
            getLara().move('l');
        } else if (action == rightAction) {
            getLara().move('r');
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
        getLara().setAngles(getCamera().getRadianYaw(), getCamera().getRadianPitch());
    }
}

Entity &Game::getLara() {
    assert(mLara >= 0);
    assert(mLara < (int)getWorld().sizeEntity());
    return getWorld().getEntity(mLara);
}

void Game::processSprites() {
    printf("Processing sprites: ");
    for (int i = 0; i < (mTombRaider.NumItems() - 1); i++) {
        if ((mTombRaider.Engine() == TR_VERSION_1) && (mTombRaider.Item()[i].intensity1 == -1))
            continue;

        for (int j = 0; j < mTombRaider.NumSpriteSequences(); j++) {
            if (mTombRaider.SpriteSequence()[j].object_id == mTombRaider.Item()[i].object_id)
                getWorld().addSprite(*new SpriteSequence(mTombRaider, i, j));
        }
    }
    printf("Done! Found %d sprites.\n", mTombRaider.NumSpriteSequences());
}

void Game::processRooms() {
    printf("Processing rooms: ");
    for (int index = 0; index < mTombRaider.NumRooms(); index++)
        getWorld().addRoom(*new Room(mTombRaider, index));
    printf("Done! Found %d rooms.\n", mTombRaider.NumRooms());
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
        getRender().loadTexture(image, 256, 256, (mTextureStart - 1) + i);

#ifdef MULTITEXTURE
        gMapTex2Bump[(mTextureStart - 1) + i] = -1;
#endif

        if (bumpmap)
        {
#ifdef MULTITEXTURE
            gMapTex2Bump[(mTextureStart - 1) + i] = (mTextureStart - 1) + i +
                    mTombRaider.NumTextures();
#endif
            getRender().loadTexture(bumpmap, 256, 256, (mTextureStart - 1) + i +
                    mTombRaider.NumTextures());
        }

        if (image)
            delete [] image;

        if (bumpmap)
            delete [] bumpmap;

        //printf(".");
        //fflush(stdout);
    }

    mTextureOffset = (mTextureStart - 1) + mTombRaider.NumTextures();

    printf("Done! Found %d textures.\n", mTombRaider.NumTextures());
}

void Game::processMoveables()
{
    unsigned int statCount = 0;

    tr2_moveable_t *moveable = mTombRaider.Moveable();
    tr2_item_t *item = mTombRaider.Item();
    tr2_sprite_sequence_t *sprite_sequence = mTombRaider.SpriteSequence();

    printf("Processing skeletal models: ");

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

    printf("Done! Found %d models.\n", mTombRaider.NumMoveables() + statCount);
}

// index moveable, i item, sometimes both moveable
// object_id of item or moveable
void Game::processMoveable(int index, int i, int object_id) {
    bool cached = false;
    unsigned int mod = 0;
    for (; mod < getWorld().sizeSkeletalModel(); mod++) {
        if (getWorld().getSkeletalModel(mod).getId() == object_id) {
            cached = true;
            break;
        }
    }

    if (!cached) {
        getWorld().addSkeletalModel(*new SkeletalModel(mTombRaider, index, i, object_id));
    }

    getWorld().addEntity(*new Entity(mTombRaider, index, i, mod));

    // Store reference to Lara
    if (getWorld().getEntity(getWorld().sizeEntity() - 1).getObjectId() == 0)
        mLara = getWorld().sizeEntity() - 1;

    if (i == mTombRaider.getSkyModelId())
        getRender().setSkyMesh(i, //moveable[i].starting_mesh,
                (mTombRaider.Engine() == TR_VERSION_2));
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

        getRender().loadTexture(Texture::generateColorTexture(color, 32, 32),
                32, 32, r_tri->texture);
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
            getWorld().addMesh(NULL); // Filler, to make meshes array ids align
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

        getWorld().addMesh(mesh);
    }

    printf("Done! Found %d meshes.\n", mTombRaider.getMeshCount());
}

