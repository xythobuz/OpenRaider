/*!
 * \file include/RoomData.h
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifndef _ROOM_DATA_H_
#define _ROOM_DATA_H_

#include <vector>
#include <memory>
#include "math/math.h"
#include "math/Matrix.h"
#include "TombRaider.h"

class BoundingBox {
public:
    BoundingBox();
    void getBoundingBox(vec3_t box[2]);
    void setBoundingBox(vec3_t min, vec3_t max);
    void display(bool points, const vec4_t c1, const vec4_t c2);
    bool inBox(vec_t x, vec_t y, vec_t z);
    bool inBoxPlane(vec_t x, vec_t z);

private:
    vec3_t a, b;
};

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

#endif

