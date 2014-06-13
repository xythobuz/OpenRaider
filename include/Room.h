/*!
 * \file include/Room.h
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifndef _ROOM_H_
#define _ROOM_H_

#include <vector>
#include "math/math.h"
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

    bool isWall(unsigned int sector);
    int getSector(float x, float z, float *floor, float *ceiling);
    int getSector(float x, float z);
    void getHeightAtPosition(float x, float *y, float z);
    int getAdjoiningRoom(float x, float y, float z,
        float x2, float y2, float z2);

    unsigned int getNumXSectors();
    unsigned int getNumZSectors();
    void getPos(vec3_t p);

    void setFlags(unsigned int f);
    unsigned int getFlags();

    unsigned int sizeAdjacentRooms();
    int getAdjacentRoom(unsigned int index);

    unsigned int sizePortals();
    Portal &getPortal(unsigned int index);

    unsigned int sizeSectors();
    Sector &getSector(unsigned int index);

    unsigned int sizeBox();
    Box &getBox(unsigned int index);

    unsigned int sizeModels();
    StaticModel &getModel(unsigned int index);

    unsigned int sizeLights();
    Light &getLight(unsigned int index);

    unsigned int sizeSprites();
    Sprite &getSprite(unsigned int index);

private:
    void sortModels();

    unsigned int flags;
    unsigned int numXSectors;
    unsigned int numZSectors;
    vec3_t pos;

    BoundingBox bbox;
    Mesh mesh;

    std::vector<int> adjacentRooms;
    std::vector<Sprite *> sprites;
    std::vector<StaticModel *> models;
    std::vector<Portal *> portals;
    std::vector<Box *> boxes;
    std::vector<Sector *> sectors;
    std::vector<Light *> lights;

    // Was used for "depth sorting" render list, but never assigned...?!
    //vec_t dist; // Distance to near plane, move to room?
};

#endif

