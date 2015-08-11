/*!
 * \file src/Selector.cpp
 * \brief Selector Window
 *
 * http://antongerdelan.net/opengl/raycasting.html
 *
 * \author xythobuz
 */

#include "imgui/imgui.h"

#include "global.h"
#include "Camera.h"
#include "Log.h"
#include "World.h"
#include "system/Window.h"
#include "Selector.h"

#include <glm/gtx/intersect.hpp>

bool Selector::visible = false;

static int lastX = -1, lastY = -1;
static bool workToDo = false;
static float grabSphere = 102.4f;
static bool clickOnGeometry = false, clickOnRoomModels = true, clickOnRoomSprites = true;
static bool clickOnSprites = true, clickOnMeshes = false, clickOnModels = false;

void Selector::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    if ((button == leftmouseKey) && (!released)) {
        lastX = x;
        lastY = y;

        if (workToDo) {
            Log::get(LOG_DEBUG) << "Selector missed mouse click event!" << Log::endl;
        }

        workToDo = true;
    }
}

void Selector::display() {
    if (!visible)
        return;

    if (!ImGui::Begin("Object Selector", &visible, ImVec2(500, 200))) {
        ImGui::End();
        return;
    }

    static glm::vec3 rayWorld;
    static glm::vec3 lastIntersectPos, lastIntersectNorm;
    static unsigned long lastRoom, lastModel;
    static bool foundSomething = false;

    if (workToDo) {
        // Calculate click ray
        glm::vec2 normalized = glm::vec2((2.0f * lastX) / Window::getSize().x - 1.0f,
                                         1.0f - (2.0f * lastY) / Window::getSize().y);
        glm::vec4 rayClip(normalized.x, normalized.y, -1.0f, 1.0f);
        glm::vec4 rayEye(glm::inverse(Camera::getProjectionMatrix()) * rayClip);
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        rayWorld = glm::vec3(glm::inverse(Camera::getViewMatrix()) * rayEye);
        rayWorld = glm::normalize(rayWorld);
        workToDo = false;

        // Check for any intersections with object bounding spheres
        if (clickOnModels) {

        }

        if (clickOnMeshes) {

        }

        if (clickOnSprites) {

        }

        if (clickOnRoomModels) {
            for (unsigned long i = 0; i < World::sizeRoom(); i++) {
                Room& r = World::getRoom(i);
                for (unsigned long j = 0; j < r.sizeModels(); j++) {
                    StaticModel& sm = r.getModel(j);
                    glm::vec3 pos, norm;
                    if (glm::intersectRaySphere(Camera::getPosition(), rayWorld, sm.getCenter(), sm.getRadius(),
                                                pos, norm)) {
                        //! \fixme This is not enough. Should be depth sorted?!
                        lastRoom = i;
                        lastModel = j;
                        lastIntersectPos = pos;
                        lastIntersectNorm = norm;
                        foundSomething = true;
                    }
                }
            }
        }

        if (clickOnRoomSprites) {

        }

        if (clickOnGeometry) {

        }
    }

    ImGui::SliderFloat("Grab Sphere", &grabSphere, 0.1f, 10240.0f);
    ImGui::Checkbox("Geometry", &clickOnGeometry);
    ImGui::SameLine();
    ImGui::Checkbox("RoomModels", &clickOnRoomModels);
    ImGui::SameLine();
    ImGui::Checkbox("RoomSprites", &clickOnRoomSprites);
    ImGui::Checkbox("Sprites", &clickOnSprites);
    ImGui::SameLine();
    ImGui::Checkbox("Meshes", &clickOnMeshes);
    ImGui::SameLine();
    ImGui::Checkbox("Models", &clickOnModels);
    ImGui::SameLine();
    if (ImGui::Button("Hide Selector")) {
        visible = false;
    }
    ImGui::Separator();

    // Not yet implemented!
    clickOnModels = false;
    clickOnMeshes = false;
    clickOnSprites = false;
    clickOnRoomSprites = false;
    clickOnGeometry = false;

    ImGui::Text("Camera: (%.2f %.2f %.2f)", Camera::getPosition().x, Camera::getPosition().y, Camera::getPosition().z);
    ImGui::Text("Last click: (%d %d)", lastX, lastY);
    if ((lastX >= 0) && (lastY >= 0)) {
        ImGui::Text("Normalized Ray: (%.3f %.3f %.3f)", rayWorld.x, rayWorld.y, rayWorld.z);
    }

    if (foundSomething) {
        ImGui::Text("Intersect Pos: (%.2f %.2f %.2f)", lastIntersectPos.x, lastIntersectPos.y, lastIntersectPos.z);
        ImGui::Text("Intersect Norm: (%.2f %.2f %.2f)", lastIntersectNorm.x, lastIntersectNorm.y, lastIntersectNorm.z);
        ImGui::Text("Last Room: %lu", lastRoom);
        ImGui::Text("Last RoomModel: %lu", lastModel);
    }

    ImGui::End();
}

