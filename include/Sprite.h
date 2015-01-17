/*!
 * \file include/Sprite.h
 * \brief World Sprite
 *
 * \author xythobuz
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "system/Shader.h"

class Sprite {
  public:
    Sprite(int tile, int x, int y, int width, int height);
    void display(glm::mat4 MVP);

  private:
    int texture;
    ShaderBuffer vertices, uvs;
};

class SpriteSequence {
  public:
    SpriteSequence(int objectID, int offset, int size)
        : id(objectID), start(offset), length(size) { }

  private:
    int id;
    int start;
    int length;
};

#endif

