/*!
 * \file include/Sprite.h
 * \brief World Sprite
 *
 * \author xythobuz
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "math/math.h"

class Sprite {
public:
    Sprite(int _numVerts, vec3_t _vertex[4], vec2_t _texel[4], vec3_t _pos, vec_t _radius, int _texture);
    void display();

private:
    int numVerts;     //!< 4 == Quad, 3 == Triangle, rendered as triangles
    vec3_t vertex[4];
    vec2_t texel[4];
    vec3_t pos;
    vec_t radius;     //!< \fixme yeah, I know (I don't? --xythobuz)
    int texture;
};

class SpriteSequence {
public:
    ~SpriteSequence();

    void add(Sprite &s);
    unsigned int size();
    Sprite &get(unsigned int index);

private:
    std::vector<Sprite *> sprites;
};

#endif

