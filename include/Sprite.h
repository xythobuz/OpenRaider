/*!
 * \file include/Sprite.h
 * \brief World Sprite
 *
 * \author xythobuz
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "math/math.h"
#include "TombRaider.h"

class Sprite {
public:
    Sprite(TombRaider &tr, unsigned int room, unsigned int index);
    Sprite(TombRaider &tr, unsigned int item, unsigned int sequence, unsigned int index);
    void display();

private:
    vec3_t vertex[4];
    vec2_t texel[4];
    vec3_t pos;
    vec_t radius;     //!< \fixme yeah, I know (I don't? --xythobuz)
    int texture;
};

class SpriteSequence {
public:
    SpriteSequence(TombRaider &tr, unsigned int item, unsigned int sequence);
    ~SpriteSequence();

    void add(Sprite &s);
    unsigned int size();
    Sprite &get(unsigned int index);

private:
    std::vector<Sprite *> sprites;
};

#endif

