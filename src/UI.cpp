/*!
 * \file src/UI.cpp
 * \brief UI interface manager
 *
 * \author xythobuz
 */

#include <algorithm>

#include "global.h"
#include "Console.h"
#include "Menu.h"
#include "OpenRaider.h"
#include "TextureManager.h"
#include "Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "imgui/stb_image.h"

std::vector<UI*> UI::windows;
std::list<std::tuple<KeyboardButton, bool>> UI::keyboardEvents;
std::list<std::tuple<char *, bool>> UI::textEvents;
std::list<std::tuple<unsigned int, unsigned int, KeyboardButton, bool>> UI::clickEvents;
std::list<std::tuple<int, int>> UI::motionEvents;
std::list<std::tuple<int, int>> UI::scrollEvents;

static GLuint fontTex;

UI::~UI() {
}

void UI::display() { }
void UI::handleKeyboard(KeyboardButton key, bool pressed) { }
void UI::handleText(char *text, bool notFinished) { }
void UI::handleAction(ActionEvents action, bool isFinished) { }
void UI::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) { }
void UI::handleMouseMotion(int xrel, int yrel) { }
void UI::handleMouseScroll(int xrel, int yrel) { }

int UI::initialize() {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)getWindow().getWidth(), (float)getWindow().getHeight());
    io.DeltaTime = 1.0f/60.0f;
    io.PixelCenterOffset = 0.0f;

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
    //glGenTextures(1, &fontTex);
    //glBindTexture(GL_TEXTURE_2D, fontTex);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    const void* png_data;
    unsigned int png_size;
    ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
    int tex_x, tex_y, tex_comp;
    void* tex_data = stbi_load_from_memory((const unsigned char*)png_data, (int)png_size, &tex_x, &tex_y, &tex_comp, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_x, tex_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
    fontTex = getTextureManager().loadBufferSlot((unsigned char *)tex_data, tex_x, tex_y, RGBA, 32, 0);
    stbi_image_free(tex_data);

    return 0;
}

void UI::eventsFinished() {
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureKeyboard) {
        while (keyboardEvents.size() > 0) {
            sendKeyboard(std::get<0>(keyboardEvents.front()), std::get<1>(keyboardEvents.front()));
            keyboardEvents.pop_front();
        }

        while (textEvents.size() > 0) {
            sendText(std::get<0>(textEvents.front()), std::get<1>(textEvents.front()));
            textEvents.pop_front();
        }
    }

    if (!io.WantCaptureMouse) {
        while (clickEvents.size() > 0) {
            sendMouseClick(std::get<0>(clickEvents.front()), std::get<1>(clickEvents.front()),
                    std::get<2>(clickEvents.front()), std::get<3>(clickEvents.front()));
            clickEvents.pop_front();
        }

        while (motionEvents.size() > 0) {
            sendMouseMotion(std::get<0>(motionEvents.front()), std::get<1>(motionEvents.front()));
            motionEvents.pop_front();
        }

        while (scrollEvents.size() > 0) {
            sendMouseScroll(std::get<0>(scrollEvents.front()), std::get<1>(scrollEvents.front()));
            scrollEvents.pop_front();
        }
    }

    keyboardEvents.clear();
    textEvents.clear();
    clickEvents.clear();
    motionEvents.clear();
    scrollEvents.clear();

    ImGui::SetNewWindowDefaultPos(ImVec2(50, 50));
    bool show_test_window = true;
    ImGui::ShowTestWindow(&show_test_window);
}

void UI::renderImGui(ImDrawList** const cmd_lists, int cmd_lists_count) {
    if (cmd_lists_count == 0)
        return;

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    //glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Setup texture
    //glBindTexture(GL_TEXTURE_2D, fontTex);
    //glEnable(GL_TEXTURE_2D);
    getTextureManager().bindTextureId(fontTex);

    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;

    /*
    // Setup orthographic projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, height, 0.0f, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    */

    // Render command lists
    for (int n = 0; n < cmd_lists_count; n++) {
        const ImDrawList* cmd_list = cmd_lists[n];
        const unsigned char* vtx_buffer = (const unsigned char*)cmd_list->vtx_buffer.begin();
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer+8));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer+16));

        int vtx_offset = 0;
        const ImDrawCmd* pcmd_end = cmd_list->commands.end();
        for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++) {
            glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w),
                    (int)(pcmd->clip_rect.z - pcmd->clip_rect.x),
                    (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
            glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
            vtx_offset += pcmd->vtx_count;
        }
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //glDisableClientState(GL_VERTEX_ARRAY);
}

