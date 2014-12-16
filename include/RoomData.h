/*!
 * \file include/RoomData.h
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifndef _ROOM_DATA_H_
#define _ROOM_DATA_H_

#include <memory>
#include <vector>
#include <glm/vec3.hpp>

class BoundingBox {
  public:
    BoundingBox(glm::vec3 min, glm::vec3 max);
    void display(bool points, const unsigned char c1[4], const unsigned char c2[4]);
    bool inBox(float x, float y, float z);
    bool inBoxPlane(float x, float z);

  private:
    glm::vec3 a, b;
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
    void display();

  private:
    int index;
    float yaw;
    float pos[3];

    // ?
    //float bbox[2][3];
};

class Portal {
  public:
    Portal(glm::vec3 vert[4], float norm[3], int adj);

    void getVertices(float vert[4][3]);
    int getAdjoiningRoom();

  private:
    float vertices[4][3];
    float normal[3];
    int adjoiningRoom;
};

class Sector {
  public:
    Sector(float f, float c, bool w) : floor(f), ceiling(c), wall(w) { }
    float getFloor();
    float getCeiling();
    bool isWall();

  private:
    float floor;
    float ceiling;
    bool wall;
};

#endif

