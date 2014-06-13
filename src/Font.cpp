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

