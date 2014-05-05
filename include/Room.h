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
#include "math/Matrix.h"
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

    void setFlags(unsigned int f);
    unsigned int getFlags();

    unsigned int getNumXSectors();

    unsigned int getNumZSectors();

    void getPos(vec3_t p);

    void getBoundingBox(vec3_t box[2]);
    bool inBox(vec_t x, vec_t y, vec_t z);
    bool inBoxPlane(vec_t x, vec_t z);

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
    void sortModels();

    unsigned int sizeLights();
    Light &getLight(unsigned int index);

    unsigned int sizeSprites();
    Sprite &getSprite(unsigned int index);

    Mesh &getMesh();

private:
    unsigned int flags;
    unsigned int numXSectors;
    unsigned int numZSectors;
    vec3_t pos;
    vec3_t bbox[2];
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

