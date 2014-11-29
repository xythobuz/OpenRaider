/*!
 * \file src/UI.cpp
 * \brief UI interface manager
 *
 * \author xythobuz
 */

#include <algorithm>
#include <cstring>

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "Log.h"
#include "Menu.h"
#include "Render.h"
#include "RunTime.h"
#include "Sound.h"
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
              tex_x, tex_y, RGBA, 32, TextureManager::TextureStorage::SYSTEM, -1, false);

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

    if (ImGui::Begin("Engine")) {
        if (ImGui::CollapsingHeader("Engine Info")) {
            ImGui::Text("Uptime: %lums", systemTimerGet());
            ImGui::Text("Frames per Second: %luFPS", getRunTime().getFPS());
            if (getRunTime().getHistoryFPS().size() > 1) {
                static bool scroll = true;
                if (scroll) {
                    int offset = getRunTime().getHistoryFPS().size() - 1;
                    if (offset > 10)
                        offset = 10;
                    ImGui::PlotLines("FPS", &getRunTime().getHistoryFPS()[1],
                            getRunTime().getHistoryFPS().size() - 1,
                            getRunTime().getHistoryFPS().size() - offset - 1);
                } else {
                    ImGui::PlotLines("FPS", &getRunTime().getHistoryFPS()[1],
                            getRunTime().getHistoryFPS().size() - 1);
                }
                ImGui::SameLine();
                ImGui::Checkbox("Scroll##fpsscroll", &scroll);
            }
        }

        if (ImGui::CollapsingHeader("RunTime Settings")) {
            bool showFPS = getRunTime().getShowFPS();
            if (ImGui::Checkbox("Show FPS##runtime", &showFPS)) {
                getRunTime().setShowFPS(showFPS);
            }
            ImGui::SameLine();
            bool running = getRunTime().isRunning();
            if (ImGui::Checkbox("Running (!)##runtime", &running)) {
                getRunTime().setRunning(running);
            }
            ImGui::SameLine();
            bool sound = getSound().getEnabled();
            if (ImGui::Checkbox("Sound##runtime", &sound)) {
                getSound().setEnabled(sound);
            }
            ImGui::SameLine();
            bool fullscreen = getWindow().getFullscreen();
            if (ImGui::Checkbox("Fullscreen##runtime", &fullscreen)) {
                getWindow().setFullscreen(fullscreen);
            }

            float vol = getSound().getVolume();
            if (ImGui::InputFloat("Volume##runtime", &vol, 0.0f, 0.0f, 3, ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (vol < 0.0f)
                    vol = 0.0f;
                if (vol > 1.0f)
                    vol = 1.0f;
                getSound().setVolume(vol);
            }

            int w = getWindow().getWidth();
            if (ImGui::InputInt("Width##runtime", &w, 10, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (w < 1)
                    w = 1;
                getWindow().setSize(w, getWindow().getHeight());
            }
            int h = getWindow().getHeight();
            if (ImGui::InputInt("Height##runtime", &h, 10, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (h < 1)
                    h = 1;
                getWindow().setSize(getWindow().getWidth(), h);
            }

            static int fr = 0;
            char buff[1024];
            strncpy(buff, getRunTime().getBaseDir().c_str(), 1024);
            if (ImGui::InputText("BaseDir##runtime", buff, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
                getRunTime().setBaseDir(buff);
                fr = getRunTime().getFPS();
            }
            if (fr > 0) {
                ImGui::SameLine();
                ImGui::Text("Done!##runtime1");
                fr--;
            }

            static int fr2 = 0;
            char buff2[1024];
            strncpy(buff2, getRunTime().getPakDir().c_str(), 1024);
            if (ImGui::InputText("PakDir##runtime", buff2, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
                getRunTime().setPakDir(buff2);
                fr2 = getRunTime().getFPS();
            }
            if (fr2 > 0) {
                ImGui::SameLine();
                ImGui::Text("Done!##runtime2");
                fr2--;
            }

            static int fr3 = 0;
            char buff3[1024];
            strncpy(buff3, getRunTime().getAudioDir().c_str(), 1024);
            if (ImGui::InputText("AudioDir##runtime", buff3, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
                getRunTime().setAudioDir(buff3);
                fr3 = getRunTime().getFPS();
            }
            if (fr3 > 0) {
                ImGui::SameLine();
                ImGui::Text("Done!##runtime3");
                fr3--;
            }

            static int fr4 = 0;
            char buff4[1024];
            strncpy(buff4, getRunTime().getDataDir().c_str(), 1024);
            if (ImGui::InputText("DataDir##runtime", buff4, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
                getRunTime().setDataDir(buff4);
                fr4 = getRunTime().getFPS();
            }
            if (fr4 > 0) {
                ImGui::SameLine();
                ImGui::Text("Done!##runtime4");
                fr4--;
            }
        }

        static bool visibleTex = false;
        static bool visibleTile = false;
        static bool visibleAnim = false;
        if (ImGui::CollapsingHeader("Texture Viewer")) {
            static bool game = getGame().isLoaded();
            static int index = 0;
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::SliderInt("##texslide", &index, 0, getTextureManager().numTextures(
                        game ? TextureManager::TextureStorage::GAME
                        : TextureManager::TextureStorage::SYSTEM) - 1);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ImGui::Button("+##texplus", ImVec2(0, 0), true)) {
                if (index < (getTextureManager().numTextures(
                                game ? TextureManager::TextureStorage::GAME
                                : TextureManager::TextureStorage::SYSTEM) - 1))
                    index++;
                else
                    index = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("-##texminus", ImVec2(0, 0), true)) {
                if (index > 0)
                    index--;
                else
                    index = getTextureManager().numTextures(
                                game ? TextureManager::TextureStorage::GAME
                                : TextureManager::TextureStorage::SYSTEM) - 1;
            }
            ImGui::SameLine();
            if ((getTextureManager().numTextures() > 0)) {
                ImGui::Checkbox("Game##texgame", &game);
            } else {
                game = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Show##texshow")) {
                visibleTex = true;
                visibleTile = false;
                visibleAnim = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear##texclear")) {
                getRender().debugDisplayTexture();
                visibleTex = false;
            }
            if (visibleTex) {
                getRender().debugDisplayTexture(index,
                        game ? TextureManager::TextureStorage::GAME
                        : TextureManager::TextureStorage::SYSTEM,
                        ImGui::GetWindowPos().x - ImGui::GetWindowWidth(),
                        ImGui::GetWindowPos().y,
                        ImGui::GetWindowWidth(), ImGui::GetWindowWidth());
            }
        }

        if (ImGui::CollapsingHeader("Textile Viewer")) {
            if (getTextureManager().numTiles() > 0) {
                static int index = 0;
                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
                ImGui::SliderInt("##tileslide", &index, 0, getTextureManager().numTiles() - 1);
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("+##tileplus", ImVec2(0, 0), true)) {
                    if (index < (getTextureManager().numTiles() - 1))
                        index++;
                    else
                        index = 0;
                }
                ImGui::SameLine();
                if (ImGui::Button("-##tileminus", ImVec2(0, 0), true)) {
                    if (index > 0)
                        index--;
                    else
                        index = getTextureManager().numTiles() - 1;
                }
                ImGui::SameLine();
                if (ImGui::Button("Show##tileshow")) {
                    visibleTile = true;
                    visibleTex = false;
                    visibleAnim = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear##tileclear")) {
                    getRender().debugDisplayTextile();
                    visibleTile = false;
                }
                if (visibleTile && (index < getTextureManager().numTiles())) {
                    ImGui::Text(getTextureManager().getTile(index).isTriangle() ? "Triangle" : "Rectangle");
                }
                if (visibleTile) {
                    getRender().debugDisplayTextile(index,
                            ImGui::GetWindowPos().x - (ImGui::GetWindowWidth() / 2),
                            ImGui::GetWindowPos().y,
                            (ImGui::GetWindowWidth() / 2), (ImGui::GetWindowWidth() / 2));
                }
            } else {
                ImGui::Text("Please load a level using the new loader!");
            }
        }

        if (ImGui::CollapsingHeader("Animated Textile Viewer")) {
            if (getTextureManager().numAnimatedTiles() > 0) {
                static int index = 0;
                static int tile = getTextureManager().getFirstTileAnimation(index);
                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
                if (ImGui::SliderInt("##animslide", &index, 0, getTextureManager().numAnimatedTiles() - 1)) {
                    tile = getTextureManager().getFirstTileAnimation(index);
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("+##animplus", ImVec2(0, 0), true)) {
                    if (index < (getTextureManager().numAnimatedTiles() - 1))
                        index++;
                    else
                        index = 0;
                    tile = getTextureManager().getFirstTileAnimation(index);
                }
                ImGui::SameLine();
                if (ImGui::Button("-##animminus", ImVec2(0, 0), true)) {
                    if (index > 0)
                        index--;
                    else
                        index = getTextureManager().numAnimatedTiles() - 1;
                    tile = getTextureManager().getFirstTileAnimation(index);
                }
                ImGui::SameLine();
                static int fr = 0;
                if (ImGui::Button("Show##animshow")) {
                    visibleAnim = true;
                    visibleTex = false;
                    visibleTile = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear##animclear")) {
                    getRender().debugDisplayTextile();
                    visibleAnim = false;
                }
                if (visibleAnim) {
                    if (fr > 0) {
                        fr--;
                    } else {
                        getRender().debugDisplayTextile(tile,
                                ImGui::GetWindowPos().x - (ImGui::GetWindowWidth() / 2),
                                ImGui::GetWindowPos().y,
                                (ImGui::GetWindowWidth() / 2), (ImGui::GetWindowWidth() / 2));
                        fr = getRunTime().getFPS() / 2;
                        tile = getTextureManager().getNextTileAnimation(tile);
                    }
                    ImGui::Text("Current Tile: %d", tile);
                }
            } else {
                ImGui::Text("Please load a level with animated textures!");
            }
        }

        if (ImGui::CollapsingHeader("SoundSample Player")) {
            if (!getSound().getEnabled()) {
                ImGui::Text("Please enable Sound before loading a level!");
                if (ImGui::Button("Enable Sound!")) {
                    getSound().setEnabled(true);
                }
            } else if (getSound().registeredSources() == 0) {
                ImGui::Text("Please load a level!");
            } else {
                static int index = 0;
                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
                ImGui::SliderInt("##soundslide", &index, 0, getSound().registeredSources() - 1);
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("+##soundplus", ImVec2(0, 0), true)) {
                    if (index < (getSound().registeredSources() - 1))
                        index++;
                    else
                        index = 0;
                }
                ImGui::SameLine();
                if (ImGui::Button("-##soundminus", ImVec2(0, 0), true)) {
                    if (index > 0)
                        index--;
                    else
                        index = getSound().registeredSources() - 1;
                }
                ImGui::SameLine();
                if (ImGui::Button("Play##soundplay")) {
                    getSound().play(index);
                }
            }
        }

        if (ImGui::CollapsingHeader("ImGui UI Help")) {
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
    getTextureManager().bindTextureId(fontTex, TextureManager::TextureStorage::SYSTEM);

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

