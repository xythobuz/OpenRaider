/*!
 * \file include/World.h
 * \brief World Model
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _WORLD_H_
#define _WORLD_H_

#include <memory>
#include <vector>

#include "Entity.h"
#include "Mesh.h"
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

    void addRoom(Room* room);
    unsigned long sizeRoom();
    Room& getRoom(unsigned long index);

    void addSprite(Sprite* sprite);
    unsigned long sizeSprite();
    Sprite& getSprite(unsigned long index);

    void addSpriteSequence(SpriteSequence* sprite);
    unsigned long sizeSpriteSequence();
    SpriteSequence& getSpriteSequence(unsigned long index);

    void addEntity(Entity* entity);
    unsigned long sizeEntity();
    Entity& getEntity(unsigned long index);

    void addSkeletalModel(SkeletalModel* model);
    unsigned long sizeSkeletalModel();
    SkeletalModel& getSkeletalModel(unsigned long index);

    void addStaticMesh(StaticMesh* model);
    unsigned long sizeStaticMesh();
    StaticMesh& getStaticMesh(unsigned long index);

    void addMesh(Mesh* mesh);
    unsigned long sizeMesh();
    Mesh& getMesh(unsigned long index);

  private:
    std::vector<std::unique_ptr<Room>> mRooms;
    std::vector<std::unique_ptr<Sprite>> mSprites;
    std::vector<std::unique_ptr<SpriteSequence>> mSpriteSequences;
    std::vector<std::unique_ptr<Entity>> mEntities;
    std::vector<std::unique_ptr<SkeletalModel>> mModels;
    std::vector<std::unique_ptr<StaticMesh>> mStaticMeshes;
    std::vector<std::unique_ptr<Mesh>> mMeshes;
};

World& getWorld();

#endif
