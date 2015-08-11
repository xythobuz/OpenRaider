/*!
 * \file src/Mesh.cpp
 * \brief Textured/Colored Mesh
 *
 * \author xythobuz
 */

#include "global.h"
#include "TextureManager.h"
#include "Mesh.h"

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

Mesh::Mesh(const std::vector<glm::vec3>& vert,
           const std::vector<IndexedRectangle>& rect,
           const std::vector<IndexedRectangle>& tri,
           const std::vector<IndexedColoredRectangle>& coloredRect,
           const std::vector<IndexedColoredRectangle>& coloredTri) {
    for (auto& t : rect) {
        indicesBuff.push_back(0);
        verticesBuff.emplace_back(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z);
        verticesBuff.emplace_back(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z);
        verticesBuff.emplace_back(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z);
        verticesBuff.emplace_back(vert.at(t.v4).x, vert.at(t.v4).y, vert.at(t.v4).z);
        texturesBuff.push_back(t.texture);
    }

    for (auto& t : tri) {
        indicesBuff.push_back(1);
        verticesBuff.emplace_back(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z);
        verticesBuff.emplace_back(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z);
        verticesBuff.emplace_back(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z);
        texturesBuff.push_back(t.texture);
    }

    for (auto& t : coloredRect) {
        indicesColorBuff.push_back(0);
        verticesColorBuff.emplace_back(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z);
        verticesColorBuff.emplace_back(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z);
        verticesColorBuff.emplace_back(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z);
        verticesColorBuff.emplace_back(vert.at(t.v4).x, vert.at(t.v4).y, vert.at(t.v4).z);
        colorsIndexBuff.push_back(t.index);
    }

    for (auto& t : coloredTri) {
        indicesColorBuff.push_back(1);
        verticesColorBuff.emplace_back(vert.at(t.v1).x, vert.at(t.v1).y, vert.at(t.v1).z);
        verticesColorBuff.emplace_back(vert.at(t.v2).x, vert.at(t.v2).y, vert.at(t.v2).z);
        verticesColorBuff.emplace_back(vert.at(t.v3).x, vert.at(t.v3).y, vert.at(t.v3).z);
        colorsIndexBuff.push_back(t.index);
    }
}

void Mesh::prepare() {
    std::vector<unsigned short> ind;
    std::vector<glm::vec3> vert;
    std::vector<glm::vec2> uvBuff;
    std::vector<unsigned int> tex;

    glm::vec3 average(0.0f, 0.0f, 0.0f);
    unsigned long averageCount = 0;

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
            uvBuff.push_back(TextureManager::getTile(texturesBuff.at(i)).getUV(v));
            tex.push_back(texture);

            // Calculate quick-n-dirty center point of mesh
            average += verticesBuff.at(vertIndex + v);
            averageCount++;
        }

        if (indicesBuff.at(i) == 0) {
            ind.push_back(ind.at(ind.size() - 4));
            ind.push_back(ind.at(ind.size() - 3));
        }

        vertIndex += (indicesBuff.at(i) == 0) ? 4 : 3;
    }

    orAssertEqual(ind.size() % 3, 0);
    orAssertEqual(vert.size(), tex.size());
    orAssertEqual(vert.size(), uvBuff.size());

    indicesBuff = std::move(ind);
    verticesBuff = std::move(vert);
    uvsBuff = std::move(uvBuff);
    texturesBuff = std::move(tex);

    std::vector<unsigned short> indCol;
    std::vector<glm::vec3> vertCol;
    std::vector<glm::vec3> cols;

    vertIndex = 0;
    for (int i = 0; i < indicesColorBuff.size(); i++) {
        for (int x = 0; x < ((indicesColorBuff.at(i) == 0) ? 4 : 3); x++) {
            int v = x;
            if (v == 0)
                v = 2;
            else if (v == 2)
                v = 0;
            indCol.push_back(vertCol.size());
            vertCol.push_back(verticesColorBuff.at(vertIndex + v));
            glm::vec4 c = TextureManager::getPalette(colorsIndexBuff.at(i));
            cols.push_back(glm::vec3(c.x, c.y, c.z));

            average += verticesColorBuff.at(vertIndex + v);
            averageCount++;
        }

        if (indicesColorBuff.at(i) == 0) {
            indCol.push_back(indCol.at(indCol.size() - 4));
            indCol.push_back(indCol.at(indCol.size() - 3));
        }

        vertIndex += (indicesColorBuff.at(i) == 0) ? 4 : 3;
    }

    orAssertEqual(indCol.size() % 3, 0);
    orAssertEqual(vertCol.size(), cols.size());

    indicesColorBuff = std::move(indCol);
    verticesColorBuff = std::move(vertCol);
    colorsBuff = std::move(cols);

    center = average / float(averageCount);
    radius = 0.0f;
    for (auto& vert : verticesBuff) {
        float dist = glm::distance(center, vert);
        if (dist > radius) radius = dist;
    }
    for (auto& vert : verticesColorBuff) {
        float dist = glm::distance(center, vert);
        if (dist > radius) radius = dist;
    }
}

void Mesh::display(glm::mat4 MVP, ShaderTexture* shaderTexture) {
    if (indicesBuff.size() > 0) {
        Shader::drawGLBuffer(verticesBuff, uvsBuff);

        for (int i = 0; i < TextureManager::numTextures(TextureStorage::GAME); i++) {
            std::vector<unsigned short> indices;
            for (int n = 0; n < indicesBuff.size(); n++) {
                if (texturesBuff.at(indicesBuff.at(n)) == i) {
                    indices.push_back(indicesBuff.at(n));
                }
            }

            Shader::drawGLOnly(indices, MVP, i, TextureStorage::GAME,
                               gl::GL_TRIANGLES, shaderTexture);
        }
    }

    if (indicesColorBuff.size() > 0)
        Shader::drawGL(verticesColorBuff, colorsBuff, indicesColorBuff, MVP, gl::GL_TRIANGLES,
                       shaderTexture);
}

