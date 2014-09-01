/*!
 * \file src/Font.cpp
 * \brief Font implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/strings.h"
#include "Window.h"
#include "Font.h"
#include "FontManager.h"

Font &getFont() {
    static FontManager gFont;
    return gFont;
}

Font::~Font() {
}

void Font::setFont(std::string font) {
    assert(mFontInit == false);
    mFontName = font;
}

void Font::drawTextCentered(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], unsigned int width, std::string s) {
    drawText(x + ((width / 2) - (widthText(scale, s) / 2)), y, scale, color, s);
}

