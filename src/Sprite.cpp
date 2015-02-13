/*!
 * \file src/Sprite.cpp
 * \brief World Sprite
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "Render.h"
#include "TextureManager.h"
#include "World.h"
#include "Sprite.h"

const static float scale = 4.0f;
const static float texelScale = 256.0f;
const static int texelOffset = 2;

Sprite::Sprite(int tile, int x, int y, int width, int height) : texture(tile) {
    width >>= 8;
    height >>= 8;

    int width2 = (int)(width * scale);
    int height2 = (int)(height * scale);

    std::vector<glm::vec2> uv;
    uv.emplace_back(float(x + texelOffset) / texelScale, float(y + height) / texelScale);
    uv.emplace_back(float(x + texelOffset) / texelScale, float(y + texelOffset) / texelScale);
    uv.emplace_back(float(x + width) / texelScale, float(y + texelOffset) / texelScale);
    uv.emplace_back(float(x + width) / texelScale, float(y + height) / texelScale);
    uv.emplace_back(uv.at(0));
    uv.emplace_back(uv.at(2));

    std::vector<glm::vec3> vert;
    vert.emplace_back(float(-width2) / 2.0f, 0.0f, 0.0f);
    vert.emplace_back(float(-width2) / 2.0f, float(-height2), 0.0f);
    vert.emplace_back(float(width2) / 2.0f, float(-height2), 0.0f);
    vert.emplace_back(float(width2) / 2.0f, 0.0f, 0.0f);
    vert.emplace_back(vert.at(0));
    vert.emplace_back(vert.at(2));

    vertices.bufferData(vert);
    uvs.bufferData(uv);

    uv2D = glm::vec4(uv.at(0), uv.at(2));
}

void Sprite::display(glm::mat4 MVP) {
    Shader::drawGL(vertices, uvs, texture, MVP);
}

// ----------------------------------------------------------------------------

void SpriteSequence::display(glm::mat4 MVP, int index) {
    assertGreaterThanEqual(index, 0);
    assertLessThan(index, length);
    getWorld().getSprite(start + index).display(MVP);
}

