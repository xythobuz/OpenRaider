/*!
 * \file src/World.cpp
 * \brief The game world (model)
 *
 * \author Mongoose
 */

#include <cstdio>
#include <math.h>

#include "global.h"
#include "World.h"

World::~World() {
    destroy();
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

void World::addEntity(Entity &entity) {
    mEntities.push_back(&entity);
}

unsigned int World::sizeEntity() {
    return mEntities.size();
}

Entity &World::getEntity(unsigned int index) {
    assert(index < mEntities.size());
    return *mEntities.at(index);
}

void World::addSkeletalModel(SkeletalModel &model) {
    mModels.push_back(&model);
}

unsigned int World::sizeSkeletalModel() {
    return mModels.size();
}

SkeletalModel &World::getSkeletalModel(unsigned int index) {
    assert(index < mModels.size());
    return *mModels.at(index);
}

void World::addStaticMesh(StaticMesh &model) {
    mMeshes.push_back(&model);
}

unsigned int World::sizeStaticMesh() {
    return mMeshes.size();
}

StaticMesh &World::getStaticMesh(unsigned int index) {
    assert(index < mMeshes.size());
    return *mMeshes.at(index);
}


int World::getRoomByLocation(int index, float x, float y, float z)
{
    assert(index >= 0);
    assert(index < (int)mRooms.size());
    Room &room = *mRooms.at(index);

    if (room.getBoundingBox().inBox(x, y, z))
        return index;
    else
        return getRoomByLocation(x, y, z);
}


int World::getRoomByLocation(float x, float y, float z) {
    int hop = -1;

    for (unsigned int i = 0; i < mRooms.size(); i++) {
        if (mRooms.at(i)->getBoundingBox().inBoxPlane(x, z)) {
            if (mRooms.at(i)->getBoundingBox().inBox(x, y, z))
                return i;
            else
                hop = i; // This room is above or below current position
        }
    }

    return hop;
}


void World::destroy() {
    for (unsigned int i = 0; i < mRooms.size(); i++)
        delete mRooms[i];
    mRooms.clear();

    for (unsigned int i = 0; i < mSprites.size(); i++)
        delete mSprites[i];
    mSprites.clear();

    for (unsigned int i = 0; i < mEntities.size(); i++)
        delete mEntities[i];
    mEntities.clear();

    for (unsigned int i = 0; i < mModels.size(); i++)
        delete mModels[i];
    mModels.clear();

    for (unsigned int i = 0; i < mMeshes.size(); i++)
        delete mMeshes[i];
    mMeshes.clear();
}

