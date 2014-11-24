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
#include "math/Matrix.h"
#include "TombRaider.h"

class BoundingBox {
  public:
    BoundingBox();
    void getBoundingBox(float box[2][3]);
    void setBoundingBox(float min[3], float max[3]);
    void display(bool points, const unsigned char c1[4], const unsigned char c2[4]);
    bool inBox(float x, float y, float z);
    bool inBoxPlane(float x, float z);

  private:
    float a[3], b[3];
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

    Light(TombRaider& tr, unsigned int room, unsigned int index);

    void getPos(float p[4]);
    void getDir(float d[3]);
    float getAtt();
    void getColor(float c[4]);
    float getCutoff();
    LightType getType();

  private:
    float pos[4]; //! Light position in 3 space
    float dir[3]; //! Light direction
    float att;
    float color[4]; //! Color of light
    float cutoff; //! Fade out distance
    LightType type; //! Type of light
};

class StaticModel {
  public:
    StaticModel(TombRaider& tr, unsigned int room, unsigned int i);
    void display();

    // Compares distance to ViewVolume for depth sorting
    bool operator<(const StaticModel& other);
    static bool compare(StaticModel* a, StaticModel* b);

  private:
    int index;
    float yaw;
    float pos[3];

    // ?
    //float bbox[2][3];
};

class Portal {
  public:
    Portal(float vert[4][3], float norm[3], int adj);
    Portal(TombRaider& tr, unsigned int room, unsigned int index, Matrix& transform);

    void getVertices(float vert[4][3]);
    int getAdjoiningRoom();

  private:
    float vertices[4][3];
    float normal[3];
    int adjoiningRoom;
};

class Box {
  public:
    Box(TombRaider& tr, unsigned int room, unsigned int index);

  private:
    float a[3], b[3], c[3], d[3];
};

class Sector {
  public:
    Sector(TombRaider& tr, unsigned int room, unsigned int index);
    float getFloor();
    float getCeiling();
    bool isWall();

  private:
    float floor;
    float ceiling;
    bool wall;
};

#endif

