/*!
 * \file src/BoundingBox.cpp
 * \brief 3D Axis Aligned Bounding Box
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "World.h"
#include "system/Shader.h"
#include "RoomData.h"

#include <glbinding/gl/gl.h>

std::vector<glm::vec3> BoundingBox::vertices, BoundingBox::colorsLine, BoundingBox::colorsPoint;
std::vector<unsigned short> BoundingBox::indicesLine;

BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max) {
    corner[0] = min;
    corner[1] = glm::vec3(max.x, min.y, min.z);
    corner[2] = glm::vec3(min.x, max.y, min.z);
    corner[3] = glm::vec3(min.x, min.y, max.z);
    corner[4] = glm::vec3(max.x, max.y, min.z);
    corner[5] = glm::vec3(min.x, max.y, max.z);
    corner[6] = glm::vec3(max.x, min.y, max.z);
    corner[7] = max;
}

bool BoundingBox::inBox(glm::vec3 p) {
    return ((p.y >= corner[0].y) && (p.y <= corner[7].y) && inBoxPlane(p));
}

bool BoundingBox::inBoxPlane(glm::vec3 p) {
    return ((p.x >= corner[0].x) && (p.x <= corner[7].x) && (p.z >= corner[0].z) && (p.z <= corner[7].z));
}

void BoundingBox::display(glm::mat4 VP, glm::vec3 colorLine, glm::vec3 colorDot) {
    auto startSize = vertices.size();

    for (auto& c : corner) {
        vertices.emplace_back(VP * glm::vec4(c, 1.0f));
        colorsLine.emplace_back(colorLine);
        colorsPoint.emplace_back(colorDot);
    }

    indicesLine.emplace_back(startSize);
    indicesLine.emplace_back(startSize + 2);
    indicesLine.emplace_back(startSize + 2);
    indicesLine.emplace_back(startSize + 4);
    indicesLine.emplace_back(startSize + 4);
    indicesLine.emplace_back(startSize + 1);
    indicesLine.emplace_back(startSize + 1);
    indicesLine.emplace_back(startSize + 6);
    indicesLine.emplace_back(startSize + 6);
    indicesLine.emplace_back(startSize + 7);
    indicesLine.emplace_back(startSize + 7);
    indicesLine.emplace_back(startSize + 5);
    indicesLine.emplace_back(startSize + 5);
    indicesLine.emplace_back(startSize + 3);
    indicesLine.emplace_back(startSize + 3);
    indicesLine.emplace_back(startSize);
    indicesLine.emplace_back(startSize);
    indicesLine.emplace_back(startSize + 1);
    indicesLine.emplace_back(startSize + 1);
    indicesLine.emplace_back(startSize + 4);
    indicesLine.emplace_back(startSize + 4);
    indicesLine.emplace_back(startSize + 7);
    indicesLine.emplace_back(startSize + 7);
    indicesLine.emplace_back(startSize + 6);
    indicesLine.emplace_back(startSize + 6);
    indicesLine.emplace_back(startSize + 3);
    indicesLine.emplace_back(startSize + 3);
    indicesLine.emplace_back(startSize + 5);
    indicesLine.emplace_back(startSize + 5);
    indicesLine.emplace_back(startSize + 2);
}

void BoundingBox::display() {
    if (vertices.size() > 0) {
        Shader::drawGL(vertices, colorsLine, indicesLine, glm::mat4(1.0f), gl::GL_LINES);
        Shader::drawGL(vertices, colorsPoint, glm::mat4(1.0f), gl::GL_POINTS);

        vertices.clear();
        colorsLine.clear();
        colorsPoint.clear();
        indicesLine.clear();
    }
}

