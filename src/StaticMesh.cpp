/*!
 * \file src/StaticMesh.cpp
 * \brief Static Model Meshes
 *
 * \author xythobuz
 */

#include "global.h"
#include "World.h"
#include "StaticMesh.h"

bool StaticMesh::showBoundingBox = false;

void StaticMesh::display(glm::mat4 MVP) {
    getWorld().getMesh(mesh).display(MVP);

    if (showBoundingBox) {
        bbox1->display(MVP, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        bbox2->display(MVP, glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

