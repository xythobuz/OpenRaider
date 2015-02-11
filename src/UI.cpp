/*!
 * \file src/UI.cpp
 * \brief UI interface manager
 *
 * \author xythobuz
 */

#include "imgui/imgui.h"

#include "global.h"
#include "Camera.h"
#include "Console.h"
#include "Game.h"
#include "Menu.h"
#include "Render.h"
#include "RunTime.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "system/Window.h"
#include "utils/time.h"
#include "UI.h"

Shader UI::imguiShader;
bool UI::visible = false;
unsigned int UI::fontTex;
std::string UI::iniFilename;
std::string UI::logFilename;
bool UI::metaKeyIsActive = false;

std::list<std::tuple<KeyboardButton, bool>> UI::keyboardEvents;
std::list<std::tuple<unsigned int, unsigned int, KeyboardButton, bool>> UI::clickEvents;
std::list<std::tuple<int, int, int, int>> UI::motionEvents;
std::list<std::tuple<int, int>> UI::scrollEvents;

void UI::setSize(glm::i32vec2 s) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(s.x, s.y);
}

int UI::initialize() {
    if (imguiShader.compile(imguiShaderVertex, imguiShaderFragment) < 0)
        return -1;
    if (imguiShader.addUniform("screen") < 0)
        return -2;
    if (imguiShader.addUniform("textureSampler") < 0)
        return -3;

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
    io.GetClipboardTextFn = Window::getClipboard;
    io.SetClipboardTextFn = Window::setClipboard;
    io.ImeSetInputScreenPosFn = Window::inputPositionCallback;

    // Load font texture
    //! \TODO allow loading other TTF fonts
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    fontTex = TextureManager::loadBufferSlot(pixels, width, height, ColorMode::RGBA, 32,
              TextureStorage::SYSTEM, -1, false);
    auto bm = TextureManager::getBufferManager(fontTex, TextureStorage::SYSTEM);
    io.Fonts->TexID = bm;

    // Set up OpenRaider style
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]                 = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.50f, 0.70f, 1.00f, 0.45f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.40f, 0.65f, 1.00f, 0.20f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.40f, 0.65f, 1.00f, 0.15f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.40f, 0.65f, 1.00f, 0.30f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.65f, 1.00f, 0.40f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(1.00f, 0.16f, 0.16f, 0.40f);
    style.Colors[ImGuiCol_ComboBg]              = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
    style.Colors[ImGuiCol_CheckHovered]         = ImVec4(1.00f, 0.40f, 0.40f, 0.45f);
    style.Colors[ImGuiCol_CheckActive]          = ImVec4(0.65f, 0.50f, 0.50f, 0.55f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(1.00f, 0.20f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_Button]               = ImVec4(1.00f, 0.20f, 0.20f, 0.60f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(1.00f, 0.20f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.21f, 0.54f, 1.00f, 0.45f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.22f, 0.56f, 1.00f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.00f, 0.31f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Column]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered]        = ImVec4(0.60f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_ColumnActive]         = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_CloseButton]          = ImVec4(0.21f, 0.56f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_CloseButtonHovered]   = ImVec4(0.21f, 0.32f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_CloseButtonActive]    = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    style.Colors[ImGuiCol_TooltipBg]            = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
    style.WindowPadding                         = ImVec2(2, 2);
    style.WindowRounding                        = 9;
    style.FramePadding                          = ImVec2(2, 1);
    style.ItemSpacing                           = ImVec2(2, 2);
    style.ItemInnerSpacing                      = ImVec2(1, 1);
    style.TouchExtraPadding                     = ImVec2(0, 0);
    style.TreeNodeSpacing                       = 3;
    style.ScrollBarWidth                        = 10;

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

    if (!(visible || Console::isVisible())) {
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
                Game::handleMouseMotion(std::get<0>(i), std::get<1>(i),
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

        if (!(visible || Console::isVisible())) {
            if (getMenu().isVisible()) {
                getMenu().handleKeyboard(std::get<0>(i), std::get<1>(i));
            } else {
                for (int n = forwardAction; n < ActionEventCount; n++) {
                    if (RunTime::getKeyBinding((ActionEvents)n) == std::get<0>(i))
                        Game::handleAction((ActionEvents)n, !std::get<1>(i));
                }
            }
        }

        if (std::get<1>(i)) {
            if (!(visible || Console::isVisible())) {
                if (RunTime::getKeyBinding(menuAction) == std::get<0>(i)) {
                    getMenu().setVisible(!getMenu().isVisible());
                }
            }

            if ((!io.WantCaptureKeyboard) || (!(visible || Console::isVisible()))) {
                if (!metaKeyIsActive) {
                    if (RunTime::getKeyBinding(debugAction) == std::get<0>(i)) {
                        visible = !visible;
                    }

                    if (RunTime::getKeyBinding(consoleAction) == std::get<0>(i)) {
                        Console::setVisible(!Console::isVisible());
                    }
                }
            }
        }

        keyboardEvents.pop_front();
    }

    bool clicked = !clickEvents.empty();
    clickEvents.clear();
    motionEvents.clear();
    scrollEvents.clear();

    if ((visible || Console::isVisible()) && (
            ((!io.WantCaptureKeyboard) && io.KeysDown[escapeKey])
            || ((!io.WantCaptureMouse) && clicked)
        )) {
        visible = false;
        Console::setVisible(false);
    }

    if (Window::getTextInput() != (visible || Console::isVisible()))
        Window::setTextInput(visible || Console::isVisible());

    bool input = !(visible || Console::isVisible() || getMenu().isVisible());
    if (Window::getMousegrab() != input)
        Window::setMousegrab(input);

    io.MouseWheel = 0;
}

