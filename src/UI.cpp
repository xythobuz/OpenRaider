/*!
 * \file src/UI.cpp
 * \brief UI interface manager
 *
 * \author xythobuz
 */

#include <algorithm>
#include <cstring>

#include "global.h"
#include "Camera.h"
#include "Console.h"
#include "Game.h"
#include "Log.h"
#include "Menu.h"
#include "Render.h"
#include "RunTime.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "World.h"
#include "commands/Command.h"
#include "system/Sound.h"
#include "system/Window.h"
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
    iniFilename = RunTime::getBaseDir() + "/imgui.ini";
    logFilename = RunTime::getBaseDir() + "/imgui_log.txt";

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(Window::getSize().x, Window::getSize().y);
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
    ImGui::GetDefaultFontData(nullptr, nullptr, &png_data, &png_size);
    int tex_x, tex_y, tex_comp;
    void* tex_data = stbi_load_from_memory((const unsigned char*)png_data,
                                           (int)png_size, &tex_x, &tex_y, &tex_comp, 0);

    fontTex = getTextureManager().loadBufferSlot((unsigned char*)tex_data,
              tex_x, tex_y, TextureManager::ColorMode::RGBA, 32,
              TextureManager::TextureStorage::SYSTEM, -1, false);

    stbi_image_free(tex_data);

    return 0;
}

