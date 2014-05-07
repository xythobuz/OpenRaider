/*!
 * \file src/Sprite.cpp
 * \brief World Sprite
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif defined WIN32
#include <gl/glew.h>
#include <gl/wglew.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "main.h"
#include "Sprite.h"

SpriteSequence::SpriteSequence(TombRaider &tr, unsigned int item, unsigned int sequence) {
    for (int i = 0; i < (-tr.SpriteSequence()[sequence].negative_length); i++)
        add(*new Sprite(tr, item, sequence, i));
}

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

Sprite::Sprite(TombRaider &tr, unsigned int item, unsigned int sequence, unsigned int index) {
    tr2_item_t *i = tr.Item();
    tr2_sprite_texture_t *spriteTextures = tr.Sprite();
    tr2_sprite_sequence_t *spriteSequence = tr.SpriteSequence();

    //! \fixme index was unused?!
    tr2_sprite_texture_t *sprite = &spriteTextures[spriteSequence[sequence].offset + index];

    int width = sprite->width >> 8;
    int height = sprite->height >> 8;
    int x = sprite->x;
    int y = sprite->y;
    float scale = 4.0f;
    float texelScale = 256.0f;
    int width2 = (int)(width * scale);
    int height2 = (int)(height * scale);

    // For vising use
    pos[0] = i[item].x;
    pos[1] = i[item].y;
    pos[2] = i[item].z;

    vertex[0][0] = -width2 / 2.0f;
    vertex[1][0] = -width2 / 2.0f;
    vertex[2][0] = width2 / 2.0f;
    vertex[3][0] = width2 / 2.0f;

    vertex[0][1] = 0;
    vertex[1][1] = -height2;
    vertex[2][1] = -height2;
    vertex[3][1] = 0;

    vertex[0][2] = 0;
    vertex[1][2] = 0;
    vertex[2][2] = 0;
    vertex[3][2] = 0;

    texel[3][0] = (vec_t)(x+width)/texelScale;
    texel[3][1] = (vec_t)(y+height)/texelScale;

    texel[2][0] = (vec_t)(x+width)/texelScale;
    texel[2][1] = (vec_t)(y)/texelScale;

    texel[1][0] = (vec_t)(x) /texelScale;
    texel[1][1] = (vec_t)(y) /texelScale;

    texel[0][0] = (vec_t)(x) / texelScale;
    texel[0][1] = (vec_t)(y+height)/texelScale;

    texture = sprite->tile + getGame().getTextureStart();
    radius = width2 / 2.0f;
}

Sprite::Sprite(TombRaider &tr, unsigned int room, unsigned int index) {
    float spriteVertices[12];
    float spriteTexCoords[8];

    tr.getRoomSprite(room, index,
            10.0f, &texture, pos, spriteVertices, spriteTexCoords);

    texture += getGame().getTextureStart(); // OpenRaider preloads some textures

    for (unsigned int j = 0; j < 12; j++)
        vertex[j / 3][j % 3] = spriteVertices[j];

    for (unsigned int j = 0; j < 8; j++)
        texel[j / 2][j % 2] = spriteTexCoords[j];
}

void Sprite::display() {
    if (!getRender().isVisible(pos[0], pos[1], pos[2], radius))
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