void UI::display() {
    if (RunTime::getShowFPS()) {
        if (ImGui::Begin("Debug Overlay", nullptr, ImVec2(0, 0), 0.3f,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
                         | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings
                         | ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%d FPS", RunTime::getFPS());
            ImGui::Text("X: %.1f (%.2f)", Camera::getPosition().x, Camera::getRotation().x);
            ImGui::Text("Y: %.2f (%.2f)", Camera::getPosition().y, Camera::getRotation().y);
            ImGui::Text("Z: %.2f", Camera::getPosition().z);

            auto window = ImGui::GetWindowSize();
            auto screen = Window::getSize();
            ImGui::SetWindowPos(ImVec2(10, screen.y - window.y - 10));
        }
        ImGui::End();
    }

    Console::display();

    if (!visible) {
        ImGui::Render();
        return;
    }

    static bool showTestWindow = false;
    static bool showStyleWindow = false;
    if (ImGui::Begin("Engine", &visible, ImVec2(400, 400))) {
        Render::displayUI();
        RunTime::display();
        TextureManager::display();
        SoundManager::display();

        if (ImGui::CollapsingHeader("ImGui/Debug UI Help")) {
            ImGui::ShowUserGuide();
            ImGui::Separator();
            if (ImGui::Button("Show/Hide Test Window")) {
                showTestWindow = !showTestWindow;
            }
            ImGui::SameLine();
            if (ImGui::Button("Show/Hide Style Editor")) {
                showStyleWindow = !showStyleWindow;
            }
        }
    }
    ImGui::End();

    if (showTestWindow)
        ImGui::ShowTestWindow();

    if (showStyleWindow)
        ImGui::ShowStyleEditor();

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
    Game::handleControllerAxis(value, axis);
}

void UI::handleControllerButton(KeyboardButton button, bool released) {
    Game::handleControllerButton(button, released);
}

void UI::renderImGui(ImDrawList** const cmd_lists, int cmd_lists_count) {
    if (cmd_lists_count == 0)
        return;

    static ShaderBuffer vert, uv, col;

    glEnable(GL_SCISSOR_TEST);
    Shader::set2DState(true);

    imguiShader.use();
    imguiShader.loadUniform(0, Window::getSize());
    vert.bindBuffer(0, 2);
    uv.bindBuffer(1, 2);
    col.bindBuffer(2, 4);

    /*! \fixme Don't copy data
     * The GL calls and the shaders can probably be slightly altered
     * to avoid copying all the vertices, uvs and colors again here.
     */

    for (int i = 0; i < cmd_lists_count; i++) {
        auto& commands = cmd_lists[i]->commands;
        auto& buffer = cmd_lists[i]->vtx_buffer;

        int offset = 0;
        for (int n = 0; n < commands.size(); n++) {
            std::vector<glm::vec2> vertices;
            std::vector<glm::vec2> uvs;
            std::vector<glm::vec4> colors;

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

            vert.bufferData(vertices);
            uv.bufferData(uvs);
            col.bufferData(colors);

            auto bm = static_cast<BufferManager*>(commands[n].texture_id);
            assert(bm != nullptr);
            imguiShader.loadUniform(1, bm->getTextureID(), bm->getTextureStorage());

            glScissor(commands[n].clip_rect.x,
                      Window::getSize().y - commands[n].clip_rect.w,
                      commands[n].clip_rect.z - commands[n].clip_rect.x,
                      commands[n].clip_rect.w - commands[n].clip_rect.y);

            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        }
    }

    vert.unbind(0);
    uv.unbind(1);
    col.unbind(2);

    Shader::set2DState(false);
    glDisable(GL_SCISSOR_TEST);
}

// --------------------------------------
// *INDENT-OFF*

const char* UI::imguiShaderVertex = R"!?!(
#version 330 core

layout(location = 0) in vec2 vertexPosition_screen;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec4 vertexColor;

out vec2 UV;
out vec4 FragColor;

uniform vec2 screen;

void main() {
    vec2 halfScreen = screen / 2;
    vec2 vertexPosition_homogenous = (vertexPosition_screen - halfScreen) / halfScreen;

    gl_Position = vec4(vertexPosition_homogenous.x, -vertexPosition_homogenous.y, 0, 1);
    UV = vertexUV;
    FragColor = vertexColor;
}
)!?!";

const char* UI::imguiShaderFragment = R"!?!(
#version 330 core

in vec2 UV;
in vec4 FragColor;

out vec4 color;

uniform sampler2D textureSampler;

void main() {
    color = texture(textureSampler, UV) * FragColor;
}
)!?!";

// --------------------------------------
// *INDENT-ON*

