/*!
 * \file src/World.cpp
 * \brief The game world (model)
 *
 * \author Mongoose
 */

#include <cstdio>
#include <math.h>
#include <assert.h>

#include "World.h"


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

World::World()
{
    mFlags = 0;
}


World::~World()
{
    destroy();
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

int World::getRoomByLocation(int index, float x, float y, float z)
{
    room_mesh_t *room = mRooms[index];

    if (room)
    {
        if (x > room->bbox_min[0] && x < room->bbox_max[0] &&
                z > room->bbox_min[2] && z < room->bbox_max[2])
        {
            if (y > room->bbox_min[1] && y < room->bbox_max[1])
                return index;
        }
    }

    return getRoomByLocation(x, y, z);
}


int World::getRoomByLocation(float x, float y, float z)
{
    room_mesh_t *room;
    int hop = -1;


    for(std::vector<int>::size_type i = 0; i < mRooms.size(); i++)
    {
        room = mRooms[i];

        if (!room)
            continue;

        if ((x > room->bbox_min[0]) && (x < room->bbox_max[0]) &&
                (z > room->bbox_min[2]) && (z < room->bbox_max[2]))
        {
            // This room contains current position
            if ((y > room->bbox_min[1]) && (y < room->bbox_max[1]))
                return i;

            // This room is above or below current position
            hop = i;
        }
    }

    // Room is -1?  Must be in void, try to hop to room with same X,Z
    if (mFlags & fEnableHopping)
        return hop;

    return -1;
}


int World::getAdjoiningRoom(int index,
        float x, float y, float z,
        float x2, float y2, float z2)
{
    room_mesh_t *room = mRooms[index];
    portal_t * portal;
    vec3_t intersect, p1, p2;


    p1[0] = x;  p1[1] = y;  p1[2] = z;
    p2[0] = x2; p2[1] = y2; p2[2] = z2;

    if (room)
    {
        for(std::vector<int>::size_type i = 0; i != room->portals.size(); i++)
        {
            portal = room->portals[i];

            if (!portal)
                continue;

            if (intersectionLinePolygon(intersect, p1, p2, // 4,
                        portal->vertices))
            {
                return portal->adjoining_room;
            }
        }
    }

    return -1;
}


int World::getSector(int room, float x, float z, float *floor, float *ceiling)
{
    room_mesh_t *r;
    sector_t * s;
    int sector;

    assert(room >= 0);
    assert(floor != NULL);
    assert(ceiling != NULL);

    r = mRooms[room];

    if (!r)
        return -1;

    sector = (((((int)x - (int)r->pos[0]) / 1024) * r->numZSectors) +
            (((int)z - (int)r->pos[2]) / 1024));

    if (sector > -1)
    {
        s = r->sectors[sector];

        if (!s)
            return -1;

        *floor = s->floor;
        *ceiling = s->ceiling;
    }

    return sector;
}


int World::getSector(int room, float x, float z) {
    int sector;
    room_mesh_t *r;

    if ((room < 0) || (room >= (int)mRooms.size()))
        return -1;

    r = mRooms[room];

    if (!r)
        return -1;

    sector = (((((int)x - (int)r->pos[0]) / 1024) * r->numZSectors) +
            (((int)z - (int)r->pos[2]) / 1024));

    if (sector < 0)
        return -1;

    return sector;
}


unsigned int World::getRoomInfo(int room) {
    room_mesh_t *r;

    if ((room >= (int)mRooms.size()) || (room < 0))
        return 0;

    r = mRooms[room];

    if (!r)
        return 0;

    return r->flags;
}


bool World::isWall(int room, int sector) {
    room_mesh_t *r;
    sector_t *sect;

    if ((room >= (int)mRooms.size()) || (room < 0))
        return true;

    r = mRooms[room];

    if ((!r) || (sector >= (int)r->sectors.size()) || (sector < 0))
        return true;

    sect = r->sectors[sector];

    if (!sect)
        return true;

    return ((sector > 0) && sect->wall); //! \fixme is (sector > 0) correct??
}


bool World::getHeightAtPosition(int index, float x, float *y, float z)
{
    room_mesh_t *room = mRooms[index];
    int sector;
    sector_t *sect;

    if (!room)
    {
        return false;
    }

    // Mongoose 2002.08.14, Remember sector_z is width of sector array
    sector = getSector(index, x, z);

    sect = room->sectors[sector];

    if (!sect)
    {
        return true;
    }

    *y = sect->floor;

    return true;
}


// Temp methods for rendering use until more refactoring is done
#ifdef BAD_BLOOD
model_mesh_t *World::getMesh(int index)
{
    return mMeshes[index];
}

skeletal_model_t *World::getModel(int index)
{
    return mModels[index];
}

room_mesh_t *World::getRoom(int index)
{
    return mRooms[index];
}

std::vector<entity_t *> *World::getEntities()
{
    return &mEntities;
}

std::vector<sprite_seq_t *> *World::getSprites()
{
    return &mSprites;
}

std::vector<room_mesh_t *> *World::getRooms()
{
    return &mRooms;
}
#endif


void World::setFlag(WorldFlag flag)
{
    mFlags |= flag;
}



void World::clearFlag(WorldFlag flag)
{
    mFlags &= ~flag;
}


void World::destroy()
{
    room_mesh_t *room;
    model_mesh_t *mesh;
    sprite_seq_t *sprite;
    skeletal_model_t *model;
    bone_frame_t *boneframe;
    bone_tag_t *tag;
    animation_frame_t *animation;
    std::list<skeletal_model_t *> cache;

    for (std::vector<int>::size_type i = 0; i != mEntities.size(); i++)
        delete mEntities[i];
    mEntities.clear();

    for (std::vector<int>::size_type i = 0; i != mRooms.size(); i++) {
        room = mRooms[i];

        if (room) {
            //! \fixme Causes "freeing already freed pointer" exceptions or EXEC_BAD_ACCESS

            //for (std::vector<int>::size_type j = 0; j != room->portals.size(); j++)
            //    delete room->portals[i];
            room->portals.clear();

            //for (std::vector<int>::size_type j = 0; j != room->models.size(); j++)
            //    delete room->models[i];
            room->models.clear();

            //for (std::vector<int>::size_type j = 0; j != room->sprites.size(); j++)
            //    delete room->sprites[i];
            room->sprites.clear();

            //for (std::vector<int>::size_type j = 0; j != room->sectors.size(); j++)
            //    delete room->sectors[i];
            room->sectors.clear();

            //for (std::vector<int>::size_type j = 0; j != room->boxes.size(); j++)
            //    delete room->boxes[i];
            room->boxes.clear();

            delete room;
        }
    }

    for (std::vector<int>::size_type i = 0; i != mMeshes.size(); i++) {
        mesh = mMeshes[i];

        if (!mesh)
            continue;

        for (std::vector<int>::size_type j = 0; j != mesh->texturedTriangles.size(); j++) {
            if (mesh->texturedTriangles[j])
                delete mesh->texturedTriangles[j];
        }

        for (std::vector<int>::size_type j = 0; j != mesh->coloredTriangles.size(); j++) {
            if (mesh->coloredTriangles[j])
                delete mesh->coloredTriangles[j];
        }

        for (std::vector<int>::size_type j = 0; j != mesh->texturedRectangles.size(); j++) {
            if (mesh->texturedRectangles[j])
                delete mesh->texturedRectangles[j];
        }

        for (std::vector<int>::size_type j = 0; j != mesh->coloredRectangles.size(); j++) {
            if (mesh->coloredRectangles[j])
                delete mesh->coloredRectangles[j];
        }

        if (mesh->vertices)
            delete [] mesh->vertices;

        if (mesh->colors)
            delete [] mesh->colors;

        if (mesh->normals)
            delete [] mesh->normals;

        delete mesh;
    }

    mMeshes.clear();

    for (std::vector<int>::size_type i = 0; i != mSprites.size(); i++) {
        sprite = mSprites[i];

        if (!sprite)
            continue;

        if (sprite->sprite)
            delete [] sprite->sprite;

        delete sprite;
    }

    mSprites.clear();

    for (std::vector<int>::size_type i = 0; i != mModels.size(); i++) {
        model = mModels[i];

        if (!model)
            continue;

        // No smart pointers, so skip if deleted once  =)
        bool found = false;
        for (std::list<skeletal_model_t *>::const_iterator iterator = cache.begin(), end = cache.end(); iterator != end; ++iterator) {
            if (model == *iterator) {
                found = true;
                break;
            }
        }

        if (!found)
            cache.push_back(model);
        else
            continue;

        for (std::vector<int>::size_type j = 0; j != model->animation.size(); j++) {
            animation  = model->animation[j];

            if (!animation)
                continue;

            for (std::vector<int>::size_type k = 0; k != animation->frame.size(); k++) {
                boneframe = animation->frame[k];

                if (!boneframe)
                    continue;

                for (std::vector<int>::size_type l = 0; l != boneframe->tag.size(); l++) {
                    tag = boneframe->tag[l];

                    if (!tag)
                        continue;

                    delete tag;
                }

                delete boneframe;
            }

            delete animation;
        }

        delete model;
    }

    mModels.clear();
}


void World::addRoom(room_mesh_t *room)
{
    mRooms.push_back(room);
}


void World::addMesh(model_mesh_t *mesh)
{
    if (mesh)
        mMeshes.push_back(mesh);
}


void World::addEntity(entity_t *e)
{
    if (e)
    {
        e->master = 0x0;
        e->moveType = worldMoveType_walk; // Walk
        e->room = getRoomByLocation(e->pos[0], e->pos[1], e->pos[2]);
        mEntities.push_back(e);
    }
}


int World::addModel(skeletal_model_t *model)
{
    if (model)
    {
        mModels.push_back(model);
        return mModels.size() - 1;
    }

    return -1;
}


void World::addSprite(sprite_seq_t *sprite)
{
    if (sprite)
        mSprites.push_back(sprite);
}


void World::moveEntity(entity_t *e, char movement)
{
    const float moved = 180.0f;
    const float testd = 220.0f;
    const float camHeight = 8.0f;
    float x, y, z, pitch, h, floor, ceiling;
    int room, sector;
    bool wall;
    unsigned int roomFlags;


    if (!e)
    {
        return;
    }

    switch (e->moveType)
    {
        case worldMoveType_walkNoSwim:
        case worldMoveType_walk:
            pitch = 0.0f;  // in the future pitch could control jump up blocks here
            break;

        case worldMoveType_noClipping:
        case worldMoveType_fly:
        case worldMoveType_swim:
            pitch = e->angles[2];
            break;
    }

    switch (movement)
    {
        case 'f':
            x = e->pos[0] + (testd * sinf(e->angles[1]));
            y = e->pos[1] + (testd * sinf(pitch));
            z = e->pos[2] + (testd * cosf(e->angles[1]));
            break;
        case 'b':
            x = e->pos[0] - (testd * sinf(e->angles[1]));
            y = e->pos[1] - (testd * sinf(pitch));
            z = e->pos[2] - (testd * cosf(e->angles[1]));
            break;
        case 'l':
            x = e->pos[0] - (testd * sinf(e->angles[1] + 90.0f));
            y = e->pos[1];
            z = e->pos[2] - (testd * cosf(e->angles[1] + 90.0f));
            break;
        case 'r':
            x = e->pos[0] + (testd * sinf(e->angles[1] + 90.0f));
            y = e->pos[1];
            z = e->pos[2] + (testd * cosf(e->angles[1] + 90.0f));
            break;
        default:
            return;
    }

    //room = getRoomByLocation(x, y, z);
    room = getRoomByLocation(e->room, x, y, z);

    if (room == -1) // Will we hit a portal?
    {
        room = getAdjoiningRoom(e->room,
                e->pos[0],  e->pos[1], e->pos[2],
                x, y, z);

        if (room > -1)
        {
            printf("Crossing from room %i to %i\n", e->room, room);
        } else {
            //! \fixme mRooms, sectors, ... are now std::vector, but often upper bound checks are missing
            return;
        }
    }

    roomFlags = getRoomInfo(room);
    sector = getSector(room, x, z, &floor, &ceiling);
    wall = isWall(room, sector);

    // If you're underwater you may want to swim  =)
    // ...if you're worldMoveType_walkNoSwim, you better hope it's shallow
    if (roomFlags & roomFlag_underWater && e->moveType == worldMoveType_walk)
    {
        e->moveType = worldMoveType_swim;
    }

    // Don't swim on land
    if (!(roomFlags & roomFlag_underWater) && e->moveType == worldMoveType_swim)
    {
        e->moveType = worldMoveType_walk;
    }

    // Mongoose 2002.09.02, Add check for room -> room transition
    //   ( Only allow by movement between rooms by using portals )
    if (((e->moveType == worldMoveType_noClipping) ||
                (e->moveType == worldMoveType_fly) ||
                (e->moveType == worldMoveType_swim)) ||
            ((room > -1) && (!wall)))
    {
        e->room = room;

        switch (movement)
        {
            case 'f':
                x = e->pos[0] + (moved * sinf(e->angles[1]));
                y = e->pos[1] + (moved * sinf(pitch));
                z = e->pos[2] + (moved * cosf(e->angles[1]));
                break;
            case 'b':
                x = e->pos[0] - (moved * sinf(e->angles[1]));
                y = e->pos[1] - (moved * sinf(pitch));
                z = e->pos[2] - (moved * cosf(e->angles[1]));
                break;
            case 'l':
                x = e->pos[0] - (moved * sinf(e->angles[1] + 90.0f));
                z = e->pos[2] - (moved * cosf(e->angles[1] + 90.0f));
                break;
            case 'r':
                x = e->pos[0] + (moved * sinf(e->angles[1] + 90.0f));
                z = e->pos[2] + (moved * cosf(e->angles[1] + 90.0f));
                break;
        }

        /*! \fixme Test for vector (move vector) / plane (portal) collision here
         * to see if we need to switch rooms... man...
         */

        h = y;
        getHeightAtPosition(room, x, &h, z);

        switch (e->moveType)
        {
            case worldMoveType_fly:
            case worldMoveType_swim:
                // Don't fall out of world, avoid a movement that does
                if (h > y - camHeight)
                {
                    e->pos[0] = x;
                    e->pos[1] = y;
                    e->pos[2] = z;
                }
                break;
            case worldMoveType_walk:
            case worldMoveType_walkNoSwim:
                y = e->pos[1];  // Override vector movement walking ( er, not pretty )

                // Now fake gravity
                // Mongoose 2002.08.14, Remember TR is upside down ( you fall 'up' )

                //ddist = h - e->pos[1];

                // This is to force false gravity, by making camera stay on ground
                e->pos[1] = h; //roomFloor->bbox_min[1];

                // Check for camera below terrian and correct
                if (e->pos[1] < h - camHeight)
                {
                    e->pos[1] = h - camHeight;
                }

                e->pos[0] = x;
                e->pos[2] = z;
                break;
            case worldMoveType_noClipping:
                e->pos[0] = x;
                e->pos[1] = y;
                e->pos[2] = z;
        }
    }
    else
    {
        e->moving = false;
        return;
    }

    e->room = room;
    e->moving = true;
}

