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

// Temp methods for rendering use until more refactoring is done
model_mesh_t *World::getMesh(int index)
{
    return mMeshes[index];
}

void World::addMesh(model_mesh_t *mesh)
{
    if (mesh)
        mMeshes.push_back(mesh);
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
    for (unsigned int i = 0; i != mRooms.size(); i++)
        delete mRooms[i];
    mRooms.clear();

    for (unsigned int i = 0; i != mSprites.size(); i++)
        delete mSprites[i];
    mSprites.clear();

    for (unsigned int i = 0; i != mEntities.size(); i++)
        delete mEntities[i];
    mEntities.clear();

    for (unsigned int i = 0; i != mModels.size(); i++)
        delete mModels[i];
    mModels.clear();

    for (std::vector<int>::size_type i = 0; i != mMeshes.size(); i++) {
        model_mesh_t *mesh = mMeshes[i];

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
}

