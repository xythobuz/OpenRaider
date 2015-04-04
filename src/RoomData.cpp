/*!
 * \file src/RoomData.cpp
 * \brief Auxiliary Room classes
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "World.h"
#include "system/Shader.h"
#include "RoomData.h"

#include "imgui/imgui.h"

#include <glbinding/gl/gl33.h>
#include <glm/gtc/matrix_transform.hpp>

void BoundingBox::display(glm::mat4 VP, glm::vec3 colorLine, glm::vec3 colorDot) {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> cols;
    std::vector<unsigned short> inds;

    for (int i = 0; i < 8; i++) {
        verts.push_back(corner[i]);
        cols.push_back(colorLine);
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

    Shader::drawGL(verts, cols, inds, VP, gl::GL_LINE_STRIP);

    cols.clear();
    inds.clear();

    for (int i = 0; i < 8; i++) {
        cols.push_back(colorDot);
        inds.push_back(i);
    }

    Shader::drawGL(verts, cols, inds, VP, gl::GL_POINTS);
}

// ----------------------------------------------------------------------------

StaticModel::StaticModel(glm::vec3 pos, float angle, int i) : id(i), cache(-1) {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, -pos.y, pos.z));
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model = translate * rotate;
}

void StaticModel::display(glm::mat4 VP) {
    if (cache < 0) {
        for (int i = 0; i < World::sizeStaticMesh(); i++) {
            if (World::getStaticMesh(i).getID() == id) {
                cache = i;
            }
        }
        orAssertGreaterThanEqual(cache, 0);
    }

    World::getStaticMesh(cache).display(VP * model);
}

void StaticModel::displayUI() {
    ImGui::Text("ID %d; No. %d", id, cache);
}

// ----------------------------------------------------------------------------

void RoomSprite::display(glm::mat4 VP) {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, -pos.y, pos.z));
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), Camera::getRotation().x, glm::vec3(0.0f, 1.0f,
                                   0.0f));
    glm::mat4 model = translate * rotate;

    World::getSprite(sprite).display(VP * model);
}

// ----------------------------------------------------------------------------

bool Portal::showBoundingBox = false;

void Portal::display(glm::mat4 VP) {
    if (showBoundingBox) {
        bbox.display(VP, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void Portal::displayUI() {
    ImGui::Text("To %03d", adjoiningRoom);
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

