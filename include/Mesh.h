/*!
 * \file include/Mesh.h
 * \brief OpenGL Mesh
 *
 * \author Mongoose
 *
 * \todo Unify the parallel systems here, arrays and the allocate/set
 */

#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "loader/LoaderTR2.h"

class Mesh {
  public:
    Mesh(const std::vector<RoomVertexTR2>& vertices,
         const std::vector<RoomRectangleTR2>& rectangles,
         const std::vector<RoomTriangleTR2>& triangles);

    void prepare();

    void display(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

  private:
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> textures;
};

#endif

