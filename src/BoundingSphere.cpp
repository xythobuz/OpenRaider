/*!
 * \file src/BoundingSphere.cpp
 * \brief 3D Axis Aligned Bounding Sphere
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "World.h"
#include "system/Shader.h"
#include "BoundingSphere.h"

#include <glbinding/gl/gl.h>

std::vector<glm::vec4> BoundingSphere::vertices;
std::vector<glm::vec3> BoundingSphere::colors;

void BoundingSphere::display(glm::mat4 VP, glm::vec3 color) {
    for (int w = 0; w < resolution; w++) {
        for (int h = (-resolution / 2); h < (resolution / 2); h++) {
            float inc1 = (w / float(resolution)) * 2.0f * glm::pi<float>();
            float inc2 = ((w + 1) / float(resolution)) * 2.0f * glm::pi<float>();
            float inc3 = (h / float(resolution)) * glm::pi<float>();
            float inc4 = ((h + 1) / float(resolution)) * glm::pi<float>();

            float x1 = glm::sin(inc1);
            float y1 = glm::cos(inc1);
            float x2 = glm::sin(inc2);
            float y2 = glm::cos(inc2);

            float radius1 = radius * glm::cos(inc3);
            float radius2 = radius * glm::cos(inc4);

            float z1 = radius * glm::sin(inc3);
            float z2 = radius * glm::sin(inc4);

            vertices.emplace_back(VP * (glm::vec4(radius1 * x1, z1, radius1 * y1, 1.0f) + glm::vec4(pos,
                                        0.0f)));
            vertices.emplace_back(VP * (glm::vec4(radius1 * x2, z1, radius1 * y2, 1.0f) + glm::vec4(pos,
                                        0.0f)));
            vertices.emplace_back(VP * (glm::vec4(radius2 * x2, z2, radius2 * y2, 1.0f) + glm::vec4(pos,
                                        0.0f)));

            vertices.emplace_back(VP * (glm::vec4(radius1 * x1, z1, radius1 * y1, 1.0f) + glm::vec4(pos,
                                        0.0f)));
            vertices.emplace_back(VP * (glm::vec4(radius2 * x2, z2, radius2 * y2, 1.0f) + glm::vec4(pos,
                                        0.0f)));
            vertices.emplace_back(VP * (glm::vec4(radius2 * x1, z2, radius2 * y1, 1.0f) + glm::vec4(pos,
                                        0.0f)));

            for (int i = 0; i < 6; i++) {
                colors.emplace_back(color);
            }
        }

    }
}

void BoundingSphere::display() {
    if (vertices.size() > 0) {
        Shader::drawGL(vertices, colors, gl::GL_POINTS);
    }

    vertices.clear();
    colors.clear();
}
