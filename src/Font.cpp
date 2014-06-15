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

    delete [] tempText.text;
    tempText.text = NULL;
}

void Font::setFont(const char *font) {
    assert(font != NULL);
    assert(font[0] != '\0');
    assert(mFontInit == false);

    mFontName = fullPath(font, 0);
}

void Font::drawText(unsigned int x, unsigned int y, float scale, const float color[4], const char *s, ...) {
    va_list args;
    va_start(args, s);
    vsnprintf(tempText.text, 256, s, args);
    tempText.text[255] = '\0';
    va_end(args);

    tempText.scale = scale;
    tempText.x = x;
    tempText.y = y;
    if (color) {
        tempText.color[0] = color[0];
        tempText.color[1] = color[1];
        tempText.color[2] = color[2];
        tempText.color[3] = color[3];
    }
    writeString(tempText);
}

