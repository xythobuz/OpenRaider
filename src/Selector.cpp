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
#include "system/Window.h"
#include "Selector.h"

bool Selector::visible = false;

static int lastX = -1, lastY = -1;
static bool workToDo = false;

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

    if (workToDo) {
        glm::vec2 normalized = glm::vec2((2.0f * lastX) / Window::getSize().x - 1.0f,
                                         1.0f - (2.0f * lastY) / Window::getSize().y);
        glm::vec4 rayClip(normalized.x, normalized.y, -1.0f, 1.0f);
        glm::vec4 rayEye(glm::inverse(Camera::getProjectionMatrix()) * rayClip);
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        rayWorld = glm::vec3(glm::inverse(Camera::getViewMatrix()) * rayEye);
        rayWorld = glm::normalize(rayWorld);
        workToDo = false;
    }

    ImGui::Text("Screenspace: (%d %d)", lastX, lastY);
    ImGui::Text("Camera: (%.2f %.2f %.2f)", Camera::getPosition().x, Camera::getPosition().y, Camera::getPosition().z);
    if ((lastX < 0) || (lastY < 0)) {
        ImGui::Text("Normalized Ray: (? ? ?)");
    } else {
        ImGui::Text("Normalized Ray: (%.3f %.3f %.3f)", rayWorld.x, rayWorld.y, rayWorld.z);
    }

    if (ImGui::Button("Hide Selector")) {
        visible = false;
    }

    ImGui::End();
}

