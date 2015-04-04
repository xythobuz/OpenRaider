/*!
 * \file src/Sprite.cpp
 * \brief 2D Billboard Collection
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

    uvBuff.emplace_back(float(x + texelOffset) / texelScale, float(y + height) / texelScale);
    uvBuff.emplace_back(float(x + texelOffset) / texelScale, float(y + texelOffset) / texelScale);
    uvBuff.emplace_back(float(x + width) / texelScale, float(y + texelOffset) / texelScale);
    uvBuff.emplace_back(float(x + width) / texelScale, float(y + height) / texelScale);
    uvBuff.emplace_back(uvBuff.at(0));
    uvBuff.emplace_back(uvBuff.at(2));

    vertexBuff.emplace_back(float(-width2) / 2.0f, 0.0f, 0.0f);
    vertexBuff.emplace_back(float(-width2) / 2.0f, float(-height2), 0.0f);
    vertexBuff.emplace_back(float(width2) / 2.0f, float(-height2), 0.0f);
    vertexBuff.emplace_back(float(width2) / 2.0f, 0.0f, 0.0f);
    vertexBuff.emplace_back(vertexBuff.at(0));
    vertexBuff.emplace_back(vertexBuff.at(2));

    uv2D = glm::vec4(uvBuff.at(0), uvBuff.at(2));
}

void Sprite::display(glm::mat4 MVP) {
    Shader::drawGL(vertexBuff, uvBuff, MVP, texture, TextureStorage::GAME);
}

// ----------------------------------------------------------------------------

void SpriteSequence::display(glm::mat4 MVP, int index) {
    orAssertGreaterThanEqual(index, 0);
    orAssertLessThan(index, length);
    World::getSprite(start + index).display(MVP);
}

