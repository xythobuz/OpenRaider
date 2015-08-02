/*!
 * \file src/Selector.cpp
 * \brief Selector Window
 *
 * \author xythobuz
 */

#include "imgui/imgui.h"

#include "global.h"
#include "Log.h"
#include "Selector.h"

bool Selector::visible = false;

static unsigned int lastX = -1, lastY = -1;

void Selector::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    if ((button == leftmouseKey) && (!released)) {
        lastX = x;
        lastY = y;
    }
}

void Selector::display() {
    if (!visible)
        return;

    if (!ImGui::Begin("Object Selector", &visible, ImVec2(300, 300))) {
        ImGui::End();
        return;
    }

    ImGui::Text("Last Click (Screen): (%d %d)", lastX, lastY);
    if ((lastX < 0) || (lastY < 0)) {
        ImGui::Text("Last Click (World): (? ?)");
    } else {

    }

    if (ImGui::Button("Hide Selector")) {
        visible = false;
    }

    ImGui::End();
}

