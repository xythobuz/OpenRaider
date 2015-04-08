/*!
 * \file src/Render.cpp
 * \brief World Renderer
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <fstream>
#include <sstream>

#include "global.h"
#include "Camera.h"
#include "Log.h"
#include "Menu.h"
#include "StaticMesh.h"
#include "World.h"
#include "system/Shader.h"
#include "system/Window.h"
#include "Render.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glbinding/gl/gl33.h>

#include "imgui/imgui.h"
#include "stb/stb_image_write.h"

RenderMode Render::mode = RenderMode::LoadScreen;
std::vector<Room*> Render::roomList;
bool Render::displayViewFrustum = false;
bool Render::displayVisibilityCheck = false;

void Render::display() {
    gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

    if (mode == RenderMode::LoadScreen) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(Window::getSize().x, Window::getSize().y));
        ImGui::Begin("SplashWindow", nullptr, ImGuiWindowFlags_NoTitleBar
                     | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                     | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
                     | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
        auto bm = TextureManager::getBufferManager(TEXTURE_SPLASH, TextureStorage::SYSTEM);
        ImGui::Image(bm, ImVec2(Window::getSize().x, Window::getSize().y));
        ImGui::End();
        return;
    }

    if (mode == RenderMode::Wireframe) {
        gl::glPolygonMode(gl::GL_FRONT_AND_BACK, gl::GL_LINE);
    } else {
        gl::glPolygonMode(gl::GL_FRONT_AND_BACK, gl::GL_FILL);
    }

    bool updated = Camera::update();
    glm::mat4 projection = Camera::getProjectionMatrix();
    glm::mat4 view = Camera::getViewMatrix();
    glm::mat4 VP = projection * view;

    if (updated || displayVisibilityCheck) {
        int r = Camera::getRoom();
        clearRoomList();
        if (r < 0) {
            buildRoomList(VP);
        } else {
            buildRoomList(VP, r);
        }
    }

    for (int r = roomList.size() - 1; r >= 0; r--) {
        roomList.at(r)->display(VP);

        for (int i = 0; i < World::sizeEntity(); i++) {
            auto& e = World::getEntity(i);
            if (roomList.at(r)->getIndex() == e.getRoom()) {
                e.display(VP);
            }
        }
    }

    if (displayViewFrustum)
        Camera::displayFrustum(VP);

    if (mode == RenderMode::Wireframe) {
        gl::glPolygonMode(gl::GL_FRONT_AND_BACK, gl::GL_FILL);
    }
}

void Render::buildRoomList(glm::mat4 VP, int room, glm::vec2 min, glm::vec2 max) {
    if (room < -1) {
        // Check if the camera currently is in a room...
        for (int i = 0; i < World::sizeRoom(); i++) {
            if (World::getRoom(i).getBoundingBox().inBox(Camera::getPosition())) {
                buildRoomList(VP, i);
                return;
            }
        }
        buildRoomList(VP, -1);
    } else if (room == -1) {
        // Check visibility for all rooms!
        for (int i = 0; i < World::sizeRoom(); i++) {
            if (Camera::boxInFrustum(World::getRoom(i).getBoundingBox())) {
                roomList.push_back(&World::getRoom(i));
            }
        }
    } else {
        roomList.push_back(&World::getRoom(room));

        if (displayVisibilityCheck) {
            // Display the visibility test for the portal to this room
            BoundingBox debugBox(glm::vec3(min, 0.0f), glm::vec3(max, 0.0f));
            debugBox.display(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // Check all portals leading from this room to somewhere else
        for (int i = 0; i < World::getRoom(room).sizePortals(); i++) {
            auto& portal = World::getRoom(room).getPortal(i);
            auto& room = World::getRoom(portal.getAdjoiningRoom());

            // Calculate the 2D screen-space bounding box of this portal
            glm::vec3 newMin, newMax;
            for (int c = 0; c < 4; c++) {
                glm::vec3 vert = portal.getVertex(c);
                glm::vec4 result = VP * glm::vec4(vert, 1.0f);
                vert = glm::vec3(result) / result.w;

                if (c == 0) {
                    newMin = vert;
                    newMax = vert;
                } else {
                    if (vert.x < newMin.x)
                        newMin.x = vert.x;
                    if (vert.y < newMin.y)
                        newMin.y = vert.y;
                    if (vert.z < newMin.z)
                        newMin.z = vert.z;
                    if (vert.x > newMax.x)
                        newMax.x = vert.x;
                    if (vert.y > newMax.y)
                        newMax.y = vert.y;
                    if (vert.z > newMax.z)
                        newMax.z = vert.z;
                }
            }

            //! \fixme Currently also checking behind player, because Z is always 1.0f?!
            //if ((newMin.z > 0.0f) || (newMin.z < -1.0f) || (newMax.z > 0.0f) || (newMax.z < -1.0f)) {
            //    continue;
            //}

            // Check if the portal intersects the portal leading into this room
            if (!((min.x < newMax.x) && (max.x > newMin.x)
                && (min.y < newMax.y) && (max.y > newMin.y))) {
                continue;
            }

            // Check if the connected room is in our view frustum (could be visible)
            if (!Camera::boxInFrustum(room.getBoundingBox())) {
                continue;
            }

            // Check if this room is already in the list...
            bool found = false;
            for (int n = 0; n < roomList.size(); n++) {
                if (roomList.at(n) == &room) {
                    found = true;
                    break;
                }
            }

            // ...only render it if it is not
            if (!found) {
                buildRoomList(VP, portal.getAdjoiningRoom(), glm::vec2(newMin), glm::vec2(newMax));
            }
        }
    }
}

void Render::screenShot(const char* filenameBase) {
    orAssert(filenameBase != nullptr);

    int w = Window::getSize().x;
    int h = Window::getSize().y;
    int sz = w * h;
    unsigned char* image = new unsigned char[sz * 3];
    gl::glReadPixels(0, 0, w, h, gl::GL_RGB, gl::GL_UNSIGNED_BYTE, image);

    unsigned char* buffer = new unsigned char[sz * 3];
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            buffer[3 * (x + (y * w))] = image[3 * (x + ((h - y - 1) * w))];
            buffer[(3 * (x + (y * w))) + 1] = image[(3 * (x + ((h - y - 1) * w))) + 1];
            buffer[(3 * (x + (y * w))) + 2] = image[(3 * (x + ((h - y - 1) * w))) + 2];
        }
    }

    // Don't overwrite files
    static int count = 0;
    bool done = false;
    std::string f;
    while (!done) {
        std::ostringstream filename;
        filename << filenameBase << "-" << count++ << ".png";
        f = filename.str();
        std::ifstream fs(f);
        done = !fs.good();
        fs.close();
    }

    if (!stbi_write_png(f.c_str(), w, h, 3, buffer, 0)) {
        Log::get(LOG_ERROR) << "Error saving image \"" << f << "\"!" << Log::endl;
    }

    delete [] image;
    delete [] buffer;
}

static const int modeStringCount = 4;
static const char* modeStrings[modeStringCount] = {
    "Splash", "Texture", "Wireframe", "Solid"
};

void Render::displayUI() {
    if (ImGui::CollapsingHeader("Render Settings##render")) {
        int item = 0;
        if (mode == RenderMode::Texture)
            item = 1;
        else if (mode == RenderMode::Wireframe)
            item = 2;
        else if (mode == RenderMode::Solid)
            item = 3;
        if (ImGui::Combo("Render Mode##render", &item, modeStrings, modeStringCount)) {
            if (item == 0)
                mode = RenderMode::LoadScreen;
            else if (item == 1)
                mode = RenderMode::Texture;
            else if (item == 2)
                mode = RenderMode::Wireframe;
            else if (item == 3)
                mode = RenderMode::Solid;
        }

        ImGui::Separator();
        ImGui::Text("Camera:");
        bool updateViewFrustum = Camera::getUpdateViewFrustum();
        if (ImGui::Checkbox("Update Frustum##render", &updateViewFrustum)) {
            Camera::setUpdateViewFrustum(updateViewFrustum);
        }
        ImGui::SameLine();
        ImGui::Checkbox("Show Frustum##render", &displayViewFrustum);
        ImGui::SameLine();
        bool showOverlay = Camera::getShowOverlay();
        if (ImGui::Checkbox("Overlay", &showOverlay)) {
            Camera::setShowOverlay(showOverlay);
        }
        glm::vec3 camPos = Camera::getPosition();
        if (ImGui::SliderFloat3("Position", &camPos.x, -100000, 100000)) {
            Camera::setPosition(camPos);
        }

        ImGui::Separator();
        ImGui::Text("Bounding Boxes:");
        bool showBoundingBox = Room::getShowBoundingBox();
        if (ImGui::Checkbox("Rooms##bbox", &showBoundingBox)) {
            Room::setShowBoundingBox(showBoundingBox);
        }
        ImGui::SameLine();
        bool showBoundingBox2 = StaticMesh::getShowBoundingBox();
        if (ImGui::Checkbox("StaticMeshes##bbox", &showBoundingBox2)) {
            StaticMesh::setShowBoundingBox(showBoundingBox2);
        }
        ImGui::SameLine();
        bool showBoundingBox3 = Portal::getShowBoundingBox();
        if (ImGui::Checkbox("Portals##bbox", &showBoundingBox3)) {
            Portal::setShowBoundingBox(showBoundingBox3);
        }
        ImGui::SameLine();
        ImGui::Checkbox("VisChecks##bbox", &displayVisibilityCheck);

        ImGui::Separator();
        ImGui::Text("Renderable Objects:");
        ImGui::Text("Room: ");
        ImGui::SameLine();
        bool showRoomGeometry = Room::getShowRoomGeometry();
        if (ImGui::Checkbox("Geometry##renderroom", &showRoomGeometry)) {
            Room::setShowRoomGeometry(showRoomGeometry);
        }
        ImGui::SameLine();
        bool showRoomModels = Room::getShowRoomModels();
        if (ImGui::Checkbox("Models##renderroom", &showRoomModels)) {
            Room::setShowRoomModels(showRoomModels);
        }
        ImGui::SameLine();
        bool showRoomSprites = Room::getShowRoomSprites();
        if (ImGui::Checkbox("Sprites##renderroom", &showRoomSprites)) {
            Room::setShowRoomSprites(showRoomSprites);
        }

        ImGui::Text("Entity: ");
        ImGui::SameLine();
        bool showEntitySprites = Entity::getShowEntitySprites();
        if (ImGui::Checkbox("Sprites##renderentity", &showEntitySprites)) {
            Entity::setShowEntitySprites(showEntitySprites);
        }
        ImGui::SameLine();
        bool showEntityMeshes = Entity::getShowEntityMeshes();
        if (ImGui::Checkbox("Meshes##renderentity", &showEntityMeshes)) {
            Entity::setShowEntityMeshes(showEntityMeshes);
        }
        ImGui::SameLine();
        bool showEntityModels = Entity::getShowEntityModels();
        if (ImGui::Checkbox("Models##renderentity", &showEntityModels)) {
            Entity::setShowEntityModels(showEntityModels);
        }

        ImGui::Separator();
        if (ImGui::Button("New Splash##render")) {
            TextureManager::initializeSplash();
        }
        ImGui::SameLine();
        if (ImGui::Button("New Menu##render")) {
            Menu::initialize();
        }
    }
}

