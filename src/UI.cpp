/*!
 * \file src/UI.cpp
 * \brief UI interface manager
 *
 * \author xythobuz
 */

#include <algorithm>

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "Log.h"
#include "Menu.h"
#include "RunTime.h"
#include "TextureManager.h"
#include "Window.h"
#include "commands/Command.h"
#include "utils/time.h"
#include "UI.h"

#define STB_IMAGE_IMPLEMENTATION
#include "imgui/stb_image.h"

bool UI::visible = false;
unsigned int UI::fontTex;
std::string UI::iniFilename;
std::string UI::logFilename;
bool UI::metaKeyIsActive = false;

std::list<std::tuple<KeyboardButton, bool>> UI::keyboardEvents;
std::list<std::tuple<unsigned int, unsigned int, KeyboardButton, bool>> UI::clickEvents;
std::list<std::tuple<int, int, int, int>> UI::motionEvents;
std::list<std::tuple<int, int>> UI::scrollEvents;

int UI::initialize() {
    iniFilename = getRunTime().getBaseDir() + "/imgui.ini";
    logFilename = getRunTime().getBaseDir() + "/imgui_log.txt";

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)getWindow().getWidth(), (float)getWindow().getHeight());
    io.DeltaTime = 1.0f / 60.0f;

    io.IniFilename = iniFilename.c_str();
    io.LogFilename = logFilename.c_str();

    io.KeyMap[ImGuiKey_Tab] = tabKey;
    io.KeyMap[ImGuiKey_LeftArrow] = leftKey;
    io.KeyMap[ImGuiKey_RightArrow] = rightKey;
    io.KeyMap[ImGuiKey_UpArrow] = upKey;
    io.KeyMap[ImGuiKey_DownArrow] = downKey;
    io.KeyMap[ImGuiKey_Home] = homeKey;
    io.KeyMap[ImGuiKey_End] = endKey;
    io.KeyMap[ImGuiKey_Delete] = delKey;
    io.KeyMap[ImGuiKey_Backspace] = backspaceKey;
    io.KeyMap[ImGuiKey_Enter] = enterKey;
    io.KeyMap[ImGuiKey_Escape] = escapeKey;
    io.KeyMap[ImGuiKey_A] = aKey;
    io.KeyMap[ImGuiKey_C] = cKey;
    io.KeyMap[ImGuiKey_V] = vKey;
    io.KeyMap[ImGuiKey_X] = xKey;
    io.KeyMap[ImGuiKey_Y] = yKey;
    io.KeyMap[ImGuiKey_Z] = zKey;

    io.RenderDrawListsFn = UI::renderImGui;

    // Load font texture
    //! \todo Use our own font subsystem instead of this?
    const void* png_data;
    unsigned int png_size;
    ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
    int tex_x, tex_y, tex_comp;
    void* tex_data = stbi_load_from_memory((const unsigned char*)png_data,
                                           (int)png_size, &tex_x, &tex_y, &tex_comp, 0);

    //! \fixme TODO use proper texture slot
    fontTex = getTextureManager().loadBufferSlot((unsigned char*)tex_data,
              tex_x, tex_y, RGBA, 32, 0, false);

    stbi_image_free(tex_data);

    return 0;
}

void UI::eventsFinished() {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)getWindow().getWidth(), (float)getWindow().getHeight());

    static unsigned long lastTime = 0;
    io.DeltaTime = ((float)(systemTimerGet() - lastTime)) / 1000.0f;
    lastTime = systemTimerGet();
    if (io.DeltaTime <= 0.0f)
        io.DeltaTime = 1.0f / 60.0f;

    ImGui::NewFrame();

    if (!visible) {
        while (!clickEvents.empty()) {
            auto i = clickEvents.front();
            if (getMenu().isVisible()) {
                getMenu().handleMouseClick(std::get<0>(i), std::get<1>(i),
                                           std::get<2>(i), std::get<3>(i));
            }
            clickEvents.pop_front();
        }

        while (!motionEvents.empty()) {
            auto i = motionEvents.front();
            if (!getMenu().isVisible()) {
                getGame().handleMouseMotion(std::get<0>(i), std::get<1>(i),
                                            std::get<2>(i), std::get<3>(i));
            }
            motionEvents.pop_front();
        }

        while (!scrollEvents.empty()) {
            auto i = scrollEvents.front();
            if (getMenu().isVisible()) {
                getMenu().handleMouseScroll(std::get<0>(i), std::get<1>(i));
            }
            scrollEvents.pop_front();
        }
    }

    while (!keyboardEvents.empty()) {
        auto i = keyboardEvents.front();

        if (!visible) {
            if (getMenu().isVisible()) {
                getMenu().handleKeyboard(std::get<0>(i), std::get<1>(i));
            } else {
                for (int n = forwardAction; n < ActionEventCount; n++) {
                    if (getRunTime().getKeyBinding((ActionEvents)n) == std::get<0>(i))
                        getGame().handleAction((ActionEvents)n, !std::get<1>(i));
                }
            }
        }

        if (std::get<1>(i)) {
            if (!visible) {
                if (getRunTime().getKeyBinding(menuAction) == std::get<0>(i)) {
                    getMenu().setVisible(!getMenu().isVisible());
                }
            }

            if ((!io.WantCaptureKeyboard) || (!visible)) {
                if (getRunTime().getKeyBinding(debugAction) == std::get<0>(i)) {
                    if (!metaKeyIsActive)
                        visible = !visible;
                }
            }
        }

        keyboardEvents.pop_front();
    }

    bool clicked = !clickEvents.empty();
    // Only already empty when !visible
    if (visible) {
        clickEvents.clear();
        motionEvents.clear();
        scrollEvents.clear();
    }

    if (visible && (
            ((!io.WantCaptureKeyboard) && io.KeysDown[escapeKey])
            || ((!io.WantCaptureMouse) && clicked)
        )) {
        visible = false;
    }

    if (getWindow().getTextInput() != visible)
        getWindow().setTextInput(visible);

    bool input = !(visible || getMenu().isVisible());
    if (getWindow().getMousegrab() != input)
        getWindow().setMousegrab(input);
}

