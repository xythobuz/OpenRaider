/*!
 * \file src/Debug.cpp
 * \brief Debug UI
 *
 * \author xythobuz
 */

#include "global.h"
#include "Debug.h"
#include "RunTime.h"
#include "TextureManager.h"
#include "Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "imgui/stb_image.h"

unsigned int Debug::fontTex;

Debug::Debug() {
    zPos = -1;

    UI::addWindow(this);
}

Debug::~Debug() {
    // TODO Segfaults...?
    //ImGui::Shutdown();

    UI::removeWindow(this);
}

int Debug::initialize() {
    iniFilename = getRunTime().getBaseDir() + "/imgui.ini";
    logFilename = getRunTime().getBaseDir() + "/imgui_log.txt";

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)getWindow().getWidth(), (float)getWindow().getHeight());
    io.DeltaTime = 1.0f/60.0f;

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

    io.RenderDrawListsFn = Debug::renderImGui;

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

void Debug::display() {
    ImGui::Render();
}

void Debug::eventsFinished() {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)getWindow().getWidth(), (float)getWindow().getHeight());
    io.DeltaTime = 1.0f / 60.0f; // TODO proper timing

    ImGui::NewFrame();
}

void Debug::handleKeyboard(KeyboardButton key, bool pressed) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = pressed;
    io.KeyCtrl = io.KeysDown[leftctrlKey] | io.KeysDown[rightctrlKey];
    io.KeyShift = io.KeysDown[leftshiftKey] | io.KeysDown[rightshiftKey];
}

void Debug::handleText(char *text, bool notFinished) {
    ImGuiIO& io = ImGui::GetIO();
    while (*text != '\0') {
        io.AddInputCharacter(*text);
        text++;
    }
}

void Debug::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
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
}

void Debug::handleMouseMotion(int xrel, int yrel, int xabs, int yabs) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)xabs, (float)yabs);
}

void Debug::handleMouseScroll(int xrel, int yrel) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = (yrel != 0) ? yrel > 0 ? 1 : -1 : 0;
}

void Debug::renderImGui(ImDrawList** const cmd_lists, int cmd_lists_count) {
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
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer+8));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer+16));

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

