/*!
 * \file src/RunTime.cpp
 * \brief run time configuration storage
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "Render.h"
#include "UI.h"
#include "system/Sound.h"
#include "system/Window.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "RunTime.h"

std::string RunTime::baseDir;
std::string RunTime::pakDir;
std::string RunTime::audioDir;
std::string RunTime::dataDir;
KeyboardButton RunTime::keyBindings[ActionEventCount];
bool RunTime::gameIsRunning = false;
bool RunTime::showFPS = false;
unsigned long RunTime::lastTime = 0;
unsigned long RunTime::lastFrameTime = 0;
unsigned long RunTime::frameCount = 0;
unsigned long RunTime::frameCount2 = 0;
unsigned long RunTime::frameTimeSum = 0;
unsigned long RunTime::frameTimeSum2 = 0;
unsigned long RunTime::fps = 0;
std::vector<float> RunTime::history;

void RunTime::initialize() {
    baseDir = expandHomeDirectory("~/.OpenRaider");
    pakDir = baseDir + "/paks";
    audioDir = baseDir + "/music";
    dataDir = baseDir + "/data";

#ifdef DEBUG
    showFPS = true;
#endif

    for (int i = 0; i < ActionEventCount; i++)
        keyBindings[i] = unknownKey;

    // Default key bindings
    keyBindings[menuAction] = escapeKey;
    keyBindings[debugAction] = qKey;
    keyBindings[forwardAction] = wKey;
    keyBindings[backwardAction] = sKey;
    keyBindings[leftAction] = aKey;
    keyBindings[rightAction] = dKey;
}

KeyboardButton RunTime::getKeyBinding(ActionEvents event) {
    assert(event < ActionEventCount);
    return keyBindings[event];
}

void RunTime::setKeyBinding(ActionEvents event, KeyboardButton button) {
    assert(event < ActionEventCount);
    keyBindings[event] = button;
}

void RunTime::updateFPS() {
    frameCount++;
    lastFrameTime = systemTimerGet() - lastTime;
    frameTimeSum += lastFrameTime;
    frameTimeSum2 += lastFrameTime;
    lastTime = systemTimerGet();
    if (frameTimeSum >= 200) {
        fps = frameCount * (1000 / frameTimeSum);
        frameCount = frameTimeSum = 0;
    }

    if (frameTimeSum2 >= 1000) {
        history.push_back(frameCount2);
        frameCount2 = frameTimeSum2 = 0;
    }
    frameCount2++;
}

void RunTime::display() {
    if (ImGui::CollapsingHeader("RunTime Settings")) {
        bool showFPS = getShowFPS();
        if (ImGui::Checkbox("Show FPS##runtime", &showFPS)) {
            setShowFPS(showFPS);
        }
        ImGui::SameLine();
        bool running = isRunning();
        if (ImGui::Checkbox("Running (!)##runtime", &running)) {
            setRunning(running);
        }
        ImGui::SameLine();
        bool sound = Sound::getEnabled();
        if (ImGui::Checkbox("Sound##runtime", &sound)) {
            Sound::setEnabled(sound);
        }
        ImGui::SameLine();
        bool fullscreen = Window::getFullscreen();
        if (ImGui::Checkbox("Fullscreen##runtime", &fullscreen)) {
            Window::setFullscreen(fullscreen);
        }

        bool updateViewFrustum = Camera::getUpdateViewFrustum();
        if (ImGui::Checkbox("Update Frustum##runtime", &updateViewFrustum)) {
            Camera::setUpdateViewFrustum(updateViewFrustum);
        }
        ImGui::SameLine();
        bool displayViewFrustum = Render::getDisplayViewFrustum();
        if (ImGui::Checkbox("Show Frustum##runtime", &displayViewFrustum)) {
            Render::setDisplayViewFrustum(displayViewFrustum);
        }

        float vol = Sound::getVolume();
        if (ImGui::InputFloat("Volume##runtime", &vol, 0.0f, 0.0f, 3,
                              ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (vol < 0.0f)
                vol = 0.0f;
            if (vol > 1.0f)
                vol = 1.0f;
            Sound::setVolume(vol);
        }

        int w = Window::getSize().x;
        if (ImGui::InputInt("Width##runtime", &w, 10, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (w < 1)
                w = 1;
            Window::setSize(glm::vec2(w, Window::getSize().y));
        }
        int h = Window::getSize().y;
        if (ImGui::InputInt("Height##runtime", &h, 10, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (h < 1)
                h = 1;
            Window::setSize(glm::vec2(Window::getSize().x, h));
        }

        static int fr = 0;
        char buff[1024];
        strncpy(buff, getBaseDir().c_str(), 1024);
        if (ImGui::InputText("BaseDir##runtime", buff, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
            setBaseDir(buff);
            fr = getFPS();
        }
        if (fr > 0) {
            ImGui::SameLine();
            ImGui::Text("Done!##runtime1");
            fr--;
        }

        static int fr2 = 0;
        char buff2[1024];
        strncpy(buff2, getPakDir().c_str(), 1024);
        if (ImGui::InputText("PakDir##runtime", buff2, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
            setPakDir(buff2);
            fr2 = getFPS();
        }
        if (fr2 > 0) {
            ImGui::SameLine();
            ImGui::Text("Done!##runtime2");
            fr2--;
        }

        static int fr3 = 0;
        char buff3[1024];
        strncpy(buff3, getAudioDir().c_str(), 1024);
        if (ImGui::InputText("AudioDir##runtime", buff3, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
            setAudioDir(buff3);
            fr3 = getFPS();
        }
        if (fr3 > 0) {
            ImGui::SameLine();
            ImGui::Text("Done!##runtime3");
            fr3--;
        }

        static int fr4 = 0;
        char buff4[1024];
        strncpy(buff4, getDataDir().c_str(), 1024);
        if (ImGui::InputText("DataDir##runtime", buff4, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
            setDataDir(buff4);
            fr4 = getFPS();
        }
        if (fr4 > 0) {
            ImGui::SameLine();
            ImGui::Text("Done!##runtime4");
            fr4--;
        }
    }

    if (ImGui::CollapsingHeader("Performance")) {
        ImGui::Text("Uptime: %lums", systemTimerGet());
        ImGui::Text("Frames per Second: %luFPS", getFPS());
        if (getHistoryFPS().size() > 1) {
            static bool scroll = true;
            if (scroll) {
                int offset = getHistoryFPS().size() - 1;
                if (offset > 10)
                    offset = 10;
                ImGui::PlotLines("FPS", &getHistoryFPS()[1],
                                 getHistoryFPS().size() - 1,
                                 getHistoryFPS().size() - offset - 1);
            } else {
                ImGui::PlotLines("FPS", &getHistoryFPS()[1],
                                 getHistoryFPS().size() - 1);
            }
            ImGui::SameLine();
            ImGui::Checkbox("Scroll##fpsscroll", &scroll);
        }
    }
}

