/*!
 * \file include/Sprite.h
 * \brief 2D Billboard Collection
 *
 * \author xythobuz
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <vector>

#include "BoundingSphere.h"

class Sprite {
  public:
    Sprite(int tile, int x, int y, int width, int height);
    void display(glm::mat4 MVP);

    int getTexture() { return texture; }
    glm::vec4 getUVs() { return uv2D; }
    BoundingSphere& getBoundingSphere() { return boundingSphere; }

  private:
    int texture;
    std::vector<glm::vec3> vertexBuff;
    std::vector<glm::vec2> uvBuff;
    glm::vec4 uv2D;
    BoundingSphere boundingSphere;
};

class SpriteSequence {
  public:
    SpriteSequence(int objectID, int offset, int size)
        : id(objectID), start(offset), length(size) { }
    void display(glm::mat4 MVP, int index);

    int getID() { return id; }
    int getStart() { return start; }
    int size() { return length; }

  private:
    int id;
    int start;
    int length;
};

#endif

