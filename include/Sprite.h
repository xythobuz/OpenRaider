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
    Sprite(uint16_t tile, uint8_t x, uint8_t y, uint16_t width, uint16_t height);

    Sprite(TombRaider& tr, unsigned int room, unsigned int index);

    Sprite(TombRaider& tr, unsigned int item, unsigned int sequence, unsigned int index) :
        Sprite((tr.Sprite() + ((tr.SpriteSequence() + sequence)->offset) + index)->tile,
        (tr.Sprite() + ((tr.SpriteSequence() + sequence)->offset) + index)->x,
        (tr.Sprite() + ((tr.SpriteSequence() + sequence)->offset) + index)->y,
        (tr.Sprite() + ((tr.SpriteSequence() + sequence)->offset) + index)->width,
        (tr.Sprite() + ((tr.SpriteSequence() + sequence)->offset) + index)->height) { }

    void display();
    void display(float x, float y, float w, float h);

  private:
    float vertex[4][3];
    float texel[4][2];
    //float pos[3];
    //float radius; //!< \fixme yeah, I know (I don't? --xythobuz)
    int texture;
};

class SpriteSequence {
  public:
    SpriteSequence(int32_t objectID);
    void add(Sprite s);

    SpriteSequence(TombRaider& tr, unsigned int item, unsigned int sequence);
    ~SpriteSequence();

    unsigned long size();
    Sprite& get(unsigned long index);

  private:
    int32_t id;
    std::vector<Sprite> sprites;

    std::vector<Sprite*> oldsprites;
};

#endif

