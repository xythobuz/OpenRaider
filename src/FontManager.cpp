/*!
 * \file src/FontManager.cpp
 * \brief Font Manager
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/strings.h"
#include "FontManager.h"
#include "Font.h"
#include "FontTRLE.h"

#ifdef USING_SDL_FONT
#include "FontSDL.h"
#endif

FontManager::FontManager() {
    add(new FontTRLE(), ".pc");

#ifdef USING_SDL_FONT
    add(new FontSDL(), ".ttf");
#endif

    mFontInit = false;
    font = -1;
}

FontManager::~FontManager() {
    while (!fonts.empty()) {
        delete fonts.back();
        fonts.pop_back();
    }
}

void FontManager::add(Font *f, std::string e) {
    fonts.push_back(f);
    extensions.push_back(e);
}

int FontManager::initialize() {
    for (unsigned int i = 0; i < fonts.size(); i++) {
        if (stringEndsWith(mFontName, extensions.at(i))) {
            font = i;
            break;
        }
    }

    if (font == -1)
        return -1;

    mFontInit = true;
    fonts.at(font)->setFont(mFontName);
    return fonts.at(font)->initialize();
}

unsigned int FontManager::widthText(float scale, std::string s) {
    assert(font != -1);
    return fonts.at(font)->widthText(scale, s);
}

void FontManager::drawText(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], std::string s) {
    assert(font != -1);
    fonts.at(font)->drawText(x, y, scale, color, s);
}

unsigned int FontManager::heightText(float scale, unsigned int maxWidth, std::string s) {
    assert(font != -1);
    return fonts.at(font)->heightText(scale, maxWidth, s);
}

void FontManager::drawTextWrapped(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], unsigned int maxWidth, std::string s) {
    assert(font != -1);
    fonts.at(font)->drawTextWrapped(x, y, scale, color, maxWidth, s);
}