void UI::passKeyboard(KeyboardButton key, bool pressed) {
    keyboardEvents.push_back(std::make_tuple(key, pressed));

    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = pressed;
    io.KeyCtrl = io.KeysDown[leftctrlKey] | io.KeysDown[rightctrlKey];
    io.KeyShift = io.KeysDown[leftshiftKey] | io.KeysDown[rightshiftKey];
}

void UI::passText(char *text, bool notFinished) {
    textEvents.push_back(std::make_tuple(text, notFinished));
    // TODO
}

void UI::passMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    clickEvents.push_back(std::make_tuple(x, y, button, released));
    // TODO
}

void UI::passMouseMotion(int xrel, int yrel) {
    motionEvents.push_back(std::make_tuple(xrel, yrel));
    // TODO
}

void UI::passMouseScroll(int xrel, int yrel) {
    scrollEvents.push_back(std::make_tuple(xrel, yrel));

    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = (yrel != 0) ? yrel > 0 ? 1 : -1 : 0;
}

void UI::addWindow(UI* window) {
    windows.push_back(window);
}

void UI::removeWindow(UI *window) {
    findInList(window, [](unsigned long i){
        windows.erase(windows.begin() + i);
    });
}

bool UI::compareUIs(UI* a, UI* b) {
    return a->zPos < b->zPos;
}

bool UI::isOnTop(unsigned long windowID) {
    assert(windowID < windows.size());
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    unsigned long maxPos = (unsigned long)(maxIterator - windows.begin());
    return (maxPos == windowID);
}

void UI::moveToTop(unsigned long windowID) {
    assert(windowID < windows.size());

    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    long max = (*maxIterator)->zPos;
    unsigned long maxPos = (unsigned long)(maxIterator - windows.begin());

    if (maxPos != windowID) {
        windows.at(windowID)->zPos = max + 1;
    }
}

void UI::makeInvisible(unsigned long windowID) {
    assert(windowID < windows.size());
    windows.at(windowID)->zPos = -1;
}

void UI::findInList(UI *w, std::function<void (unsigned long i)> func) {
    for (unsigned long i = 0; i < windows.size(); i++) {
        auto UIptr = &(*windows.at(i));
        if (w == UIptr) {
            func(i);
            return;
        }
    }

    assert(false); // called UI was not registered
}

bool UI::isOnTop() {
    bool top = false;
    findInList(this, [&](unsigned long i) {
        top = UI::isOnTop(i);
    });
    return top;
}

void UI::moveToTop() {
    findInList(this, [](unsigned long i) {
        UI::moveToTop(i);
    });
}

void UI::makeInvisible() {
    findInList(this, [](unsigned long i) {
        UI::makeInvisible(i);
    });
}

void UI::displayInOrder() {
    std::sort(windows.begin(), windows.end(), compareUIs);
    for (auto &x : windows) {
        if (x->zPos >= 0) {
            x->display();
        }
    }

    ImGui::Render();
}

void UI::sendKeyboard(KeyboardButton key, bool pressed) {
    if (pressed) {
        if (getOpenRaider().keyBindings[menuAction] == key) {
            if (getMenu().isOnTop()) {
                getMenu().makeInvisible();
            } else {
                getMenu().moveToTop();
            }
        } else if (getOpenRaider().keyBindings[consoleAction] == key) {
            if (getConsole().isOnTop()) {
                getConsole().makeInvisible();
            } else {
                getConsole().moveToTop();
            }
        }
    }

    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleKeyboard(key, pressed);

    for (int i = forwardAction; i < ActionEventCount; i++) {
        if (getOpenRaider().keyBindings[i] == key) {
            (*maxIterator)->handleAction((ActionEvents)i, !pressed);
        }
    }

    bool mousegrab = (*maxIterator)->zPos == 0;
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void UI::sendText(char *text, bool notFinished) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleText(text, notFinished);
}

void UI::sendMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleMouseClick(x, y, button, released);

    for (int i = forwardAction; i < ActionEventCount; i++) {
        if (getOpenRaider().keyBindings[i] == button) {
            (*maxIterator)->handleAction((ActionEvents)i, released);
        }
    }
}

void UI::sendMouseMotion(int xrel, int yrel) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleMouseMotion(xrel, yrel);
}

void UI::sendMouseScroll(int xrel, int yrel) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleMouseScroll(xrel, yrel);
}

