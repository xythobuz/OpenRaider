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

#include <glm/gtc/matrix_transform.hpp>

StaticModel::StaticModel(glm::vec3 pos, float angle, int i) : id(i), cache(-1) {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), pos);
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
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), pos);

    //! \fixme Calculate angle between camera and sprite
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), Camera::getRotation().x,
                                   glm::vec3(0.0f, 1.0f, 0.0f));

    World::getSprite(sprite).display(VP * (translate * rotate));
}

// ----------------------------------------------------------------------------

bool Portal::showBoundingBox = false;

Portal::Portal(int adj, glm::vec3 n, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
               glm::vec3 v4) : adjoiningRoom(adj), normal(n), bbox(v1, v3),
                               bboxNormal(v1 + ((v3 - v1) / 2.0f),
                                          v1 + ((v3 - v1) / 2.0f)
                                             + (normal * 1024.0f)) {
    vert[0] = v1; vert[1] = v2;
    vert[2] = v3; vert[3] = v4;
}

void Portal::display(glm::mat4 VP) {
    if (showBoundingBox) {
        bbox.display(VP, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        bboxNormal.display(VP, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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