void UI::eventsFinished() {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(Window::getSize().x, Window::getSize().y);

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
                    if (RunTime::getKeyBinding((ActionEvents)n) == std::get<0>(i))
                        getGame().handleAction((ActionEvents)n, !std::get<1>(i));
                }
            }
        }

        if (std::get<1>(i)) {
            if (!visible) {
                if (RunTime::getKeyBinding(menuAction) == std::get<0>(i)) {
                    getMenu().setVisible(!getMenu().isVisible());
                }
            }

            if ((!io.WantCaptureKeyboard) || (!visible)) {
                if (RunTime::getKeyBinding(debugAction) == std::get<0>(i)) {
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

    if (Window::getTextInput() != visible)
        Window::setTextInput(visible);

    bool input = !(visible || getMenu().isVisible());
    if (Window::getMousegrab() != input)
        Window::setMousegrab(input);

    io.MouseWheel = 0;
}

void UI::display() {
    if (!visible)
        return;

    Console::display();

    if (ImGui::Begin("Engine")) {
        RunTime::display();
        SoundManager::display();

        /*
        static bool visibleTex = false;
        static bool visibleTile = false;
        static bool visibleAnim = false;
        static bool visibleSprite = false;
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
                visibleSprite = false;
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
                    visibleSprite = false;
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
                if (ImGui::Button("Show##animshow")) {
                    visibleAnim = true;
                    visibleTex = false;
                    visibleTile = false;
                    visibleSprite = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear##animclear")) {
                    getRender().debugDisplayTextile();
                    visibleAnim = false;
                }
                if (visibleAnim) {
                    static int fr = 0;
                    if (fr > 0) {
                        fr--;
                    } else {
                        getRender().debugDisplayTextile(tile,
                                                        ImGui::GetWindowPos().x - (ImGui::GetWindowWidth() / 2),
                                                        ImGui::GetWindowPos().y,
                                                        (ImGui::GetWindowWidth() / 2), (ImGui::GetWindowWidth() / 2));
                        fr = RunTime::getFPS() / 2;
                        tile = getTextureManager().getNextTileAnimation(tile);
                    }
                    ImGui::Text("Current Tile: %d", tile);
                }
            } else {
                ImGui::Text("Please load a level with animated textures!");
            }
        }

        if (ImGui::CollapsingHeader("Sprite Sequence Viewer")) {
            if (getWorld().sizeSprite() <= 0) {
                ImGui::Text("Please load a level containing sprites!");
            } else {
                static int index = 0;
                static int sprite = 0;
                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
                if (ImGui::SliderInt("##spriteslide", &index, 0, getWorld().sizeSprite() - 1))
                    sprite = 0;
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("+##spriteplus", ImVec2(0, 0), true)) {
                    if (index < (getWorld().sizeSprite() - 1))
                        index++;
                    else
                        index = 0;
                    sprite = 0;
                }
                ImGui::SameLine();
                if (ImGui::Button("-##spriteminus", ImVec2(0, 0), true)) {
                    if (index > 0)
                        index--;
                    else
                        index = getWorld().sizeSprite() - 1;
                    sprite = 0;
                }
                ImGui::SameLine();
                if (ImGui::Button("Show##spriteshow")) {
                    visibleSprite = true;
                    visibleTex = false;
                    visibleTile = false;
                    visibleAnim = false;
                    sprite = 0;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear##spriteclear")) {
                    getRender().debugDisplaySprite();
                    visibleSprite = false;
                }
                if (visibleSprite) {
                    static int fr = 0;
                    if (fr > 0) {
                        fr--;
                    } else {
                        getRender().debugDisplaySprite(index, sprite,
                                                       ImGui::GetWindowPos().x - (ImGui::GetWindowWidth() / 2),
                                                       ImGui::GetWindowPos().y,
                                                       (ImGui::GetWindowWidth() / 2), (ImGui::GetWindowWidth() / 2));
                        fr = RunTime::getFPS() / 10;
                        if (sprite < (getWorld().getSprite(index).size() - 1))
                            sprite++;
                        else
                            sprite = 0;
                    }

                    ImGui::Text("Sprite %d/%d", sprite + 1, getWorld().getSprite(index).size());
                }
            }
        }
        */

        if (ImGui::CollapsingHeader("ImGui/Debug UI Help")) {
            //ImGui::TextWrapped("DebugViewer Textures/Textiles/Sprites will be drawn on"
            //                   " the left side and scale with the size of this window!");
            //ImGui::Separator();
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
    io.MouseWheel += yrel;

    scrollEvents.push_back(std::make_tuple(xrel, yrel));
}

void UI::handleControllerAxis(float value, KeyboardButton axis) {
    getGame().handleControllerAxis(value, axis);
}

void UI::handleControllerButton(KeyboardButton button, bool released) {
    getGame().handleControllerButton(button, released);
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

    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);

    Window::imguiShader.use();
    Window::imguiShader.loadUniform(0, Window::getSize());
    Window::imguiShader.loadUniform(1, fontTex, TextureManager::TextureStorage::SYSTEM);
    Window::imguiShader.bindBuffer(0, 0, 2);
    Window::imguiShader.bindBuffer(1, 1, 2);
    Window::imguiShader.bindBuffer(2, 2, 4);

    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> colors;

    /*! \fixme Don't copy data
     * The GL calls and the shaders can probably be slightly altered
     * to avoid copying all the vertices, uvs and colors again here.
     */

    for (int i = 0; i < cmd_lists_count; i++) {
        auto& commands = cmd_lists[i]->commands;
        auto& buffer = cmd_lists[i]->vtx_buffer;

        int offset = 0;
        for (int n = 0; n < commands.size(); n++) {
            for (int v = 0; v < commands[n].vtx_count; v++) {
                vertices.push_back(glm::vec2(buffer[offset + v].pos.x, buffer[offset + v].pos.y));
                uvs.push_back(glm::vec2(buffer[offset + v].uv.x, buffer[offset + v].uv.y));

                float r, g, b, a;
                a = ((buffer[offset + v].col & 0xFF000000) >> 24) / 255.0f;
                b = ((buffer[offset + v].col & 0x00FF0000) >> 16) / 255.0f;
                g = ((buffer[offset + v].col & 0x0000FF00) >> 8) / 255.0f;
                r = (buffer[offset + v].col & 0x000000FF) / 255.0f;
                colors.push_back(glm::vec4(r, g, b, a));
            }

            offset += commands[n].vtx_count;

            Window::imguiShader.bufferData(0, vertices);
            Window::imguiShader.bufferData(1, uvs);
            Window::imguiShader.bufferData(2, colors);

            glScissor(commands[n].clip_rect.x,
                      Window::getSize().y - commands[n].clip_rect.w,
                      commands[n].clip_rect.z - commands[n].clip_rect.x,
                      commands[n].clip_rect.w - commands[n].clip_rect.y);

            glDrawArrays(GL_TRIANGLES, 0, vertices.size());

            vertices.clear();
            uvs.clear();
            colors.clear();
        }
    }

    Window::imguiShader.disableAttribs();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
}

