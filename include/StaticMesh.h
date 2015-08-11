/*!
 * \file include/StaticMesh.h
 * \brief Static Model Meshes
 *
 * \author xythobuz
 */

#ifndef _STATIC_MODEL_H_
#define _STATIC_MODEL_H_

#include <memory>

#include "BoundingBox.h"
#include "BoundingSphere.h"

class StaticMesh {
  public:
    StaticMesh(int i, int m, BoundingBox* b1, BoundingBox* b2)
        : id(i), mesh(m), bbox1(b1), bbox2(b2) { }
    void display(glm::mat4 MVP);
    void displayUI();

    BoundingSphere& getBoundingSphere();

    int getID() { return id; }

    static void setShowBoundingBox(bool s) { showBoundingBox = s; }
    static bool getShowBoundingBox() { return showBoundingBox; }

  private:
    int id;
    int mesh;
    std::unique_ptr<BoundingBox> bbox1, bbox2;

    static bool showBoundingBox;
};

#endif

