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
    BoundingBox(glm::vec3 min, glm::vec3 max) {
        corner[0] = glm::vec3(min.x, min.y, min.z);
        corner[1] = glm::vec3(max.x, min.y, min.z);
        corner[2] = glm::vec3(min.x, max.y, min.z);
        corner[3] = glm::vec3(min.x, min.y, max.z);
        corner[4] = glm::vec3(max.x, max.y, min.z);
        corner[5] = glm::vec3(min.x, max.y, max.z);
        corner[6] = glm::vec3(max.x, min.y, max.z);
        corner[7] = glm::vec3(max.x, max.y, max.z);
    }

    bool inBox(glm::vec3 p) {
        return ((p.y >= corner[0].y) && (p.y <= corner[7].y)
                && inBoxPlane(p));
    }

    bool inBoxPlane(glm::vec3 p) {
        return ((p.x >= corner[0].x) && (p.x <= corner[7].x)
                && (p.z >= corner[0].z) && (p.z <= corner[7].z));
    }

    glm::vec3 getCorner(int i) {
        assert((i >= 0) && (i < 8));
        return corner[i];
    }

    void display(glm::mat4 VP);

  private:
    glm::vec3 corner[8];
};

// --------------------------------------

class StaticModel {
  public:
    StaticModel(glm::vec3 p, float a, int i) : pos(p), angle(a), id(i), cache(-1) { }
    void display(glm::mat4 view, glm::mat4 projection);

  private:
    glm::vec3 pos;
    float angle;
    int id;
    int cache;
};

// --------------------------------------

class Portal {
  public:
    Portal(int adj, glm::vec3 n, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
           glm::vec3 v4) : adjoiningRoom(adj), normal(n) {
        vert[0] = v1; vert[1] = v2;
        vert[2] = v3; vert[3] = v4;
    }
    int getAdjoiningRoom() { return adjoiningRoom; }
    glm::vec3 getNormal() { return normal; }
    glm::vec3 getVertex(int i) { assert((i >= 0) && (i < 4)); return vert[i]; }

  private:
    int adjoiningRoom;
    glm::vec3 normal;
    glm::vec3 vert[4];
};

// --------------------------------------

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

// --------------------------------------

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

