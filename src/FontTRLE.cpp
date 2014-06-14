/*!
 * \file src/FontTRLE.cpp
 * \brief SDL Font implementation
 *
 * \author xythobuz
 */

#include <cstdio>
#include <cstdarg>

#include "global.h"
#include "utils/strings.h"
#include "FontTRLE.h"

FontTRLE::FontTRLE() {
    mFontInit = false;
    mFontName = NULL;
    mFontTexture = 0;

    // TRLE defaults
    resolution1 = 240;
    resolution2 = 512;
    ruler1 = -16;
    ruler2 = -40;
    baselineAbs = 0; // always zero?
    spacing = 0.075f;
    squashedTextFactor = 0.75f;
    // offset table default is set in header

    tempText.text = new char[256];
    tempText.color[0] = 0xFF;
    tempText.color[1] = 0xFF;
    tempText.color[2] = 0xFF;
    tempText.color[3] = 0xFF;
    tempText.scale = 1.2f;
    tempText.w = 0;
    tempText.h = 0;
}

FontTRLE::~FontTRLE() {
    delete [] tempText.text;
    tempText.text = NULL;
}

int FontTRLE::initialize() {

    // tmp debug
    delete [] mFontName;
    mFontName = bufferString("/Users/thomas/Downloads/TR Fonts/TR4 Official/Font.pc");

    assert(mFontInit == false);
    assert(mFontName != NULL);
    assert(mFontName[0] != '\0');
    assert(stringEndsWith(mFontName, ".pc") == true);

    // Load .pc file into GL texture
    unsigned char *pixels = NULL;
    glGenTextures(1, &mFontTexture);
    glBindTexture(GL_TEXTURE_2D, mFontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

    // Try to load .lps file, overwriting defaults
    char *lpsFile = stringReplace(mFontName, ".pc", ".lps");
    loadLPS(lpsFile);
    delete [] lpsFile;

    mFontInit = true;
    return 0;
}

void FontTRLE::loadLPS(const char *file) {
    //! \todo load lps file
}

void FontTRLE::writeChar(unsigned int index, unsigned int xDraw, FontString &s) {
/*
    SDL_Color color;
    color.r = (unsigned char)(s.color[0] * 255.0f);
    color.g = (unsigned char)(s.color[1] * 255.0f);
    color.b = (unsigned char)(s.color[2] * 255.0f);
    color.a = (unsigned char)(s.color[3] * 255.0f);

    glBindTexture(GL_TEXTURE_2D, mFontTexture);

    GLuint xMin = s.x;
    GLuint yMin = s.y;
    GLuint xMax = xMin + s.w;
    GLuint yMax = yMin + s.h;

    glColor4f(color.r / 256.0f, color.g / 256.0f, color.b / 256.0f, color.a / 256.0f);

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
*/
}

void FontTRLE::writeString(FontString &s) {
    assert(mFontInit == true);
    assert(s.text != NULL);

    unsigned int x = s.x;

    for (unsigned int i = 0; s.text[i] != '\0'; i++) {
        // index into offset table
        int index = s.text[i] - ' ';

        if ((index < 0) || (index > 105))
            continue; // skip unprintable chars

        writeChar((unsigned int)index, x, s);
        x += offsets[index][2]; // width
    }

/*
    s.w = (int)((float)surface->w * s.scale);
    s.h = (int)((float)surface->h * s.scale);
*/
}

void FontTRLE::drawText(unsigned int x, unsigned int y, float scale, const float color[4], const char *s, ...) {
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

