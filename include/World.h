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
    static void destroy();

    static void addRoom(Room* room);
    static unsigned long sizeRoom();
    static Room& getRoom(unsigned long index);

    static void addSprite(Sprite* sprite);
    static unsigned long sizeSprite();
    static Sprite& getSprite(unsigned long index);

    static void addSpriteSequence(SpriteSequence* sprite);
    static unsigned long sizeSpriteSequence();
    static SpriteSequence& getSpriteSequence(unsigned long index);

    static void addEntity(Entity* entity);
    static unsigned long sizeEntity();
    static Entity& getEntity(unsigned long index);

    static void addSkeletalModel(SkeletalModel* model);
    static unsigned long sizeSkeletalModel();
    static SkeletalModel& getSkeletalModel(unsigned long index);

    static void addStaticMesh(StaticMesh* model);
    static unsigned long sizeStaticMesh();
    static StaticMesh& getStaticMesh(unsigned long index);

    static void addMesh(Mesh* mesh);
    static unsigned long sizeMesh();
    static Mesh& getMesh(unsigned long index);

    static void displayUI();

  private:
    static std::vector<std::unique_ptr<Room>> rooms;
    static std::vector<std::unique_ptr<Sprite>> sprites;
    static std::vector<std::unique_ptr<SpriteSequence>> spriteSequences;
    static std::vector<std::unique_ptr<Entity>> entities;
    static std::vector<std::unique_ptr<SkeletalModel>> models;
    static std::vector<std::unique_ptr<StaticMesh>> staticMeshes;
    static std::vector<std::unique_ptr<Mesh>> meshes;
};

#endif

