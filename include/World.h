/*!
 * \file include/World.h
 * \brief The game world (model)
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _WORLD_H_
#define _WORLD_H_

#include <memory>
#include <vector>

#include "Entity.h"
#include "Room.h"
#include "SkeletalModel.h"
#include "Sprite.h"
#include "StaticMesh.h"

/*!
 * \brief The game world (model)
 */
class World {
public:

    /*!
     * \brief Deconstructs an object of World
     */
    ~World();

    /*!
     * \brief Clears all data in world
     */
    void destroy();

    void addRoom(Room &room);
    unsigned long sizeRoom();
    Room &getRoom(unsigned long index);

    void addSprite(SpriteSequence &sprite);
    unsigned long sizeSprite();
    SpriteSequence &getSprite(unsigned long index);

    void addEntity(Entity &entity);
    unsigned long sizeEntity();
    Entity &getEntity(unsigned long index);

    void addSkeletalModel(SkeletalModel &model);
    unsigned long sizeSkeletalModel();
    SkeletalModel &getSkeletalModel(unsigned long index);

    void addStaticMesh(StaticMesh &model);
    unsigned long sizeStaticMesh();
    StaticMesh &getStaticMesh(unsigned long index);

    /*!
     * \brief Find room a location is in.
     *
     * If it fails to be in a room it gives closest overlapping room.
     * \param index Guessed room index
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \returns correct room index or -1 for unknown
     */
    long getRoomByLocation(long index, float x, float y, float z);

    /*!
     * \brief Find room a location is in.
     *
     * If it fails to be in a room it gives closest overlapping room.
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \returns correct room index or -1 for unknown
     */
    long getRoomByLocation(float x, float y, float z);

private:
    std::vector<std::unique_ptr<Room>> mRooms;
    std::vector<std::unique_ptr<SpriteSequence>> mSprites;
    std::vector<std::unique_ptr<Entity>> mEntities;
    std::vector<std::unique_ptr<SkeletalModel>> mModels;
    std::vector<std::unique_ptr<StaticMesh>> mMeshes;
};

World &getWorld();

#endif
