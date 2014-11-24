/*!
 * \file include/Sprite.h
 * \brief World Sprite
 *
 * \author xythobuz
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <cstdint>

#include "loader/Loader.h"
#include "TombRaider.h"

class Sprite {
  public:
    Sprite(struct vertex_t vert, uint16_t tex);

    // Old API
    Sprite(TombRaider& tr, unsigned int room, unsigned int index);
    Sprite(TombRaider& tr, unsigned int item, unsigned int sequence, unsigned int index);
    void display();

  private:
    float vertex[4][3];
    float texel[4][2];
    float pos[3];
    float radius; //!< \fixme yeah, I know (I don't? --xythobuz)
    int texture;
};

class SpriteSequence {
  public:
    SpriteSequence(TombRaider& tr, unsigned int item, unsigned int sequence);
    ~SpriteSequence();

    unsigned long size();
    Sprite& get(unsigned long index);

  private:
    std::vector<Sprite*> sprites;
};

#endif

