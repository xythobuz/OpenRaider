/*!
 * \file include/Room.h
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifndef _ROOM_H_
#define _ROOM_H_

#include <vector>
#include "Mesh.h"
#include "Sprite.h"
#include "RoomData.h"

typedef enum {
    RoomFlagUnderWater = (1 << 0)
} RoomFlags;

class Room {
  public:
    Room(float p[3] = nullptr, unsigned int f = 0, unsigned int x = 0, unsigned int z = 0);
    ~Room();

    BoundingBox& getBoundingBox();
    void display(bool alpha);

    bool isWall(unsigned long sector);
    long getSector(float x, float z, float* floor, float* ceiling);
    long getSector(float x, float z);
    void getHeightAtPosition(float x, float* y, float z);
    int getAdjoiningRoom(float x, float y, float z,
                         float x2, float y2, float z2);

    Mesh& getMesh() { return mesh; }

    unsigned int getNumXSectors();
    unsigned int getNumZSectors();
    void getPos(float p[3]);

    void setNumXSectors(unsigned int n);
    void setNumZSectors(unsigned int n);
    void setPos(float p[3]);

    void setFlags(unsigned int f);
    unsigned int getFlags();

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
    unsigned int flags;
    unsigned int numXSectors;
    unsigned int numZSectors;
    float pos[3];

    BoundingBox bbox;
    Mesh mesh;

    std::vector<long> adjacentRooms;
    std::vector<Sprite*> sprites;
    std::vector<StaticModel*> models;
    std::vector<Portal*> portals;
    std::vector<Sector*> sectors;
    std::vector<Light*> lights;

    // Was used for "depth sorting" render list, but never assigned...?!
    //float dist; // Distance to near plane, move to room?
};

#endif

