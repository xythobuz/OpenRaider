/*!
 * \file src/FontSDL.cpp
 * \brief SDL Font implementation
 *
 * \author xythobuz
 */

#include <stdio.h>

#include "global.h"
#include "FontSDL.h"

FontSDL::FontSDL() {
    mFont = NULL;
    mFontInit = false;
    mFontName = NULL;
    mFontTexture = 0;
}

FontSDL::~FontSDL() {
    if (mFont)
        TTF_CloseFont(mFont);

    if (mFontInit) {
        TTF_Quit();
        glDeleteTextures(1, &mFontTexture);
    }
}

int FontSDL::initialize() {
    assert(mFontInit == false);
    assert(mFontName != NULL);
    assert(mFontName[0] != '\0');

    if (TTF_Init() != 0) {
        printf("Could not initialize SDL-TTF!\n");
        return -1;
    }

    mFont = TTF_OpenFont(mFontName, 24);
    if (mFont == NULL) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return -2;
    }

    glGenTextures(1, &mFontTexture);

    mFontInit = true;
    return 0;
}

void FontSDL::writeString(FontString &s) {
    assert(mFontInit == true);
    assert(s.text != NULL);

    SDL_Color color;
    color.r = s.color[0];
    color.g = s.color[1];
    color.b = s.color[2];
    color.a = s.color[3];

    SDL_Surface *surface = TTF_RenderUTF8_Blended(mFont, s.text, color);
    if (surface == NULL) {
        printf("TTF_RenderUTF8_Blended Error: %s\n", TTF_GetError());
        return;
    }

    s.w = (int)((float)surface->w * s.scale);
    s.h = (int)((float)surface->h * s.scale);

    GLenum textureFormat;
    if (surface->format->BytesPerPixel == 4) {
        if (surface->format->Rmask == 0x000000FF)
            textureFormat = GL_RGBA;
        else
            textureFormat = GL_BGRA_EXT;
    } else {
        if (surface->format->Rmask == 0x000000FF)
            textureFormat = GL_RGB;
        else
            textureFormat = GL_BGR_EXT;
    }

    glBindTexture(GL_TEXTURE_2D, mFontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);

    GLuint xMin = s.x;
    GLuint yMin = s.y;
    GLuint xMax = xMin + s.w;
    GLuint yMax = yMin + s.h;

    glColor4ubv(s.color);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(xMin, yMin);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2i(xMin, yMax);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(xMax, yMax);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2i(xMax, yMin);
    glEnd();
}

