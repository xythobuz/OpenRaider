/*!
 * \file include/RoomMesh.h
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifndef _ROOM_MESH_H_
#define _ROOM_MESH_H_

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Mesh.h"

struct RoomVertexTR2 {
    int x, y, z; // Vertex coordinates, relative to x/zOffset
    int light1, light2; // Almost always equal

    // Set of flags for special rendering effects
    // 0x8000 - Something to do with water surface?
    // 0x4000 - Underwater lighting modulation/movement if seen from above
    // 0x2000 - Water/Quicksand surface movement
    // 0x0010 - Normal?
    unsigned int attributes;
};

class RoomMesh {
  public:
    RoomMesh(const std::vector<RoomVertexTR2>& vertices,
             const std::vector<IndexedRectangle>& rectangles,
             const std::vector<IndexedRectangle>& triangles);
    void prepare();
    void display(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

  private:
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> textures;
};

#endif

