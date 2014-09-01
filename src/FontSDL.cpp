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

    if (TTF_Init() != 0) {
        printf("Could not initialize SDL-TTF!\n");
        return -1;
    }

    mFont = TTF_OpenFont(mFontName.c_str(), 24);
    if (mFont == NULL) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return -2;
    }

    glGenTextures(1, &mFontTexture);

    mFontInit = true;
    return 0;
}

unsigned int FontSDL::widthText(float scale, std::string s) {
    assert(mFontInit == true);

    int w;
    if (TTF_SizeUTF8(mFont, s.c_str(), &w, NULL) != 0) {
        printf("TTF_SizeUTF8 Error: %s\n", TTF_GetError());
        return s.length() * 15 * scale;
    }
    return w * scale;
}

void FontSDL::drawText(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], std::string s) {
    assert(mFontInit == true);
    assert(s.length() > 0);

    SDL_Color col;
    col.r = color[0];
    col.g = color[1];
    col.b = color[2];
    col.a = color[3];

    SDL_Surface *surface = TTF_RenderUTF8_Blended(mFont, s.c_str(), col);
    if (surface == NULL) {
        printf("TTF_RenderUTF8_Blended Error: %s\n", TTF_GetError());
        return;
    }

    int w = (int)((float)surface->w * scale);
    int h = (int)((float)surface->h * scale);

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

    GLuint xMin = x;
    GLuint yMin = y;
    GLuint xMax = xMin + w;
    GLuint yMax = yMin + h;

    glColor4ubv(color);

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

unsigned int FontSDL::heightText(float scale, unsigned int maxWidth, std::string s) {
    assert(mFontInit == true);
    assert(s.length() > 0);
    assert(maxWidth > 0);

    SDL_Color col;
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(mFont, s.c_str(), col, maxWidth);
    if (surface == NULL) {
        printf("TTF_RenderUTF8_Blended_Wrapped Error: %s\n", TTF_GetError());
        return 0;
    }
    int h = surface->h * scale;
    SDL_FreeSurface(surface);
    return h;
}

void FontSDL::drawTextWrapped(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], unsigned int maxWidth, std::string s) {
    assert(mFontInit == true);
    assert(s.length() > 0);
    assert(maxWidth > 0);

    SDL_Color col;
    col.r = color[0];
    col.g = color[1];
    col.b = color[2];
    col.a = color[3];

    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(mFont, s.c_str(), col, maxWidth);
    if (surface == NULL) {
        printf("TTF_RenderUTF8_Blended_Wrapped Error: %s\n", TTF_GetError());
        return;
    }

    int w = (int)((float)surface->w * scale);
    int h = (int)((float)surface->h * scale);

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

    GLuint xMin = x;
    GLuint yMin = y;
    GLuint xMax = xMin + w;
    GLuint yMax = yMin + h;

    glColor4ubv(color);

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

