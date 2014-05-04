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
#include "Mesh.h"
#include "Sprite.h"

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

    Light(vec4_t pos, vec3_t dir, vec_t att, vec4_t color, vec_t cutoff, LightType type);

//private:
    vec4_t mPos; //! Light position in 3 space
    vec3_t mDir; //! Light direction
    vec_t mAtt;
    vec4_t mColor; //! Color of light
    vec_t mCutoff; //! Fade out distance
    LightType mType; //! Type of light
};

class StaticModel {
public:
    StaticModel(int _index, vec_t _yaw, vec3_t _pos);
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
    Portal(vec3_t _vertices[4], vec3_t _normal, int _adjoiningRoom);

    void getVertices(vec3_t vert[4]);
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

    void setFlags(unsigned int f);
    unsigned int getFlags();

    void setNumXSectors(unsigned int n);
    unsigned int getNumXSectors();

    void setNumZSectors(unsigned int n);
    unsigned int getNumZSectors();

    void setPos(vec3_t p);
    void getPos(vec3_t p);

    void getBoundingBox(vec3_t box[2]);
    void setBoundingBox(vec3_t box[2]);
    bool inBox(vec_t x, vec_t y, vec_t z);
    bool inBoxPlane(vec_t x, vec_t z);

    unsigned int sizeAdjacentRooms();
    int getAdjacentRoom(unsigned int index);
    void addAdjacentRoom(int r);

    unsigned int sizePortals();
    Portal &getPortal(unsigned int index);
    void addPortal(Portal &p);

    unsigned int sizeSectors();
    Sector &getSector(unsigned int index);
    void addSector(Sector &s);

    unsigned int sizeBox();
    Box &getBox(unsigned int index);
    void addBox(Box &b);

    unsigned int sizeModels();
    StaticModel &getModel(unsigned int index);
    void addModel(StaticModel &m);
    void sortModels();

    unsigned int sizeLights();
    Light &getLight(unsigned int index);
    void addLight(Light &l);

    unsigned int sizeSprites();
    Sprite &getSprite(unsigned int index);
    void addSprite(Sprite &s);

    Mesh &getMesh();

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

    // Was previously stored in RenderRoom
    //vec_t dist;                  //!< Distance to near plane, move to room?
    std::vector<Light *> lights; //!< List of lights in this room
    Mesh mesh;                   //!< OpenGL mesh that represents this room
};

#endif

