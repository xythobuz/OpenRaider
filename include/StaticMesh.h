/*!
 * \file include/StaticMesh.h
 * \brief Static Model Meshes
 *
 * \author xythobuz
 */

#ifndef _STATIC_MODEL_H_
#define _STATIC_MODEL_H_

class StaticMesh {
  public:
    StaticMesh(int i, int m) : id(i), mesh(m) { }
    void display(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    int getID() { return id; }

  private:
    int id;
    int mesh;
};

#endif

