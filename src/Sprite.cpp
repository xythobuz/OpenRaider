/*!
 * \file src/Sprite.cpp
 * \brief World Sprite
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "main.h"
#include "Sprite.h"

SpriteSequence::~SpriteSequence() {
    for (unsigned int i = 0; i < sprites.size(); i++)
        delete &sprites.at(i);
}

void SpriteSequence::add(Sprite &s) {
    sprites.push_back(&s);
}

unsigned int SpriteSequence::size() {
    return sprites.size();
}

Sprite &SpriteSequence::get(unsigned int index) {
    return *sprites.at(index);
}

Sprite::Sprite(int _numVerts, vec3_t _vertex[4], vec2_t _texel[4], vec3_t _pos, vec_t _radius, int _texture) {
    numVerts = _numVerts;
    radius = _radius;
    texture = _texture;
    for (unsigned int i = 0; i < 3; i++) {
        vertex[0][i] = _vertex[0][i];
        vertex[1][i] = _vertex[1][i];
        vertex[2][i] = _vertex[2][i];
        vertex[3][i] = _vertex[3][i];
        pos[i] = _pos[i];
        if (i < 2) {
            texel[0][i] = _texel[0][i];
            texel[1][i] = _texel[1][i];
            texel[2][i] = _texel[2][i];
            texel[3][i] = _texel[3][i];
        }
    }
}

void Sprite::display() {
    if (!getRender().isVisible(pos[0], pos[1], pos[2],
                radius))
        return;

    glPushMatrix();
    glTranslated(pos[0], pos[1], pos[2]);

    // Sprites must always face camera, because they have no depth =)
    glRotated(OR_RAD_TO_DEG(getCamera().getRadianYaw()), 0, 1, 0);

    switch (getRender().getMode()) {
        // No vertex lighting on sprites, as far as I see in specs
        // So just draw normal texture, no case 2
        case Render::modeSolid:
            glBegin(GL_TRIANGLE_STRIP);
            glColor3f(texel[0][0], texel[0][1], 0.5);
            glVertex3fv(vertex[0]);

            glColor3f(texel[1][0], texel[1][1], 0.5);
            glVertex3fv(vertex[1]);

            glColor3f(texel[3][0], texel[3][1], 0.5);
            glVertex3fv(vertex[3]);

            glColor3f(texel[2][0], texel[2][1], 0.5);
            glVertex3fv(vertex[2]);
            glEnd();
            break;
        case Render::modeWireframe:
            glColor3fv(CYAN);
            glBegin(GL_LINE_LOOP);
            glVertex3fv(vertex[0]);
            glVertex3fv(vertex[1]);
            glVertex3fv(vertex[2]);
            glVertex3fv(vertex[3]);
            glEnd();
            glColor3fv(WHITE);
            break;
        default:
            glBindTexture(GL_TEXTURE_2D, texture+1);

            glBegin(GL_TRIANGLE_STRIP);
            glTexCoord2fv(texel[0]);
            glVertex3fv(vertex[0]);
            glTexCoord2fv(texel[1]);
            glVertex3fv(vertex[1]);
            glTexCoord2fv(texel[3]);
            glVertex3fv(vertex[3]);
            glTexCoord2fv(texel[2]);
            glVertex3fv(vertex[2]);
            glEnd();
    }

    glPopMatrix();
}

