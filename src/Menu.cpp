/*!
 * \file src/Menu.cpp
 * \brief Main Menu
 *
 * \author xythobuz
 */

#include "imgui/imgui.h"

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "Log.h"
#include "RunTime.h"
#include "UI.h"
#include "TextureManager.h"
#include "system/Window.h"
#include "utils/random.h"
#include "utils/strings.h"
#include "Menu.h"

const glm::vec4 Menu::textColor(0.5f, 0.7f, 1.0f, 1.0f);
const glm::vec4 Menu::selectedColor(1.0f, 0.0f, 0.0f, 1.0f);

bool Menu::visible = false;
Folder* Menu::mapFolder = nullptr;
std::vector<Script> Menu::scripts;
std::vector<Folder> Menu::paths;
std::vector<int> Menu::images;

int Menu::initialize() {
    shutdown();
    mapFolder = new Folder(RunTime::getPakDir());

    std::vector<File> files;
    mapFolder->findRecursiveFilesEndingWith(files, "tombpc.dat");
    for (auto& f : files) {
        scripts.emplace_back();
        auto s = scripts.size() - 1;

        images.push_back(-1);
        auto i = images.size() - 1;

        std::string path = convertPathDelimiter(removeLastPathElement(f.getPath()));
        paths.emplace_back(path);
        auto p = paths.size() - 1;

        if (scripts.at(s).load(f.getPath()) != 0) {
            Log::get(LOG_ERROR) << "Invalid Script: \"" << f.getPath()
                                << "\"!" << Log::endl;
            scripts.pop_back();
        } else {
            // Load one of the pictures of this level
            std::vector<File> texFiles;
            paths.at(p).findRecursiveFilesEndingWith(texFiles, ".pcx");
            paths.at(p).findRecursiveFilesEndingWith(texFiles, ".bmp");
            paths.at(p).findRecursiveFilesEndingWith(texFiles, ".png");
            paths.at(p).findRecursiveFilesEndingWith(texFiles, ".tga");
            paths.at(p).findRecursiveFilesEndingWith(texFiles, ".jpg");

            if (texFiles.size() > 0) {
                int which = randomInteger(texFiles.size() - 1);
                images.at(i) = TextureManager::loadImage(texFiles.at(which).getPath(),
                               TextureStorage::SYSTEM);
            }
        }
    }

    return 0;
}

void Menu::shutdown() {
    if (mapFolder != nullptr) {
        delete mapFolder;
        mapFolder = nullptr;
    }

    scripts.clear();
    paths.clear();
    images.clear(); //! \fixme free texture slots
}

void Menu::display() {
    if (!visible)
        return;

    glm::vec2 size = Window::getSize();
    if (!ImGui::Begin("Menu", nullptr, ImVec2(size.x, size.y), -1.0f, ImGuiWindowFlags_NoTitleBar
                      | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
                      | ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::End();
        return;
    }

    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(size.x, size.y));

    // Draw heading with version string
    auto headingSize = ImGui::CalcTextSize(VERSION);
    headingSize.x *= 3.0f; headingSize.y *= 3.0f;
    ImGui::SetWindowFontScale(3.0f);
    ImGui::SameLine(0, (size.x / 2) - (headingSize.x / 2));
    ImGui::TextColored(textColor, VERSION);
    ImGui::SetWindowFontScale(1.0f);

    static ImGuiFs::Dialog* dialog = nullptr;
    const bool browseButtonPressed = ImGui::Button("Select Level File");
    if (browseButtonPressed) {
        if (dialog != nullptr)
            delete dialog;
        dialog = new ImGuiFs::Dialog();
    }
    if (dialog) {
        dialog->chooseFileDialog(browseButtonPressed, RunTime::getPakDir().c_str(),
                                 ".phd;.psx;.tub;.tr2", nullptr, ImVec2(400, 300), ImGui::GetMousePos());
        std::string selectedFile = dialog->getChosenPath();
        if (selectedFile.length() > 0) {
            Game::loadLevel(selectedFile);
            delete dialog;
            dialog = nullptr;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Toggle Console")) {
        Console::setVisible(!Console::isVisible());
    }
    ImGui::SameLine();
    if (ImGui::Button("Toggle Debug Window")) {
        UI::setVisible(!UI::isVisible());
    }

    ImGui::Separator();

    // Set up columns for game list
    ImGui::Columns(3, "menuscripts");
    static bool offsets = false;
    if (!offsets) {
        ImGui::SetColumnOffset(1, 140.0f);
        ImGui::SetColumnOffset(2, size.x - 200.0f);
        offsets = true;
    }

    // List found games
    for (int i = 0; i < scripts.size(); i++) {
        ImGui::PushID(i);
        Script& s = scripts.at(i);

        if (images.at(i) >= 0) {
            auto bm = TextureManager::getBufferManager(images.at(i), TextureStorage::SYSTEM);
            ImGui::Image(bm, ImVec2(128, 128));
        }
        ImGui::NextColumn();

        ImGui::TextWrapped("Language: %s", s.getLanguage().c_str());
        ImGui::TextWrapped("Description: %s", s.getDescription().c_str());

        if (ImGui::TreeNode("", "%d Levels", s.levelCount())) {
            for (int l = 0; l < s.levelCount(); l++) {
                ImGui::PushID(l);
                ImGui::Bullet();
                ImGui::TextWrapped("%s (%s)", s.getLevelName(l).c_str(), s.getLevelFilename(l).c_str());
                ImGui::SameLine();
                if (ImGui::Button("Play level")) {
                    std::vector<File> levelFiles;
                    paths.at(i).findRecursiveFilesEndingWith(levelFiles,
                            getLastPathElement(s.getLevelFilename(l)));
                    if (levelFiles.size() == 0) {
                        Log::get(LOG_ERROR) << "Could not find level \""
                                            << getLastPathElement(s.getLevelFilename(l))
                                            << "\"!" << Log::endl;
                    } else {
                        Game::loadLevel(levelFiles.at(0).getPath());
                    }
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("", "%d Cut-Scenes", s.cutsceneCount())) {
            for (int c = 0; c < s.cutsceneCount(); c++) {
                ImGui::Bullet();
                ImGui::TextWrapped("%s", s.getCutsceneFilename(c).c_str());
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("", "%d Videos", s.videoCount())) {
            for (int v = 0; v < s.videoCount(); v++) {
                ImGui::Bullet();
                ImGui::TextWrapped("%s", s.getVideoFilename(v).c_str());
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("", "%d Pictures", s.pictureCount())) {
            for (int p = 0; p < s.pictureCount(); p++) {
                ImGui::Bullet();
                ImGui::TextWrapped("%s", s.getPictureFilename(p).c_str());
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("", "%d Titles", s.titleCount())) {
            for (int t = 0; t < s.titleCount(); t++) {
                ImGui::Bullet();
                ImGui::TextWrapped("%s", s.getTitleFilename(t).c_str());
            }
            ImGui::TreePop();
        }

        ImGui::NextColumn();
        ImGui::TextWrapped("Real Gameplay not yet implemented!");
        ImGui::NextColumn();

        if (i < (scripts.size() - 1))
            ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::Columns(1);

    if (scripts.size() == 0) {
        ImGui::TextWrapped("OpenRaiders built-in TombRaider-Script detection mechanism could not find any suitable TR2/TR3 script files (called \"TOMBPC.DAT\") in your PAK folder \"%s\"! Use the \"Select Level File\" Button to load a single level without starting the entire game.",
                           RunTime::getPakDir().c_str());
    }

    ImGui::End();
}

