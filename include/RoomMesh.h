/*!
 * \file include/RoomMesh.h
 * \brief Room Mesh Geometry
 *
 * \author xythobuz
 */

#ifndef _ROOM_MESH_H_
#define _ROOM_MESH_H_

#include <vector>

#include "Mesh.h"
#include "system/Shader.h"

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
    void display(glm::mat4 MVP);

  private:
    std::vector<unsigned short> indicesBuff;
    std::vector<glm::vec3> verticesBuff;
    std::vector<unsigned int> texturesBuff;
    ShaderBuffer indices, vertices, uvs;
};

#endif

