/*!
 * \file include/RoomData.h
 * \brief Auxiliary Room classes
 *
 * \author xythobuz
 */

#ifndef _ROOM_DATA_H_
#define _ROOM_DATA_H_

#include "BoundingBox.h"

class StaticModel {
  public:
    StaticModel(glm::vec3 pos, float angle, int i);
    void display(glm::mat4 VP);
    void displayUI();

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
    Portal(int adj, glm::vec3 n,
           glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);

    int getAdjoiningRoom() { return adjoiningRoom; }
    glm::vec3 getNormal() { return normal; }
    BoundingBox getBoundingBox() { return bbox; }

    void display(glm::mat4 VP);
    void displayUI();

    glm::vec3 getVertex(int i) {
        orAssertGreaterThanEqual(i, 0);
        orAssertLessThan(i, 4);
        return vert[i];
    }

    static void setShowBoundingBox(bool s) { showBoundingBox = s; }
    static bool getShowBoundingBox() { return showBoundingBox; }

  private:
    int adjoiningRoom;
    glm::vec3 normal;
    glm::vec3 vert[4];
    BoundingBox bbox, bboxNormal;

    static bool showBoundingBox;
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

