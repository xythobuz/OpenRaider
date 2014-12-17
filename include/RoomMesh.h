/*!
 * \file include/RoomMesh.h
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#ifndef _ROOM_MESH_H_
#define _ROOM_MESH_H_

#include <cstdint>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct RoomVertexTR2 {
    int16_t x, y, z; // Vertex coordinates, relative to x/zOffset
    int16_t light1, light2; // Almost always equal

    // Set of flags for special rendering effects
    // 0x8000 - Something to do with water surface?
    // 0x4000 - Underwater lighting modulation/movement if seen from above
    // 0x2000 - Water/Quicksand surface movement
    // 0x0010 - Normal?
    uint16_t attributes;
};

struct IndexedRectangle {
    unsigned int v1, v2, v3, v4; // Vertex list indices
    unsigned int texture; // Index into object-texture list

    IndexedRectangle(uint16_t t, uint16_t _v1, uint16_t _v2, uint16_t _v3, uint16_t _v4 = 0)
        : v1(_v1), v2(_v2), v3(_v3), v4(_v4), texture(t) { }
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

