/*!
 * \file include/RoomData.h
 * \brief Auxiliary Room classes
 *
 * \author xythobuz
 */

#ifndef _ROOM_DATA_H_
#define _ROOM_DATA_H_

#include <memory>
#include <vector>

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
        orAssertGreaterThanEqual(i, 0);
        orAssertLessThan(i, 8);
        return corner[i];
    }

    void display(glm::mat4 VP, glm::vec3 colorLine, glm::vec3 colorDot);

  private:
    glm::vec3 corner[8];
};

// --------------------------------------

class StaticModel {
  public:
    StaticModel(glm::vec3 pos, float angle, int i);
    void display(glm::mat4 VP);

  private:
    int id;
    int cache;
    glm::mat4 model;
};

// --------------------------------------

class RoomSprite {
  public:
    RoomSprite(glm::vec3 p, int s) : pos(p), sprite(s) { }
    void display(glm::mat4 VP);

  private:
    glm::vec3 pos;
    int sprite;
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

    glm::vec3 getVertex(int i) {
        orAssertGreaterThanEqual(i, 0);
        orAssertLessThan(i, 4);
        return vert[i];
    }

  private:
    int adjoiningRoom;
    glm::vec3 normal;
    glm::vec3 vert[4];
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

