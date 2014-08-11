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

Font::~Font() {
    delete [] mFontName;
    mFontName = NULL;
}

void Font::setFont(const char *font) {
    assert(font != NULL);
    assert(font[0] != '\0');
    assert(mFontInit == false);

    mFontName = fullPath(font, 0);
}

void Font::drawTextCentered(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], unsigned int width, std::string s) {
    drawText(x + ((width / 2) - (widthText(scale, s) / 2)), y, scale, color, s);
}