void UI::display() {
    if (!visible)
        return;

    Console::display();

    if (ImGui::Begin("Engine/RT")) {
        if (ImGui::CollapsingHeader("Engine", NULL, true, true)) {
            ImGui::Text("Uptime: %lums", systemTimerGet());
        }

        if (ImGui::CollapsingHeader("Debug")) {

        }

        if (ImGui::CollapsingHeader("UI Help")) {
            ImGui::ShowUserGuide();
        }
    }
    ImGui::End();

    ImGui::Render();
}

void UI::shutdown() {
    ImGui::Shutdown();
}

void UI::handleKeyboard(KeyboardButton key, bool pressed) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = pressed;
    io.KeyCtrl = io.KeysDown[leftctrlKey] | io.KeysDown[rightctrlKey];
    io.KeyShift = io.KeysDown[leftshiftKey] | io.KeysDown[rightshiftKey];

    keyboardEvents.push_back(std::make_tuple(key, pressed));

    if ((key == leftguiKey) || (key == rightguiKey))
        metaKeyIsActive = pressed;
}

void UI::handleText(char* text, bool notFinished) {
    if (notFinished)
        return;

    ImGuiIO& io = ImGui::GetIO();
    while (*text != '\0') {
        io.AddInputCharacter(*text);
        text++;
    }
}

void UI::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
    if (button == leftmouseKey) {
        io.MouseDown[0] = !released;
    } else if (button == rightmouseKey) {
        io.MouseDown[1] = !released;
    } else if (button == middlemouseKey) {
        io.MouseDown[2] = !released;
    } else if (button == fourthmouseKey) {
        io.MouseDown[3] = !released;
    } else if (button == fifthmouseKey) {
        io.MouseDown[4] = !released;
    }

    clickEvents.push_back(std::make_tuple(x, y, button, released));
}

void UI::handleMouseMotion(int xrel, int yrel, int xabs, int yabs) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)xabs, (float)yabs);

    motionEvents.push_back(std::make_tuple(xrel, yrel, xabs, yabs));
}

void UI::handleMouseScroll(int xrel, int yrel) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = (yrel != 0) ? yrel > 0 ? 1 : -1 : 0;

    scrollEvents.push_back(std::make_tuple(xrel, yrel));
}

void UI::setVisible(bool v) {
    visible = v;
}

bool UI::isVisible() {
    return visible;
}

void UI::renderImGui(ImDrawList** const cmd_lists, int cmd_lists_count) {
    if (cmd_lists_count == 0)
        return;

    getWindow().glEnter2D();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Setup texture
    getTextureManager().bindTextureId(fontTex);

    // Render command lists
    for (int n = 0; n < cmd_lists_count; n++) {
        const ImDrawList* cmd_list = cmd_lists[n];
        const unsigned char* vtx_buffer = (const unsigned char*)cmd_list->vtx_buffer.begin();
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + 8));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + 16));

        int vtx_offset = 0;
        const ImDrawCmd* pcmd_end = cmd_list->commands.end();
        for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++) {
            glScissor((int)pcmd->clip_rect.x, (int)(ImGui::GetIO().DisplaySize.y - pcmd->clip_rect.w),
                      (int)(pcmd->clip_rect.z - pcmd->clip_rect.x),
                      (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
            glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
            vtx_offset += pcmd->vtx_count;
        }
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    getWindow().glExit2D();
}

