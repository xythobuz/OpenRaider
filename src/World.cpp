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

World::~World()
{
    destroy();
}


// Temp methods for rendering use until more refactoring is done
model_mesh_t *World::getMesh(int index)
{
    return mMeshes[index];
}

skeletal_model_t *World::getModel(int index)
{
    return mModels[index];
}

std::vector<entity_t *> *World::getEntities()
{
    return &mEntities;
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


void World::addRoom(Room &room) {
    mRooms.push_back(&room);
}

unsigned int World::sizeRoom() {
    return mRooms.size();
}

Room &World::getRoom(unsigned int index) {
    assert(index < mRooms.size());
    return *mRooms.at(index);
}

void World::addSprite(SpriteSequence &sprite) {
    mSprites.push_back(&sprite);
}

unsigned int World::sizeSprite() {
    return mSprites.size();
}

SpriteSequence &World::getSprite(unsigned int index) {
    assert(index < mSprites.size());
    return *mSprites.at(index);
}


int World::getRoomByLocation(int index, float x, float y, float z)
{
    assert(index >= 0);
    assert(index < mRooms.size());
    Room &room = *mRooms.at(index);

    if (room.inBox(x, y, z))
        return index;
    else
        return getRoomByLocation(x, y, z);
}


int World::getRoomByLocation(float x, float y, float z) {
    int hop = -1;

    for (unsigned int i = 0; i < mRooms.size(); i++) {
        if (mRooms.at(i)->inBoxPlane(x, z)) {
            if (mRooms.at(i)->inBox(x, y, z))
                return i;
            else
                hop = i; // This room is above or below current position
        }
    }

    return hop;
}


int World::getAdjoiningRoom(int index,
        float x, float y, float z,
        float x2, float y2, float z2) {
    assert(index >= 0);
    assert(index < mRooms.size());
    Room &room = *mRooms.at(index);
    vec3_t intersect, p1, p2;

    p1[0] = x;  p1[1] = y;  p1[2] = z;
    p2[0] = x2; p2[1] = y2; p2[2] = z2;

    for (unsigned int i = 0; i < room.sizePortals(); i++) {
        if (intersectionLinePolygon(intersect, p1, p2, //4,
                    room.getPortal(i).getVertices()))
            return room.getPortal(i).getAdjoiningRoom();
    }

    return -1;
}


int World::getSector(int room, float x, float z, float *floor, float *ceiling) {
    assert(room >= 0);
    assert(room < mRooms.size());
    assert(floor != NULL);
    assert(ceiling != NULL);

    int sector = getSector(room, x, z);

    if ((sector >= 0) && (sector < mRooms.at(room)->sizeSectors())) {
        *floor = mRooms.at(room)->getSector(sector).getFloor();
        *ceiling = mRooms.at(room)->getSector(sector).getCeiling();
    }

    return sector;
}


int World::getSector(int room, float x, float z) {
    assert(room >= 0);
    assert(room < mRooms.size());

    vec3_t pos;
    mRooms.at(room)->getPos(pos);
    int sector = (((((int)x - (int)pos[0]) / 1024) *
        mRooms.at(room)->getNumZSectors()) + (((int)z - (int)pos[2]) / 1024));

    if (sector < 0)
        return -1;

    return sector;
}


unsigned int World::getRoomInfo(int room) {
    assert(room >= 0);
    assert(room < mRooms.size());

    return mRooms.at(room)->getFlags();
}


bool World::isWall(int room, int sector) {
    assert(room >= 0);
    assert(room < mRooms.size());
    assert(sector >= 0);
    assert(sector < mRooms.at(room)->sizeSectors());

    //! \fixme is (sector > 0) correct??
    return ((sector > 0) && mRooms.at(room)->getSector(sector).isWall());
}


void World::getHeightAtPosition(int index, float x, float *y, float z) {
    assert(index >= 0);
    assert(index < mRooms.size());

    int sector = getSector(index, x, z);
    if ((sector >= 0) && (sector < mRooms.at(index)->sizeSectors()))
        *y = mRooms.at(index)->getSector(sector).getFloor();
}


void World::destroy()
{
    for (unsigned int i = 0; i != mRooms.size(); i++)
        delete mRooms[i];

    for (unsigned int i = 0; i != mSprites.size(); i++)
        delete mSprites[i];

    model_mesh_t *mesh;
    skeletal_model_t *model;
    bone_frame_t *boneframe;
    bone_tag_t *tag;
    animation_frame_t *animation;
    std::list<skeletal_model_t *> cache;

    for (std::vector<int>::size_type i = 0; i != mEntities.size(); i++)
        delete mEntities[i];

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

