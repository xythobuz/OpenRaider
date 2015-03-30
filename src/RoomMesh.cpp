/*!
 * \file src/RoomMesh.cpp
 * \brief Room Mesh Geometry
 *
 * \author xythobuz
 */

#include "global.h"
#include "TextureManager.h"
#include "system/Shader.h"
#include "RoomMesh.h"

RoomMesh::RoomMesh(const std::vector<RoomVertexTR2>& vert,
                   const std::vector<IndexedRectangle>& rect,
                   const std::vector<IndexedRectangle>& tri) {
    for (auto& t : rect) {
        indicesBuff.push_back(0);
        verticesBuff.push_back(glm::vec3(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z));
        verticesBuff.push_back(glm::vec3(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z));
        verticesBuff.push_back(glm::vec3(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z));
        verticesBuff.push_back(glm::vec3(vert.at(t.v4).x, vert.at(t.v4).y, vert.at(t.v4).z));
        texturesBuff.push_back(t.texture);
    }

    for (auto& t : tri) {
        indicesBuff.push_back(1);
        verticesBuff.push_back(glm::vec3(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z));
        verticesBuff.push_back(glm::vec3(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z));
        verticesBuff.push_back(glm::vec3(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z));
        texturesBuff.push_back(t.texture);
    }
}

void RoomMesh::prepare() {
    std::vector<unsigned short> ind;
    std::vector<glm::vec3> vert;
    std::vector<unsigned int> tex;

    int vertIndex = 0;
    for (int i = 0; i < indicesBuff.size(); i++) {
        unsigned int texture = TextureManager::getTile(texturesBuff.at(i)).getTexture();
        for (int x = 0; x < ((indicesBuff.at(i) == 0) ? 4 : 3); x++) {
            int v = x;
            if (v == 0)
                v = 2;
            else if (v == 2)
                v = 0;
            ind.push_back(vert.size());
            vert.push_back(verticesBuff.at(vertIndex + v));
            uvsBuff.push_back(TextureManager::getTile(texturesBuff.at(i)).getUV(v));
            tex.push_back(texture);
        }

        if (indicesBuff.at(i) == 0) {
            ind.push_back(ind.at(ind.size() - 4));
            ind.push_back(ind.at(ind.size() - 3));
        }

        vertIndex += (indicesBuff.at(i) == 0) ? 4 : 3;
    }

    orAssertEqual(ind.size() % 3, 0);
    orAssertEqual(vert.size(), tex.size());
    orAssertEqual(vert.size(), uvsBuff.size());

    indicesBuff = std::move(ind);
    verticesBuff = std::move(vert);
    texturesBuff = std::move(tex);
}

void RoomMesh::display(glm::mat4 MVP) {
    if (indicesBuff.size() > 0) {
        Shader::drawGLBuffer(verticesBuff, uvsBuff);

        for (int i = 0; i < TextureManager::numTextures(TextureStorage::GAME); i++) {
            std::vector<unsigned short> indices;
            for (int n = 0; n < indicesBuff.size(); n++) {
                if (texturesBuff.at(indicesBuff.at(n)) == i) {
                    indices.push_back(indicesBuff.at(n));
                }
            }

            Shader::drawGLOnly(indices, MVP, i, TextureStorage::GAME);
        }
    }
}

