/*!
 * \file src/RoomData.cpp
 * \brief World Room Mesh
 *
 * \author xythobuz
 */

#include <glm/gtc/matrix_transform.hpp>

#include "global.h"
#include "SkeletalModel.h"
#include "World.h"
#include "system/Window.h"
#include "RoomData.h"

void BoundingBox::display(glm::mat4 VP, glm::vec3 color) {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> cols;
    std::vector<unsigned short> inds;

    for (int i = 0; i < 8; i++) {
        verts.push_back(corner[i]);
        cols.push_back(color);
    }

    inds.push_back(0);
    inds.push_back(2);
    inds.push_back(4);
    inds.push_back(1);
    inds.push_back(6);
    inds.push_back(7);
    inds.push_back(5);
    inds.push_back(3);
    inds.push_back(0);
    inds.push_back(1);
    inds.push_back(4);
    inds.push_back(7);
    inds.push_back(6);
    inds.push_back(3);
    inds.push_back(5);
    inds.push_back(2);

    Window::drawLinesGL(verts, cols, inds, VP);
}

// ----------------------------------------------------------------------------

void StaticModel::display(glm::mat4 view, glm::mat4 projection) {
    if (cache < 0) {
        for (int i = 0; i < getWorld().sizeStaticMesh(); i++) {
            if (getWorld().getStaticMesh(i).getID() == id) {
                cache = i;
            }
        }
        assert(cache >= 0);
    }

    glm::mat4 model = glm::rotate(glm::translate(glm::mat4(1.0f), pos),
                                  angle, glm::vec3(0.0f, 1.0f, 0.0f));
    getWorld().getStaticMesh(cache).display(model, view, projection);
}

// ----------------------------------------------------------------------------

void Light::getPos(float p[4]) {
    p[0] = pos[0];
    p[1] = pos[1];
    p[2] = pos[2];
    p[3] = pos[3];
}

void Light::getDir(float d[3]) {
    d[0] = dir[0];
    d[1] = dir[1];
    d[2] = dir[2];
}

float Light::getAtt() {
    return att;
}

void Light::getColor(float c[4]) {
    c[0] = color[0];
    c[1] = color[1];
    c[2] = color[2];
    c[3] = color[3];
}

float Light::getCutoff() {
    return cutoff;
}

Light::LightType Light::getType() {
    return type;
}

// ----------------------------------------------------------------------------

float Sector::getFloor() {
    return floor;
}

float Sector::getCeiling() {
    return ceiling;
}

bool Sector::isWall() {
    return wall;
}

