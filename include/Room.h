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
#include "TombRaider.h"
#include "RoomData.h"

typedef enum {
    RoomFlagUnderWater = (1 << 0)
} RoomFlags;

class Room {
public:
    Room(TombRaider &tr, unsigned int index);
    ~Room();

    BoundingBox &getBoundingBox();
    Mesh &getMesh();
    void display(bool alpha);

    bool isWall(unsigned long sector);
    long getSector(float x, float z, float *floor, float *ceiling);
    long getSector(float x, float z);
    void getHeightAtPosition(float x, float *y, float z);
    int getAdjoiningRoom(float x, float y, float z,
        float x2, float y2, float z2);

    unsigned int getNumXSectors();
    unsigned int getNumZSectors();
    void getPos(float p[3]);

    void setFlags(unsigned int f);
    unsigned int getFlags();

    unsigned long sizeAdjacentRooms();
    long getAdjacentRoom(unsigned long index);

    unsigned long sizePortals();
    Portal &getPortal(unsigned long index);

    unsigned long sizeSectors();
    Sector &getSector(unsigned long index);

    unsigned long sizeBox();
    Box &getBox(unsigned long index);

    unsigned long sizeModels();
    StaticModel &getModel(unsigned long index);

    unsigned long sizeLights();
    Light &getLight(unsigned long index);

    unsigned long sizeSprites();
    Sprite &getSprite(unsigned long index);

private:
    void sortModels();

    unsigned int flags;
    unsigned int numXSectors;
    unsigned int numZSectors;
    float pos[3];

    BoundingBox bbox;
    Mesh mesh;

    std::vector<long> adjacentRooms;
    std::vector<Sprite *> sprites;
    std::vector<StaticModel *> models;
    std::vector<Portal *> portals;
    std::vector<Box *> boxes;
    std::vector<Sector *> sectors;
    std::vector<Light *> lights;

    // Was used for "depth sorting" render list, but never assigned...?!
    //float dist; // Distance to near plane, move to room?
};

#endif

