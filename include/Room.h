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
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Mesh.h"
#include "Sprite.h"
#include "RoomData.h"

enum RoomFlags {
    RoomFlagUnderWater = (1 << 0)
};

class Room {
  public:
    Room(glm::vec3 _pos, BoundingBox* _bbox, Mesh* _mesh, unsigned int f)
        : pos(_pos), bbox(_bbox), mesh(_mesh), flags(f) { }

    void prepare();
    void display(glm::mat4 view, glm::mat4 projection);

    bool isWall(unsigned long sector);
    long getSector(float x, float z, float* floor, float* ceiling);
    long getSector(float x, float z);
    void getHeightAtPosition(float x, float* y, float z);
    int getAdjoiningRoom(float x, float y, float z,
                         float x2, float y2, float z2);

    BoundingBox& getBoundingBox() { return *bbox; }
    Mesh& getMesh() { return *mesh; }

    void setNumXSectors(unsigned int n) { numXSectors = n; }
    unsigned int getNumXSectors() { return numXSectors; }

    void setNumZSectors(unsigned int n) { numZSectors = n; }
    unsigned int getNumZSectors() { return numZSectors; }

    unsigned int getFlags() { return flags; }

    unsigned long sizeAdjacentRooms();
    long getAdjacentRoom(unsigned long index);
    void addAdjacentRoom(long r);

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
    std::unique_ptr<BoundingBox> bbox;
    std::unique_ptr<Mesh> mesh;

    unsigned int flags;
    unsigned int numXSectors;
    unsigned int numZSectors;

    std::vector<long> adjacentRooms;
    std::vector<std::unique_ptr<Sprite>> sprites;
    std::vector<std::unique_ptr<StaticModel>> models;
    std::vector<std::unique_ptr<Portal>> portals;
    std::vector<std::unique_ptr<Sector>> sectors;
    std::vector<std::unique_ptr<Light>> lights;

    // Was used for "depth sorting" render list, but never assigned...?!
    //float dist; // Distance to near plane, move to room?
};

#endif

