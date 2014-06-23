/*!
 * \file src/Font.cpp
 * \brief Font implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/strings.h"
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

void Font::drawText(unsigned int x, unsigned int y, float scale, const unsigned char color[4], const char *s, ...) {
    FontString tempText;
    va_list args;
    va_start(args, s);

    tempText.text = new char[256];
    tempText.scale = scale;
    tempText.w = 0;
    tempText.h = 0;
    tempText.x = x;
    tempText.y = y;
    tempText.color[0] = color[0];
    tempText.color[1] = color[1];
    tempText.color[2] = color[2];
    tempText.color[3] = color[3];

    vsnprintf(tempText.text, 256, s, args);
    tempText.text[255] = '\0';
    writeString(tempText);

    delete [] tempText.text;
    va_end(args);
}

