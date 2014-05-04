/*!
 * \file src/Game.cpp
 * \brief Game abstraction
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <algorithm>
#include <map>
#include <cstdlib>

#include "main.h"
#include "Console.h"
#include "Game.h"
#include "utils/strings.h"

#include "games/TombRaider1.h"

// Old Code compatibility
#define TexelScale 256.0f

#ifndef EXPERIMENTAL_UNIFIED_ROOM_GEOMETERY
#define TextureLimit 24
#endif

#ifdef EXPERIMENTAL
std::vector<unsigned int> gColorTextureHACK;
#endif

#ifdef MULTITEXTURE
std::map<int, int> gMapTex2Bump;
#endif

Game::Game() {
    mLoaded = false;
    mName = NULL;
    mLara = NULL;

    mTextureStart = 0;
    mTextureOffset = 0;
}

Game::~Game() {
    destroy();
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

    mLoaded = false;
    mLara = NULL;
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
    if (mLara == NULL) {
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
            getWorld().moveEntity(mLara, 'f');
        } else if (action == backwardAction) {
            getWorld().moveEntity(mLara, 'b');
        } else if (action == leftAction) {
            getWorld().moveEntity(mLara, 'l');
        } else if (action == rightAction) {
            getWorld().moveEntity(mLara, 'r');
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
        mLara->angles[1] = getCamera().getRadianYaw();
        mLara->angles[2] = getCamera().getRadianPitch();
    }
}

void Game::processSprites() {
    float scale = 4.0f;
    tr2_item_t *item = mTombRaider.Item();
    tr2_sprite_texture_t *spriteTextures = mTombRaider.Sprite();
    tr2_sprite_sequence_t *spriteSequence = mTombRaider.SpriteSequence();

    printf("Processing sprites: ");

    for (int i = 0; i < (mTombRaider.NumItems() - 1); i++) {
        if ((mTombRaider.Engine() == TR_VERSION_1) && (item[i].intensity1 == -1))
            continue;

        int id = item[i].object_id;
        for (int j = 0; j < mTombRaider.NumSpriteSequences(); j++) {
            if (spriteSequence[j].object_id == id) {
                SpriteSequence &sequence = *new SpriteSequence();
                for (int k = 0; k < (-spriteSequence[j].negative_length); k++) {
                    tr2_sprite_texture_t *sprite = &spriteTextures[spriteSequence[j].offset];

                    int width = sprite->width >> 8;
                    int height = sprite->height >> 8;
                    int x = sprite->x;
                    int y = sprite->y;
                    int width2 = (int)(width * scale);
                    int height2 = (int)(height * scale);

                    vec3_t vertex[4];
                    vec2_t texel[4];
                    vec3_t pos;

                    // For vising use
                    pos[0] = item[i].x;
                    pos[1] = item[i].y;
                    pos[2] = item[i].z;

                    vertex[0][0] = -width2 / 2.0f;
                    vertex[1][0] = -width2 / 2.0f;
                    vertex[2][0] = width2 / 2.0f;
                    vertex[3][0] = width2 / 2.0f;

                    vertex[0][1] = 0;
                    vertex[1][1] = -height2;
                    vertex[2][1] = -height2;
                    vertex[3][1] = 0;

                    vertex[0][2] = 0;
                    vertex[1][2] = 0;
                    vertex[2][2] = 0;
                    vertex[3][2] = 0;

                    texel[3][0] = (vec_t)(x+width)/TexelScale;
                    texel[3][1] = (vec_t)(y+height)/TexelScale;

                    texel[2][0] = (vec_t)(x+width)/TexelScale;
                    texel[2][1] = (vec_t)(y)/TexelScale;

                    texel[1][0] = (vec_t)(x) /TexelScale;
                    texel[1][1] = (vec_t)(y) /TexelScale;

                    texel[0][0] = (vec_t)(x) / TexelScale;
                    texel[0][1] = (vec_t)(y+height)/TexelScale;

                    sequence.add(*new Sprite(vertex, texel, pos, width2 / 2.0f, sprite->tile + mTextureStart));

                    //printf(".");
                    //fflush(stdout);
                }
                getWorld().addSprite(sequence);
            }
        }
    }

    printf("Done! Found %d sprites.\n", mTombRaider.NumSpriteSequences());
}

void Game::processRooms() {
    printf("Processing rooms: ");

    for (int index = 0; index < mTombRaider.NumRooms(); index++) {
        Matrix transform;
        Room &room = *new Room(index);
        getWorld().addRoom(room);

        if (!mTombRaider.isRoomValid(index)) {
            getConsole().print("WARNING: Handling invalid vertex array in room");
            //printf("x");
            //fflush(stdout);
            //return; // ?
            continue;
        }

        unsigned int flags;
        vec3_t pos;
        vec3_t bbox[2];
        mTombRaider.getRoomInfo(index, &flags, pos, bbox[0], bbox[1]);
        room.setFlags(flags);
        room.setPos(pos);

        // Adjust positioning for OR world coordinate translation
        bbox[0][0] += pos[0];
        bbox[1][0] += pos[0];
        bbox[0][2] += pos[2];
        bbox[1][2] += pos[2];

        room.setBoundingBox(bbox);

        // Mongoose 2002.04.03, Setup 3D transform
        transform.setIdentity();
        transform.translate(pos);

        // Setup portals
        float portalVertices[12];
        unsigned int count = mTombRaider.getRoomPortalCount(index);

        // Current room is always first
        room.addAdjacentRoom(index);

        for (unsigned int i = 0; i < count; i++) {
            vec3_t vertices[4];
            vec3_t normal;
            int adjoiningRoom;
            mTombRaider.getRoomPortal(index, i, &adjoiningRoom, normal, portalVertices);
            for (unsigned int j = 0; j < 4; ++j) {
                vertices[j][0] = portalVertices[j*3];
                vertices[j][1] = portalVertices[j*3+1];
                vertices[j][2] = portalVertices[j*3+2];

                // Relative coors in vis portals
                transform.multiply3v(vertices[j], vertices[j]);
            }
            Portal &portal = *new Portal(vertices, normal, adjoiningRoom);
            room.addPortal(portal);
            room.addAdjacentRoom(adjoiningRoom);
        }

        // Physics/gameplay use /////////////////////////////

        //! \fixme Use more of sector structure, boxes, and floordata

        // List of sectors in this room
        unsigned int numXSectors, numZSectors;
        count = mTombRaider.getRoomSectorCount(index, &numZSectors, &numXSectors);
        room.setNumXSectors(numXSectors);
        room.setNumZSectors(numZSectors);
        for (unsigned int i = 0; i < count; i++) {
            unsigned int sectorFlags;
            int floorDataIndex, boxIndex, roomBelow, roomAbove;
            vec_t floor, ceiling;
            bool wall = false;

            mTombRaider.getRoomSector(index, i, &sectorFlags,
                    &ceiling, &floor, &floorDataIndex, &boxIndex,
                    &roomBelow, &roomAbove);

            if (sectorFlags & tombraiderSector_wall)
                wall = true;

            room.addSector(*new Sector(floor, ceiling, wall));
        }

        // Setup collision boxes
        //! fixme Should use sectors, but this is a test
        count = mTombRaider.getRoomBoxCount(index);

        //! fixme Only to be done only on room[0]?  I don't think so...
        for (unsigned int i = 0; !index && i < count; i++) {
            vec3_t a, b, c, d;
            mTombRaider.getRoomBox(index, i, a, b, c, d);
            room.addBox(*new Box(a, b, c, d));
        }

        // Setup room lights /////////////////////////////////////
        count = mTombRaider.getRoomLightCount(index);

        for (unsigned int i = 0; i < count; i++) {
            unsigned int lightFlags, lightType;
            vec4_t position;
            vec3_t dir;
            vec_t att;
            vec4_t color;
            vec_t cutoff;
            Light::LightType type;

            mTombRaider.getRoomLight(index, i, position, color,
                    dir, &att, &cutoff, &lightType, &lightFlags);

            switch (lightType) {
                case tombraiderLight_typeDirectional:
                    type = Light::typeDirectional;
                    break;
                case tombraiderLight_typeSpot:
                    type = Light::typeSpot;
                    break;
                case tombraiderLight_typePoint:
                default:
                    type = Light::typePoint;
            }

            room.addLight(*new Light(position, dir, att, color, cutoff, type));

            //! \todo Light flags?
        }

        // Room geometery //////////////////////////////////

//#define EXPERIMENTAL_UNIFIED_ROOM_GEOMETERY
#ifdef EXPERIMENTAL_UNIFIED_ROOM_GEOMETERY
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

        room.getMesh().bufferVertexArray(vertexCount, (vec_t *)vertexArray);
        room.getMesh().bufferNormalArray(normalCount, (vec_t *)normalArray);
        room.getMesh().bufferColorArray(vertexCount, (vec_t *)colorArray);

        mTombRaider.getRoomTriangles(index, mTextureStart,
                &triCount, &indices, &texCoords, &textures,
                &flags);

        room.getMesh().bufferTriangles(triCount, indices, texCoords, textures, flags);
#else
        float rgba[4];
        float xyz[3];

        count = mTombRaider.getRoomVertexCount(index);
        room.getMesh().allocateVertices(count);
        room.getMesh().allocateNormals(0); // count
        room.getMesh().allocateColors(count);

        for (unsigned int i = 0; i < count; ++i)
        {
            mTombRaider.getRoomVertex(index, i, xyz, rgba);

            room.getMesh().setVertex(i, xyz[0], xyz[1], xyz[2]);
            room.getMesh().setColor(i, rgba);
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

        for (unsigned int i = 0; i < TextureLimit; ++i)
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
        unsigned int r, t, q, v;
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
                getConsole().print("Handling bad room[%i].tris[%i].texture = %i",
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
                getConsole().print("Handling bad room[%i].quad[%i].texture = %i",
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
        room.getMesh().allocateTriangles(numTris);
        room.getMesh().allocateRectangles(numQuads);

        for (unsigned int i = 0, j = 0; i < TextureLimit; ++i)
        {
            if (tris_mesh_map[i] > 0)
            {
                j = tris_mesh_map[i] - 1;

                t = triangle_counter[i];

                room.getMesh().mTris[j].texture = i;
#ifdef MULTITEXTURE
                room.getMesh().mTris[j].bumpmap = gMapTex2Bump[i];
#endif
                room.getMesh().mTris[j].cnum_triangles = 0;
                room.getMesh().mTris[j].num_triangles = 0;
                room.getMesh().mTris[j].cnum_alpha_triangles = 0;
                room.getMesh().mTris[j].num_alpha_triangles = 0;
                room.getMesh().mTris[j].triangles = 0x0;
                room.getMesh().mTris[j].alpha_triangles = 0x0;
                room.getMesh().mTris[j].texcoors = 0x0;
                room.getMesh().mTris[j].texcoors2 = 0x0;

                if (t > 0)
                {
                    room.getMesh().mTris[j].num_triangles = t;
                    room.getMesh().mTris[j].triangles = new unsigned int[t*3];
                    room.getMesh().mTris[j].num_texcoors = t * 3;
                    room.getMesh().mTris[j].texcoors = new vec2_t[t * 3];
                }

                t = triangle_counter_alpha[i];

                if (t > 0)
                {
                    room.getMesh().mTris[j].num_alpha_triangles = t;
                    room.getMesh().mTris[j].alpha_triangles = new unsigned int[t*3];
                    room.getMesh().mTris[j].num_texcoors2 = t * 3;
                    room.getMesh().mTris[j].texcoors2 = new vec2_t[t * 3];
                }
            }

            ///////////////////////////////////////////

            if (rect_mesh_map[i] > 0)
            {
                j = rect_mesh_map[i] - 1;

                r = rectangle_counter[i];

                room.getMesh().mQuads[j].texture = i;
#ifdef MULTITEXTURE
                room.getMesh().mQuads[j].bumpmap = gMapTex2Bump[i];
#endif
                room.getMesh().mQuads[j].cnum_quads = 0;
                room.getMesh().mQuads[j].num_quads = 0;
                room.getMesh().mQuads[j].cnum_alpha_quads = 0;
                room.getMesh().mQuads[j].num_alpha_quads = 0;
                room.getMesh().mQuads[j].quads = 0x0;
                room.getMesh().mQuads[j].alpha_quads = 0x0;
                room.getMesh().mQuads[j].texcoors = 0x0;
                room.getMesh().mQuads[j].texcoors2 = 0x0;

                if (r > 0)
                {
                    room.getMesh().mQuads[j].num_quads = r;
                    room.getMesh().mQuads[j].quads = new unsigned int[r*4];
                    room.getMesh().mQuads[j].num_texcoors = r * 4;
                    room.getMesh().mQuads[j].texcoors = new vec2_t[r * 4];
                }

                r = rectangle_counter_alpha[i];

                if (r > 0)
                {
                    room.getMesh().mQuads[j].num_alpha_quads = r;
                    room.getMesh().mQuads[j].alpha_quads = new unsigned int[r*4];
                    room.getMesh().mQuads[j].num_texcoors2 = r * 4;
                    room.getMesh().mQuads[j].texcoors2 = new vec2_t[r * 4];
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

            unsigned int j = tris_mesh_map[texture] - 1;

            // Setup per vertex
            for (unsigned int i = 0; i < 3; ++i)
            {
                // Get vertex index {(0, a), (1, b), (2, c)}
                v = indices[i];

                if ((flags & tombraiderFace_Alpha ||
                            flags & tombraiderFace_PartialAlpha) &&
                        room.getMesh().mTris[j].num_alpha_triangles > 0)
                {
                    q = room.getMesh().mTris[j].cnum_alpha_triangles*3+i;

                    room.getMesh().mTris[j].alpha_triangles[q] = v;

                    room.getMesh().mTris[j].texcoors2[q][0] = texCoords[i*2];
                    room.getMesh().mTris[j].texcoors2[q][1] = texCoords[i*2+1];
                }
                else if (room.getMesh().mTris[j].num_triangles > 0)
                {
                    q = room.getMesh().mTris[j].cnum_triangles*3+i;

                    room.getMesh().mTris[j].triangles[q] = v;

                    room.getMesh().mTris[j].texcoors[q][0] = texCoords[i*2];
                    room.getMesh().mTris[j].texcoors[q][1] = texCoords[i*2+1];
                }

                // Partial alpha hack
                if (flags & tombraiderFace_PartialAlpha)
                {
                    //room.getMesh().colors[v].rgba[3] = 0.45;
                }
            }

            if (flags & tombraiderFace_Alpha ||
                    flags & tombraiderFace_PartialAlpha)
            {
                room.getMesh().mTris[j].cnum_alpha_triangles++;
            }
            else
            {
                room.getMesh().mTris[j].cnum_triangles++;
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

            unsigned int j = rect_mesh_map[texture] - 1;

            if (room.getMesh().mQuads[j].num_quads <= 0 &&
                    room.getMesh().mQuads[j].num_alpha_quads <= 0)
                continue;

            // Setup per vertex
            for (unsigned int i = 0; i < 4; ++i)
            {
                // Get vertex index {(0, a), (1, b), (2, c), (3, d)}
                v = indices[i];

                if ((flags & tombraiderFace_Alpha ||
                            flags & tombraiderFace_PartialAlpha) &&
                        room.getMesh().mQuads[j].num_alpha_quads > 0)
                {
                    q = room.getMesh().mQuads[j].cnum_alpha_quads*4+i;

                    room.getMesh().mQuads[j].alpha_quads[q] = v;

                    room.getMesh().mQuads[j].texcoors2[q][0] = texCoords[i*2];
                    room.getMesh().mQuads[j].texcoors2[q][1] = texCoords[i*2+1];
                }
                else if (room.getMesh().mQuads[j].num_quads > 0)
                {
                    q = room.getMesh().mQuads[j].cnum_quads*4+i;

                    room.getMesh().mQuads[j].quads[q] = v;

                    room.getMesh().mQuads[j].texcoors[q][0] = texCoords[i*2];
                    room.getMesh().mQuads[j].texcoors[q][1] = texCoords[i*2+1];
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
                room.getMesh().mQuads[j].cnum_alpha_quads++;
            }
            else
            {
                room.getMesh().mQuads[j].cnum_quads++;
            }
        }
#endif

        // Room models
        count = mTombRaider.getRoomModelCount(index);
        for (unsigned int i = 0; i < count; i++) {
            int ind;
            vec_t yaw;
            vec3_t position;
            mTombRaider.getRoomModel(index, i, &ind, position, &yaw);
            room.addModel(*new StaticModel(ind, yaw, position));
        }

        // Room sprites
        count = mTombRaider.getRoomSpriteCount(index);
        for (unsigned int i = 0; i < count; i++) {
            float spriteVertices[12];
            float spriteTexCoords[8];
            vec3_t vertex[4];
            vec2_t texel[4];
            vec3_t position;
            int tex;

            mTombRaider.getRoomSprite(index, i,
                    10.0f, &tex, position, spriteVertices, spriteTexCoords);

            tex += mTextureStart; // OpenRaider preloads some textures

            for (unsigned int j = 0; j < 12; j++)
                vertex[j / 3][j % 3] = spriteVertices[j];

            for (unsigned int j = 0; j < 8; j++)
                texel[j / 2][j % 2] = spriteTexCoords[j];

            room.addSprite(*new Sprite(vertex, texel, position, 0.0f, tex)); //! \fixme radius 0??
        }

        getWorld().addRoom(room);

        //printf(".");
        //fflush(stdout);
    }

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
    getRender().addSkeletalModel(sModel);
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
    // if (getWorld().isCachedSkeletalModel(moveable[index].object_id))
    // {
    //   thing->modelId = getRender().add(sModel);
    //   return;
    // }

    r_model = new skeletal_model_t;
    r_model->id = moveable[index].object_id;

    // Gather more info if this is lara
    if (moveable[index].object_id == 0)
    {
        lara = true;
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
        getConsole().print("Invalid animation data for model %d", index);
        delete r_model;
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
        getWorld().addEntity(thing);

        k = getWorld().addModel(r_model);

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

                    getRender().setFlags(Render::fRenderPonytail);
                    getConsole().print("Found known ponytail");
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

                    getRender().setFlags(Render::fRenderPonytail);
                    getConsole().print("Found ponytail?");
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
        getWorld().addEntity(thing);
        getWorld().addModel(c_model);
        printf("c");
        return;
    }

    int aloop = mTombRaider.getNumAnimsForMoveable(index);

#ifdef DEBUG_MOVEABLE
    printf("\nanimation = %i, num_animations = %i\n",
            moveable[index].animation, aloop);
    printf("\nitem[%i].flags = %i\nentity[%i]\n",
            i, item[i].flags, thing->id);
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

#ifdef DEBUG_MOVEABLE
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

#ifdef DEBUG_MOVEABLE
            printf("animation[%i].boneframe[%u] = offset %u, step %i\n",
                    a, f, frame_offset, frame_step);
#endif

            // Mongoose 2002.08.15, Was
            //   if (frame_offset + 8 > _tombraider.NumFrames())
            if (frame_offset > mTombRaider.NumFrames())
            {
                getConsole().print("WARNING: Bad animation frame %i > %i",
                        frame_offset, mTombRaider.NumFrames());

                // Mongoose 2002.08.15, Attempt to skip more likely bad animation data
                getConsole().print("WARNING: Handling bad animation data...");
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
        getRender().setSkyMesh(i, //moveable[i].starting_mesh,
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

        getRender().loadTexture(Texture::generateColorTexture(color, 32, 32),
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
        //printf(".");
        //fflush(stdout);
    }

    printf("Done! Found %d meshes.\n", mTombRaider.getMeshCount());
}

