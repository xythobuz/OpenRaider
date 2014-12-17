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
#include "Render.h"
#include "utils/strings.h"
#include "utils/tga.h"
#include "World.h"
#include "system/Window.h"

RenderMode Render::mode = RenderMode::Disabled;

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
            //glDisable(GL_TEXTURE_2D);
            break;
        default:
            //glClearColor(BLACK[0] / 256.0f, BLACK[1] / 256.0f,
            //             BLACK[2] / 256.0f, BLACK[3] / 256.0f);
            //glEnable(GL_TEXTURE_2D);
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

    glm::mat4 view = Camera::getViewMatrix();

    static unsigned int w = getWindow().getWidth();
    static unsigned int h = getWindow().getHeight();
    static glm::mat4 projection = glm::perspective(45.0f, // Field of View
                                  (float)getWindow().getWidth()
                                  / (float)getWindow().getHeight(),
                                  0.1f, // Min Distance
                                  100000.0f); // Max Distance

    if ((w != getWindow().getWidth()) || (h != getWindow().getHeight())) {
        w = getWindow().getWidth();
        h = getWindow().getHeight();
        glm::mat4 projection = glm::perspective(45.0f, // Field of View
                                                (float)getWindow().getWidth() / (float)getWindow().getHeight(),
                                                0.1f, // Min Distance
                                                100000.0f); // Max Distance
    }

    // Just draw all rooms, as a test
    for (int i = 0; i < getWorld().sizeRoom(); i++)
        getWorld().getRoom(i).display(view, projection);

    if (mode == RenderMode::Wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Render::screenShot(const char* filenameBase) {
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

    //glReadPixels(0, 0, getWindow().getWidth(), getWindow().getHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE,
    //             image);
    //tgaSave(filename.str().c_str(), image, getWindow().getWidth(), getWindow().getHeight(), 0);
    delete [] image;
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

