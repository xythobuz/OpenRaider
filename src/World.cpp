/*!
 * \file src/World.cpp
 * \brief The game world (model)
 *
 * \author Mongoose
 */

#include "global.h"
#include "World.h"

World::~World() {
    destroy();
}

void World::destroy() {
    mRooms.clear();
    mSprites.clear();
    mEntities.clear();
    mModels.clear();
    mStaticMeshes.clear();
    mMeshes.clear();
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

void World::addSprite(Sprite* sprite) {
    mSprites.emplace_back(std::unique_ptr<Sprite>(sprite));
}

unsigned long World::sizeSprite() {
    return mSprites.size();
}

Sprite& World::getSprite(unsigned long index) {
    assert(index < mSprites.size());
    return *mSprites.at(index);
}

void World::addSpriteSequence(SpriteSequence* sprite) {
    mSpriteSequences.emplace_back(std::unique_ptr<SpriteSequence>(sprite));
}

unsigned long World::sizeSpriteSequence() {
    return mSpriteSequences.size();
}

SpriteSequence& World::getSpriteSequence(unsigned long index) {
    assert(index < mSpriteSequences.size());
    return *mSpriteSequences.at(index);
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
    mStaticMeshes.emplace_back(std::unique_ptr<StaticMesh>(model));
}

unsigned long World::sizeStaticMesh() {
    return mStaticMeshes.size();
}

StaticMesh& World::getStaticMesh(unsigned long index) {
    assert(index < mStaticMeshes.size());
    return *mStaticMeshes.at(index);
}

void World::addMesh(Mesh* mesh) {
    mMeshes.emplace_back(mesh);
}

unsigned long World::sizeMesh() {
    return mMeshes.size();
}

Mesh& World::getMesh(unsigned long index) {
    assert(index < mMeshes.size());
    return *mMeshes.at(index);
}

