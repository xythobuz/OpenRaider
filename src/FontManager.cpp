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

    font = -1;
}

FontManager::~FontManager() {
    while (!fonts.empty()) {
        delete fonts.back();
        fonts.pop_back();
    }
}

void FontManager::add(Font *f, const char *e) {
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

    fonts.at(font)->setFont(mFontName);
    return fonts.at(font)->initialize();
}

void FontManager::writeString(FontString &s) {
    assert(font != -1);
    fonts.at(font)->writeString(s);
}

