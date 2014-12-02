/*!
 * \file src/Sprite.cpp
 * \brief World Sprite
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "Game.h"
#include "Render.h"
#include "TextureManager.h"
#include "Sprite.h"

SpriteSequence::SpriteSequence(int32_t objectID) {
    id = objectID;
}

void SpriteSequence::add(Sprite s) {
    sprites.push_back(s);
}

// -----------------------

SpriteSequence::SpriteSequence(TombRaider& tr, unsigned int item, unsigned int sequence) {
    for (int i = 0; i < (-tr.SpriteSequence()[sequence].negative_length); i++)
        oldsprites.push_back(new Sprite(tr, item, sequence, i));
}

SpriteSequence::~SpriteSequence() {
    for (unsigned int i = 0; i < oldsprites.size(); i++)
        delete oldsprites.at(i);
}

unsigned long SpriteSequence::size() {
    if (oldsprites.size() > 0)
        return oldsprites.size();
    else
        return sprites.size();
}

Sprite& SpriteSequence::get(unsigned long index) {
    if (oldsprites.size() > 0) {
        assert(index < oldsprites.size());
        return *oldsprites.at(index);
    } else {
        assert(index < sprites.size());
        return sprites.at(index);
    }
}

// ----------------------------------------------------------------------------

Sprite::Sprite(uint16_t tile, uint8_t x, uint8_t y, uint16_t width, uint16_t height) {
    /*!
     * \fixme TODO Can't do translation/visibility-check when rendering here.
     * We don't know xyz of this Sprite because it could be used by
     * different items at other coordinates in the world.
     * Do translation/visibility-check at item/room level.
     */

    const float scale = 4.0f;
    const float texelScale = 256.0f;

    width >>= 8;
    height >>= 8;
    int width2 = (int)(width * scale);
    int height2 = (int)(height * scale);

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

    texel[3][0] = (float)(x + width) / texelScale;
    texel[3][1] = (float)(y + height) / texelScale;

    texel[2][0] = (float)(x + width) / texelScale;
    texel[2][1] = (float)(y) / texelScale;

    texel[1][0] = (float)(x) / texelScale;
    texel[1][1] = (float)(y) / texelScale;

    texel[0][0] = (float)(x) / texelScale;
    texel[0][1] = (float)(y + height) / texelScale;

    texture = tile;
    //radius = width2 / 2.0f;
}

// -----------------------

Sprite::Sprite(TombRaider& tr, unsigned int room, unsigned int index) {
    float spriteVertices[12];
    float spriteTexCoords[8];
    float posUnused[3];

    tr.getRoomSprite(room, index,
                     10.0f, &texture, posUnused, spriteVertices, spriteTexCoords);

    for (unsigned int j = 0; j < 12; j++)
        vertex[j / 3][j % 3] = spriteVertices[j];

    for (unsigned int j = 0; j < 8; j++)
        texel[j / 2][j % 2] = spriteTexCoords[j];

    //radius = 0.0f;
}

void Sprite::display() {
    //if (!getRender().isVisible(pos[0], pos[1], pos[2], radius))
    //    return;

    glPushMatrix();
    //glTranslated(pos[0], pos[1], pos[2]);

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
            glColor3ubv(CYAN);
            glBegin(GL_LINE_LOOP);
            glVertex3fv(vertex[0]);
            glVertex3fv(vertex[1]);
            glVertex3fv(vertex[2]);
            glVertex3fv(vertex[3]);
            glEnd();
            glColor3ubv(WHITE);
            break;
        default:
            getTextureManager().bindTextureId(texture);

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

void Sprite::display(float x, float y, float w, float h) {
    float z = 0.0f;

    getTextureManager().bindTextureId(texture);

    glBegin(GL_QUADS);

    glTexCoord2fv(texel[1]);
    glVertex3f(x, y, z);

    glTexCoord2fv(texel[2]);
    glVertex3f(x + w, y, z);

    glTexCoord2fv(texel[3]);
    glVertex3f(x + w, y + h, z);

    glTexCoord2fv(texel[0]);
    glVertex3f(x, y + h, z);

    glEnd();
}

