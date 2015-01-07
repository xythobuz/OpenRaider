/*!
 * \file include/Room.h
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifndef _ROOM_H_
#define _ROOM_H_

#include <memory>
#include <vector>

#include "Sprite.h"
#include "RoomData.h"
#include "RoomMesh.h"

enum RoomFlags {
    RoomFlagUnderWater = (1 << 0)
};

class Room {
  public:
    Room(glm::vec3 _pos, BoundingBox* _bbox, RoomMesh* _mesh, unsigned int f,
         int a, int x, int z);

    void prepare() { mesh->prepare(); }
    void display(glm::mat4 VP);

    bool isWall(unsigned long sector);
    long getSector(float x, float z, float* floor, float* ceiling);
    long getSector(float x, float z);
    void getHeightAtPosition(float x, float* y, float z);
    int getAdjoiningRoom(float x, float y, float z,
                         float x2, float y2, float z2);

    BoundingBox& getBoundingBox() { return *bbox; }
    RoomMesh& getMesh() { return *mesh; }

    unsigned int getNumXSectors() { return numXSectors; }
    unsigned int getNumZSectors() { return numZSectors; }

    unsigned int getFlags() { return flags; }

    unsigned long sizePortals();
    Portal& getPortal(unsigned long index);
    void addPortal(Portal* p);

    unsigned long sizeSectors();
    Sector& getSector(unsigned long index);
    void addSector(Sector* s);

    unsigned long sizeModels();
    StaticModel& getModel(unsigned long index);
    void addModel(StaticModel* s);

    unsigned long sizeLights();
    Light& getLight(unsigned long index);
    void addLight(Light* l);

    unsigned long sizeSprites();
    Sprite& getSprite(unsigned long index);
    void addSprite(Sprite* s);

  private:
    glm::vec3 pos;
    glm::mat4 model;
    std::unique_ptr<BoundingBox> bbox;
    std::unique_ptr<RoomMesh> mesh;

    unsigned int flags;
    int alternateRoom;
    int numXSectors;
    int numZSectors;

    std::vector<std::unique_ptr<Sprite>> sprites;
    std::vector<std::unique_ptr<StaticModel>> models;
    std::vector<std::unique_ptr<Portal>> portals;
    std::vector<std::unique_ptr<Sector>> sectors;
    std::vector<std::unique_ptr<Light>> lights;

    // Was used for "depth sorting" render list, but never assigned...?!
    //float dist; // Distance to near plane, move to room?
};

#endif

