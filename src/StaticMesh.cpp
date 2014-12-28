/*!
 * \file src/StaticMesh.cpp
 * \brief Static Model Meshes
 *
 * \author xythobuz
 */

#include "global.h"
#include "Render.h"
#include "World.h"
#include "StaticMesh.h"

void StaticMesh::display(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    getWorld().getMesh(mesh).display(model, view, projection);

    if (Render::getMode() == RenderMode::Wireframe) {
        bbox1->display(projection * view * model,
                       glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        bbox2->display(projection * view * model,
                       glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

