/*!
 * \file include/Room.h
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifndef _ROOM_H_
#define _ROOM_H_

#include <vector>
#include <memory>
#include "math/math.h"
#include "Sprite.h"

class StaticModel {
public:
    StaticModel(int _index, vec_t _yaw, vec3_t _pos);

private:
    int index;
    vec_t yaw;
    vec3_t pos;

    // ?
    //vec3_t bbox[2];
};

class Portal {
public:
    Portal(vec3_t _vertices[4], vec3_t _normal, int _adjoiningRoom);

    vec3_t *getVertices();
    int getAdjoiningRoom();

private:
    vec3_t vertices[4];
    vec3_t normal;
    int adjoiningRoom;
};

class Box {
public:
    Box(vec3_t _a, vec3_t _b, vec3_t _c, vec3_t _d);

private:
    vec3_t a;
    vec3_t b;
    vec3_t c;
    vec3_t d;
};

class Sector {
public:
    Sector(vec_t _floor, vec_t _ceiling, bool _wall);
    vec_t getFloor();
    vec_t getCeiling();
    bool isWall();

private:
    vec_t floor;
    vec_t ceiling;
    bool wall;
};

typedef enum {
    RoomFlagUnderWater = (1 << 0)
} RoomFlags;

class Room {
public:
    Room(int _id);
    ~Room();

    unsigned int getFlags();
    unsigned int getNumXSectors();
    unsigned int getNumZSectors();
    void getPos(vec_t *target);

    bool inBox(vec_t x, vec_t y, vec_t z);
    bool inBoxPlane(vec_t x, vec_t z);

    unsigned int sizePortals();
    Portal &getPortal(unsigned int index);

    unsigned int sizeSectors();
    Sector &getSector(unsigned int index);

private:
    int id;
    unsigned int flags;
    unsigned int numXSectors;
    unsigned int numZSectors;
    vec3_t pos;
    vec3_t bbox[2];

    std::vector<int> adjacentRooms;
    std::vector<Sprite *> sprites;
    std::vector<StaticModel *> models;
    std::vector<Portal *> portals;
    std::vector<Box *> boxes;
    std::vector<Sector *> sectors;
};

#endif

