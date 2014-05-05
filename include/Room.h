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

class Light {
public:
    /*!
     * \brief Type a light can be of
     */
    typedef enum {
        typePoint       = 1, //!< Point light
        typeSpot        = 2, //!< Spot light
        typeDirectional = 3  //!< Directional light
    } LightType;

    Light(TombRaider &tr, unsigned int room, unsigned int index);

    void getPos(vec4_t p);
    void getDir(vec3_t d);
    vec_t getAtt();
    void getColor(vec4_t c);
    vec_t getCutoff();
    LightType getType();

private:
    vec4_t pos; //! Light position in 3 space
    vec3_t dir; //! Light direction
    vec_t att;
    vec4_t color; //! Color of light
    vec_t cutoff; //! Fade out distance
    LightType type; //! Type of light
};

class StaticModel {
public:
    StaticModel(TombRaider &tr, unsigned int room, unsigned int i);
    void display();

    // Compares distance to ViewVolume for depth sorting
    bool operator<(const StaticModel &other);

private:
    int index;
    vec_t yaw;
    vec3_t pos;

    // ?
    //vec3_t bbox[2];
};

class Portal {
public:
    Portal(TombRaider &tr, unsigned int room, unsigned int index, Matrix &transform);

    void getVertices(vec3_t vert[4]);
    int getAdjoiningRoom();

private:
    vec3_t vertices[4];
    vec3_t normal;
    int adjoiningRoom;
};

class Box {
public:
    Box(TombRaider &tr, unsigned int room, unsigned int index);

private:
    vec3_t a;
    vec3_t b;
    vec3_t c;
    vec3_t d;
};

class Sector {
public:
    Sector(TombRaider &tr, unsigned int room, unsigned int index);
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

