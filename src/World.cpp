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

void World::addRoom(Room* room) {
    mRooms.emplace_back(std::unique_ptr<Room>(room));
}

unsigned long World::sizeRoom() {
    return mRooms.size();
}

Room& World::getRoom(unsigned long index) {
    assert(index < mRooms.size());
    return *mRooms.at(index);
}

void World::addSprite(SpriteSequence* sprite) {
    mSprites.emplace_back(std::unique_ptr<SpriteSequence>(sprite));
}

unsigned long World::sizeSprite() {
    return mSprites.size();
}

SpriteSequence& World::getSprite(unsigned long index) {
    assert(index < mSprites.size());
    return *mSprites.at(index);
}

void World::addEntity(Entity* entity) {
    mEntities.emplace_back(std::unique_ptr<Entity>(entity));
}

unsigned long World::sizeEntity() {
    return mEntities.size();
}

Entity& World::getEntity(unsigned long index) {
    assert(index < mEntities.size());
    return *mEntities.at(index);
}

void World::addSkeletalModel(SkeletalModel* model) {
    mModels.emplace_back(std::unique_ptr<SkeletalModel>(model));
}

unsigned long World::sizeSkeletalModel() {
    return mModels.size();
}

SkeletalModel& World::getSkeletalModel(unsigned long index) {
    assert(index < mModels.size());
    return *mModels.at(index);
}

void World::addStaticMesh(StaticMesh* model) {
    mMeshes.emplace_back(std::unique_ptr<StaticMesh>(model));
}

unsigned long World::sizeStaticMesh() {
    return mMeshes.size();
}

StaticMesh& World::getStaticMesh(unsigned long index) {
    assert(index < mMeshes.size());
    return *mMeshes.at(index);
}


long World::getRoomByLocation(long index, float x, float y, float z) {
    assert(index >= 0);
    assert(index < (long)mRooms.size());
    Room& room = *mRooms.at(index);

    if (room.getBoundingBox().inBox(x, y, z))
        return index;
    else
        return getRoomByLocation(x, y, z);
}


long World::getRoomByLocation(float x, float y, float z) {
    long hop = -1;

    for (unsigned long i = 0; i < mRooms.size(); i++) {
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
    mRooms.clear();
    mSprites.clear();
    mEntities.clear();
    mModels.clear();
    mMeshes.clear();
}

