/*!
 * \file src/Render.cpp
 * \brief OpenRaider Renderer class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <algorithm>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "global.h"
#include "Camera.h"
#include "Game.h"
#include "utils/strings.h"
#include "utils/tga.h"
#include "World.h"
#include "system/Window.h"
#include "Render.h"

RenderMode Render::mode = RenderMode::Disabled;
std::vector<Room*> Render::roomList;
bool Render::displayViewFrustum = false;

RenderMode Render::getMode() {
    return mode;
}

void Render::setMode(RenderMode m) {
    mode = m;
    switch (mode) {
        case RenderMode::Disabled:
            break;
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
        drawTexture(0.0f, 0.0f, getWindow().getWidth(), getWindow().getHeight(),
                    color, TEXTURE_SPLASH, TextureManager::TextureStorage::SYSTEM);
        return;
    } else if (mode == RenderMode::Disabled) {
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
        std::cout << "Rendering " << roomList.size() << "/"
                  << getWorld().sizeRoom() << " rooms..." << std::endl;
    }

    glm::mat4 projection = Camera::getProjectionMatrix();
    glm::mat4 view = Camera::getViewMatrix();

    for (auto r : roomList) {
        r->display(view, projection);
    }

    if (displayViewFrustum)
        Camera::displayFrustum(projection * view);

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
        std::cout << "Camera not found!" << std::endl;
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
    /*
    int sz = getWindow().getWidth() * getWindow().getHeight();
    unsigned char* image = new unsigned char[sz * 3];
    static int count = 0;
    bool done = false;

    assert(filenameBase != nullptr);

    // Don't overwrite files
    std::ostringstream filename;
    while (!done) {
        filename << filenameBase << "-" << count++ << ".tga";

        FILE* f = fopen(filename.str().c_str(), "rb");
        if (f) {
            fclose(f);
        } else {
            done = true;
        }
    }

    glReadPixels(0, 0, getWindow().getWidth(), getWindow().getHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE,
                 image);
    tgaSave(filename.str().c_str(), image, getWindow().getWidth(), getWindow().getHeight(), 0);
    delete [] image;
    */
}

void Render::drawTexture(float x, float y, float w, float h, glm::vec4 color,
                         unsigned int texture, TextureManager::TextureStorage s) {
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

    //! \fixme drawTextGL only uses SYSTEM textures!
    assert(s == TextureManager::TextureStorage::SYSTEM);

    Window::drawTextGL(vertices, uvs, color, texture);
}

