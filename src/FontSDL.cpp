/*!
 * \file src/FontSDL.cpp
 * \brief SDL Font implementation
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
#include "FontSDL.h"

bool FontSDL::mFontInit = false;
TTF_Font* FontSDL::mFont = nullptr;
unsigned int FontSDL::mFontTexture = 0;

void FontSDL::shutdown() {
    if (mFont != nullptr)
        TTF_CloseFont(mFont);
    mFont = nullptr;

    if (mFontInit) {
        TTF_Quit();
        glDeleteTextures(1, &mFontTexture);
        mFontInit = false;
    }
}

int FontSDL::initialize(std::string font) {
    if (!mFontInit) {
        if (TTF_Init() != 0) {
            std::cout << "Could not initialize SDL-TTF!" << std::endl;
            return -1;
        }

        glGenTextures(1, &mFontTexture);
        mFontInit = true;
    }

    if (mFont != nullptr)
        TTF_CloseFont(mFont);

    mFont = TTF_OpenFont(font.c_str(), 24);
    if (mFont == nullptr) {
        std::cout << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        return -2;
    }

    return 0;
}

unsigned int FontSDL::widthText(float scale, std::string s) {
    assert(mFontInit == true);
    assert(mFont != nullptr);

    int w;
    if (TTF_SizeUTF8(mFont, s.c_str(), &w, nullptr) != 0) {
        std::cout << "TTF_SizeUTF8 Error: " << TTF_GetError() << std::endl;
        return s.length() * 15 * scale;
    }
    return w * scale;
}

void FontSDL::drawText(unsigned int x, unsigned int y, float scale,
                       const unsigned char color[4], std::string s) {
    assert(mFontInit == true);
    assert(mFont != nullptr);
    assert(s.length() > 0);

    SDL_Color col;
    col.r = color[0];
    col.g = color[1];
    col.b = color[2];
    col.a = color[3];

    SDL_Surface* surface = TTF_RenderUTF8_Blended(mFont, s.c_str(), col);
    if (surface == nullptr) {
        std::cout << "TTF_RenderUTF8_Blended Error: " << TTF_GetError() << std::endl;
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
    glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0,
                 textureFormat, GL_UNSIGNED_BYTE, surface->pixels);
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
    assert(mFont != nullptr);
    assert(s.length() > 0);
    assert(maxWidth > 0);

    SDL_Color col;
    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(mFont, s.c_str(), col, maxWidth);
    if (surface == nullptr) {
        std::cout << "TTF_RenderUTF8_Blended_Wrapped Error: " << TTF_GetError() << std::endl;
        return 0;
    }
    int h = surface->h * scale;
    SDL_FreeSurface(surface);
    return h;
}

void FontSDL::drawTextWrapped(unsigned int x, unsigned int y, float scale,
                              const unsigned char color[4], unsigned int maxWidth, std::string s) {
    assert(mFontInit == true);
    assert(mFont != nullptr);
    assert(s.length() > 0);
    assert(maxWidth > 0);

    SDL_Color col;
    col.r = color[0];
    col.g = color[1];
    col.b = color[2];
    col.a = color[3];

    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(mFont, s.c_str(), col, maxWidth);
    if (surface == nullptr) {
        std::cout << "TTF_RenderUTF8_Blended_Wrapped Error: " << TTF_GetError() << std::endl;
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
    glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0,
                 textureFormat, GL_UNSIGNED_BYTE, surface->pixels);
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

