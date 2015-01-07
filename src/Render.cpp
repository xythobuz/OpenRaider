/*!
 * \file src/Render.cpp
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <algorithm>
#include <sstream>

#include "global.h"
#include "Camera.h"
#include "Game.h"
#include "Log.h"
#include "UI.h"
#include "World.h"
#include "system/Shader.h"
#include "system/Window.h"
#include "utils/strings.h"
#include "Render.h"

#include <glm/gtc/matrix_transform.hpp>

#include "imgui/imgui.h"
#include "stb/stb_image_write.h"

RenderMode Render::mode = RenderMode::LoadScreen;
std::vector<Room*> Render::roomList;
bool Render::displayViewFrustum = false;

RenderMode Render::getMode() {
    return mode;
}

void Render::setMode(RenderMode m) {
    mode = m;
    switch (mode) {
        case RenderMode::Solid:
        case RenderMode::Wireframe:
            //glClearColor(PURPLE[0] / 256.0f, PURPLE[1] / 256.0f,
            //             PURPLE[2] / 256.0f, PURPLE[3] / 256.0f);
            break;
        default:
            //glClearColor(BLACK[0] / 256.0f, BLACK[1] / 256.0f,
            //             BLACK[2] / 256.0f, BLACK[3] / 256.0f);
            break;
    }
}

void Render::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mode == RenderMode::LoadScreen) {
        glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
        drawTexture(0.0f, 0.0f, Window::getSize().x, Window::getSize().y,
                    color, TEXTURE_SPLASH, TextureStorage::SYSTEM);
        return;
    }

    if (mode == RenderMode::Wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (Camera::update()) {
        clearRoomList();
        buildRoomList(-2); // TODO cache room
    }

    glm::mat4 projection = Camera::getProjectionMatrix();
    glm::mat4 view = Camera::getViewMatrix();
    glm::mat4 VP = projection * view;

    for (auto r : roomList) {
        r->display(VP);
    }

    if (displayViewFrustum)
        Camera::displayFrustum(VP);

    if (mode == RenderMode::Wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Render::clearRoomList() {
    roomList.clear();
}

void Render::buildRoomList(int room) {
    if (room < -1) {
        // Check if the camera currently is in a room...
        for (int i = 0; i < getWorld().sizeRoom(); i++) {
            if (getWorld().getRoom(i).getBoundingBox().inBox(Camera::getPosition())) {
                buildRoomList(i);
                return;
            }
        }
        buildRoomList(-1);
    } else if (room == -1) {
        // Check visibility for all rooms!
        for (int i = 0; i < getWorld().sizeRoom(); i++) {
            if (Camera::boxInFrustum(getWorld().getRoom(i).getBoundingBox())) {
                roomList.push_back(&getWorld().getRoom(i));
            }
        }
    } else {
        // Check visibility of room and connected rooms, recursively?
        if (Camera::boxInFrustum(getWorld().getRoom(room).getBoundingBox())) {
            roomList.push_back(&getWorld().getRoom(room));
            for (int i = 0; i < getWorld().getRoom(room).sizePortals(); i++) {
                int r = getWorld().getRoom(room).getPortal(i).getAdjoiningRoom();
                bool found = false;
                for (int n = 0; n < roomList.size(); n++) {
                    if (roomList.at(n) == &getWorld().getRoom(r)) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    buildRoomList(r);
            }
        }
    }
}

void Render::screenShot(const char* filenameBase) {
    assert(filenameBase != nullptr);

    int w = Window::getSize().x;
    int h = Window::getSize().y;
    int sz = w * h;
    unsigned char* image = new unsigned char[sz * 3];
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);

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
        getLog() << "Error saving image \"" << f << "\"!" << Log::endl;
    }

    delete [] image;
    delete [] buffer;
}

void Render::drawTexture(float x, float y, float w, float h, glm::vec4 color,
                         unsigned int texture, TextureStorage s) {
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;

    vertices.push_back(glm::vec2(x, y + h));
    vertices.push_back(glm::vec2(x, y));
    vertices.push_back(glm::vec2(x + w, y + h));

    vertices.push_back(glm::vec2(x + w, y));
    vertices.push_back(glm::vec2(x + w, y + h));
    vertices.push_back(glm::vec2(x, y));

    uvs.push_back(glm::vec2(0.0f, 1.0f));
    uvs.push_back(glm::vec2(0.0f, 0.0f));
    uvs.push_back(glm::vec2(1.0f, 1.0f));

    uvs.push_back(glm::vec2(1.0f, 0.0f));
    uvs.push_back(glm::vec2(1.0f, 1.0f));
    uvs.push_back(glm::vec2(0.0f, 0.0f));

    static ShaderBuffer vert, uv;
    vert.bufferData(vertices);
    uv.bufferData(uvs);

    Shader::drawGL(vert, uv, color, texture, s);
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
        if (ImGui::Combo("Mode", &item, modeStrings, modeStringCount)) {
            if (item == 0)
                mode = RenderMode::LoadScreen;
            else if (item == 1)
                mode = RenderMode::Texture;
            else if (item == 2)
                mode = RenderMode::Wireframe;
            else if (item == 3)
                mode = RenderMode::Solid;
        }

        bool updateViewFrustum = Camera::getUpdateViewFrustum();
        if (ImGui::Checkbox("Update Frustum##render", &updateViewFrustum)) {
            Camera::setUpdateViewFrustum(updateViewFrustum);
        }
        ImGui::SameLine();
        ImGui::Checkbox("Show Frustum##render", &displayViewFrustum);
    }
}

