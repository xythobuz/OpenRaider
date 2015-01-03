/*!
 * \file src/system/FontSDL.cpp
 * \brief SDL Font implementation
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
#include "TextureManager.h"
#include "system/Shader.h"
#include "system/FontSDL.h"

bool FontSDL::mFontInit = false;
TTF_Font* FontSDL::mFont = nullptr;
unsigned int FontSDL::mFontTexture = -1;
ShaderBuffer FontSDL::vertexBuffer;
ShaderBuffer FontSDL::uvBuffer;

void FontSDL::shutdown() {
    if (mFont != nullptr)
        TTF_CloseFont(mFont);
    mFont = nullptr;

    if (mFontInit) {
        TTF_Quit();
        mFontInit = false;
    }
}

int FontSDL::initialize(std::string font) {
    if (!mFontInit) {
        if (TTF_Init() != 0) {
            std::cout << "Could not initialize SDL-TTF!" << std::endl;
            return -1;
        }

        // Reserve slot
        mFontTexture = getTextureManager().loadBufferSlot(nullptr, 256, 256,
                       TextureManager::ColorMode::RGBA, 32, TextureManager::TextureStorage::SYSTEM);

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

    TextureManager::ColorMode textureFormat;
    unsigned int bpp = 0;
    if (surface->format->BytesPerPixel == 4) {
        if (surface->format->Rmask == 0x000000FF)
            textureFormat = TextureManager::ColorMode::RGBA;
        else
            textureFormat = TextureManager::ColorMode::BGRA;
        bpp = 32;
    } else {
        if (surface->format->Rmask == 0x000000FF)
            textureFormat = TextureManager::ColorMode::RGB;
        else
            textureFormat = TextureManager::ColorMode::BGR;
        bpp = 24;
    }

    getTextureManager().loadBufferSlot(static_cast<unsigned char*>(surface->pixels),
                                       surface->w, surface->h, textureFormat, bpp,
                                       TextureManager::TextureStorage::SYSTEM, mFontTexture);
    SDL_FreeSurface(surface);

    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;

    vertices.push_back(glm::vec2(x, y + h));
    vertices.push_back(glm::vec2(x, y));
    vertices.push_back(glm::vec2(x + w, y + h));

    vertices.push_back(glm::vec2(x + w, y));
    vertices.push_back(glm::vec2(x + w, y + h));
    vertices.push_back(glm::vec2(x, y));

    uvs.push_back(glm::vec2(0.0f, 1.0f));
    uvs.push_back(glm::vec2(0.0f, 0.0f));
    uvs.push_back(glm::vec2(1.0f, 1.0f));

    uvs.push_back(glm::vec2(1.0f, 0.0f));
    uvs.push_back(glm::vec2(1.0f, 1.0f));
    uvs.push_back(glm::vec2(0.0f, 0.0f));

    vertexBuffer.bufferData(vertices);
    uvBuffer.bufferData(uvs);
    Shader::drawGL(vertexBuffer, uvBuffer, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                   mFontTexture, TextureManager::TextureStorage::SYSTEM);
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

    TextureManager::ColorMode textureFormat;
    unsigned int bpp = 0;
    if (surface->format->BytesPerPixel == 4) {
        if (surface->format->Rmask == 0x000000FF)
            textureFormat = TextureManager::ColorMode::RGBA;
        else
            textureFormat = TextureManager::ColorMode::BGRA;
        bpp = 32;
    } else {
        if (surface->format->Rmask == 0x000000FF)
            textureFormat = TextureManager::ColorMode::RGB;
        else
            textureFormat = TextureManager::ColorMode::BGR;
        bpp = 24;
    }

    getTextureManager().loadBufferSlot(static_cast<unsigned char*>(surface->pixels),
                                       surface->w, surface->h, textureFormat, bpp,
                                       TextureManager::TextureStorage::SYSTEM, mFontTexture);
    SDL_FreeSurface(surface);

    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;

    vertices.push_back(glm::vec2(x, y + h));
    vertices.push_back(glm::vec2(x, y));
    vertices.push_back(glm::vec2(x + w, y + h));

    vertices.push_back(glm::vec2(x + w, y));
    vertices.push_back(glm::vec2(x + w, y + h));
    vertices.push_back(glm::vec2(x, y));

    uvs.push_back(glm::vec2(0.0f, 1.0f));
    uvs.push_back(glm::vec2(0.0f, 0.0f));
    uvs.push_back(glm::vec2(1.0f, 1.0f));

    uvs.push_back(glm::vec2(1.0f, 0.0f));
    uvs.push_back(glm::vec2(1.0f, 1.0f));
    uvs.push_back(glm::vec2(0.0f, 0.0f));

    vertexBuffer.bufferData(vertices);
    uvBuffer.bufferData(uvs);
    Shader::drawGL(vertexBuffer, uvBuffer, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                   mFontTexture, TextureManager::TextureStorage::SYSTEM);
}

