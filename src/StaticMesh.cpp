/*!
 * \file src/StaticMesh.cpp
 * \brief Static Model Meshes
 *
 * \author xythobuz
 */

#include "global.h"
#include "World.h"
#include "StaticMesh.h"

void StaticMesh::display(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    getWorld().getMesh(mesh).display(model, view, projection);
}

