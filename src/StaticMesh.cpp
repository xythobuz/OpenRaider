/*!
 * \file src/StaticMesh.cpp
 * \brief Static Model Meshes
 *
 * \author xythobuz
 */

#include "global.h"
#include "World.h"
#include "StaticMesh.h"

#include "imgui/imgui.h"

bool StaticMesh::showBoundingBox = false;

BoundingSphere& StaticMesh::getBoundingSphere() {
    return World::getMesh(mesh).getBoundingSphere();
}

void StaticMesh::display(glm::mat4 MVP) {
    World::getMesh(mesh).display(MVP);

    if (showBoundingBox) {
        bbox1->display(MVP, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        bbox2->display(MVP, glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void StaticMesh::displayUI() {
    ImGui::Text("%03d", id);
    ImGui::NextColumn();
    ImGui::Text("%03d // and two bboxes", mesh);
    ImGui::NextColumn();
}

