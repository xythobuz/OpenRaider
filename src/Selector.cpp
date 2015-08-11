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
WorldObjects Selector::lastClickedObject = WorldObjectCount;

// geometryObject, roomSpriteObject, roomModelObject, spriteObject, meshObject, modelObject, entityObject
std::array<bool, WorldObjectCount> Selector::clickOnObject = {{ false, true, true, false, false, false, false }};

glm::i32vec2 Selector::rayScreen(-1, -1);
glm::vec3 Selector::rayWorld, Selector::lastIntersectPos, Selector::lastIntersectNorm;
unsigned long Selector::lastIndexA, Selector::lastIndexB;

void Selector::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    if ((button == leftmouseKey) && (!released)) {
        // Calculate click ray
        rayScreen = glm::vec2(x, y);
        glm::vec2 normalized = glm::vec2((2.0f * rayScreen.x) / Window::getSize().x - 1.0f,
                                         1.0f - (2.0f * rayScreen.y) / Window::getSize().y);
        glm::vec4 rayClip(normalized.x, normalized.y, -1.0f, 1.0f);
        glm::vec4 rayEye(glm::inverse(Camera::getProjectionMatrix()) * rayClip);
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        rayWorld = glm::vec3(glm::inverse(Camera::getViewMatrix()) * rayEye);
        rayWorld = glm::normalize(rayWorld);

        // Check for any intersections with object bounding spheres
        bool foundSomething = false;
        float depth = -1.0f;

        if (clickOnObject[roomModelObject] || clickOnObject[roomSpriteObject]) {
            for (unsigned long i = 0; i < World::sizeRoom(); i++) {
                Room& r = World::getRoom(i);
                glm::vec3 pos, norm;

                if (clickOnObject[roomModelObject]) {
                    for (unsigned long j = 0; j < r.sizeModels(); j++) {
                        StaticModel& sm = r.getModel(j);
                        if (glm::intersectRaySphere(Camera::getPosition(), rayWorld, sm.getCenter(), sm.getRadius(),
                                                    pos, norm)) {
                            float newDepth = glm::abs(glm::distance(sm.getCenter(), Camera::getPosition()));
                            if ((newDepth < depth) || (depth < 0.0f)) {
                                depth = newDepth;
                                lastIndexA = i;
                                lastIndexB = j;
                                lastIntersectPos = pos;
                                lastIntersectNorm = norm;
                                lastClickedObject = roomModelObject;
                                foundSomething = true;
                            }
                        }
                    }
                }

                if (clickOnObject[roomSpriteObject]) {
                    for (unsigned long j = 0; j < r.sizeSprites(); j++) {
                        RoomSprite& rs = r.getSprite(j);
                        if (glm::intersectRaySphere(Camera::getPosition(), rayWorld, rs.getCenter(), rs.getRadius(), pos, norm)) {
                            float newDepth = glm::abs(glm::distance(rs.getCenter(), Camera::getPosition()));
                            if ((newDepth < depth) || (depth < 0.0f)) {
                                depth = newDepth;
                                lastIndexA = i;
                                lastIndexB = j;
                                lastIntersectPos = pos;
                                lastIntersectNorm = norm;
                                lastClickedObject = roomSpriteObject;
                                foundSomething = true;
                            }
                        }
                    }
                }
            }
        }

        if (!foundSomething) {
            lastClickedObject = WorldObjectCount;
        }
    }
}

void Selector::displaySelection() {
    if (lastClickedObject == roomModelObject) {
        World::getRoom(lastIndexA).getModel(lastIndexB).displayBoundingSphere(Camera::getProjectionMatrix() * Camera::getViewMatrix(), glm::vec3(1.0f, 0.0f, 0.0f));
    } else if (lastClickedObject == roomSpriteObject) {
        World::getRoom(lastIndexA).getSprite(lastIndexB).displayBoundingSphere(Camera::getProjectionMatrix() * Camera::getViewMatrix(), glm::vec3(1.0f, 0.0f, 0.0f));
    } else {
        lastClickedObject = WorldObjectCount;
    }
}

void Selector::display() {
    if (!visible)
        return;

    if (!ImGui::Begin("Object Selector", &visible, ImVec2(500, 200))) {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("RoomModels", &clickOnObject[roomModelObject]);
    ImGui::SameLine();
    ImGui::Checkbox("RoomSprites", &clickOnObject[roomSpriteObject]);
    ImGui::SameLine();
    if (ImGui::Button("Hide Selector")) {
        visible = false;
    }
    ImGui::Separator();

    ImGui::Text("Camera: (%.2f %.2f %.2f)", Camera::getPosition().x, Camera::getPosition().y, Camera::getPosition().z);
    ImGui::Text("Last click: (%d %d)", rayScreen.x, rayScreen.y);
    if ((rayScreen.x >= 0) && (rayScreen.y >= 0)) {
        ImGui::Text("Normalized Ray: (%.3f %.3f %.3f)", rayWorld.x, rayWorld.y, rayWorld.z);
    }

    if (lastClickedObject != WorldObjectCount) {
        ImGui::Text("Intersect Pos: (%.2f %.2f %.2f)", lastIntersectPos.x, lastIntersectPos.y, lastIntersectPos.z);
        ImGui::Text("Intersect Norm: (%.2f %.2f %.2f)", lastIntersectNorm.x, lastIntersectNorm.y, lastIntersectNorm.z);
    }

    if (lastClickedObject == roomModelObject) {
        ImGui::Text("Last Room: %lu", lastIndexA);
        ImGui::Text("Last RoomModel: %lu", lastIndexB);
    } else if (lastClickedObject == roomSpriteObject) {
        ImGui::Text("Last Room: %lu", lastIndexA);
        ImGui::Text("Last RoomSprite: %lu", lastIndexB);
    } else {
        lastClickedObject = WorldObjectCount;
    }

    ImGui::End();
}

