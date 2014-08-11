/*!
 * \file src/FontTRLE.cpp
 * \brief SDL Font implementation
 *
 * \author xythobuz
 */

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "global.h"
#include "utils/strings.h"
#include "FontTRLE.h"

FontTRLE::FontTRLE() {
    mFontInit = false;
    mFontName = NULL;
    mFontTexture = 0;
}

FontTRLE::~FontTRLE() {
}

int FontTRLE::initialize() {
    assert(mFontInit == false);
    assert(mFontName != NULL);
    assert(mFontName[0] != '\0');
    assert(stringEndsWith(mFontName, ".pc") == true);

    // Load .pc file...
    std::ifstream file(mFontName, std::ios::in | std::ios::binary);
    unsigned char *pixels = new unsigned char[256 * 256 * 4];
    if (!file.read((char *)pixels, 256 * 256 * 4)) {
        delete [] pixels;
        return -1;
    }

    // Fix coloring
    for (unsigned int i = 0; i < (256 * 256 * 4); i += 4) {
        float y = (0.2126f * pixels[i + 2]);
        y += (0.7152f * pixels[i + 1]);
        y += (0.0722f * pixels[i]);
        pixels[i] = pixels[i + 1] = pixels[i + 2] = (unsigned char)y;
    }

    // ...into GL texture
    glGenTextures(1, &mFontTexture);
    glBindTexture(GL_TEXTURE_2D, mFontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
    delete [] pixels;

    // Try to load .lps file, overwriting default glyph positions
    char *lpsFile = stringReplace(mFontName, ".pc", ".lps");
    loadLPS(lpsFile);
    delete [] lpsFile;

    mFontInit = true;
    return 0;
}

void FontTRLE::loadLPS(const char *f) {
    std::ifstream file(f);
    if (!file)
        return;

    /*!
     * \todo This is probably the worlds most unreliable parser...
     */

    for (std::string line; std::getline(file, line);) {
        std::istringstream stream(line);
        int index;
        stream >> index;
        if (stream.get() != '=')
            continue;
        stream >> offsets[index][0];
        if (stream.get() != ',')
            continue;
        stream >> offsets[index][1];
        if (stream.get() != ',')
            continue;
        stream >> offsets[index][2];
        if (stream.get() != ',')
            continue;
        stream >> offsets[index][3];
        if (stream.get() != ',')
            continue;
        stream >> offsets[index][4];
    }
}

#define SCALING 2.0f

void FontTRLE::writeChar(unsigned int index, unsigned int xDraw, unsigned int yDraw,
        float scale, const unsigned char color[4]) {
    assert(mFontInit == true);

    int width = (int)(((float)offsets[index][2]) * scale * SCALING);
    int height = (int)(((float)offsets[index][3]) * scale * SCALING);
    int offset = (int)(((float)offsets[index][4]) * scale * SCALING);

    // screen coordinates
    int xMin = xDraw;
    int yMin = ((int)yDraw) + offset + (int)(10.0f * scale * SCALING);
    int xMax = xMin + width;
    int yMax = yMin + height;

    // texture part
    float txMin = ((float)offsets[index][0]) / 256.0f;
    float txMax = ((float)(offsets[index][0] + offsets[index][2])) / 256.0f;
    float tyMin = ((float)offsets[index][1]) / 256.0f;
    float tyMax = ((float)(offsets[index][1] + offsets[index][3])) / 256.0f;

    // draw
    glBindTexture(GL_TEXTURE_2D, mFontTexture);
    glColor4f(color[0] * 256.0f, color[1] * 256.0f, color[2] * 256.0f, color[3] * 256.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(txMin, tyMin);
    glVertex2i(xMin, yMin);
    glTexCoord2f(txMin, tyMax);
    glVertex2i(xMin, yMax);
    glTexCoord2f(txMax, tyMax);
    glVertex2i(xMax, yMax);
    glTexCoord2f(txMax, tyMin);
    glVertex2i(xMax, yMin);
    glEnd();
}

unsigned int FontTRLE::widthText(float scale, std::string s) {
    assert(mFontInit == true);
    assert(s.length() > 0);

    unsigned int width = 0;
    for (unsigned int i = 0; i < s.length(); i++) {
        // index into offset table
        int index = s[i] - '!';

        if (index == -1) // space
            width += (unsigned int)(14.0f * scale * SCALING);

        if ((index < 0) || (index > 105))
            continue; // skip unprintable chars

        width += (float)(offsets[index][2] + 1) * scale * SCALING; // glyph width
    }

    return width;
}

void FontTRLE::drawText(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], std::string s) {
    assert(mFontInit == true);
    assert(s.length() > 0);

    for (unsigned int i = 0; i < s.length(); i++) {
        // index into offset table
        int index = s[i] - '!';

        if (index == -1) // space
            x += (unsigned int)(14.0f * scale * SCALING);

        if ((index < 0) || (index > 105))
            continue; // skip unprintable chars

        writeChar((unsigned int)index, x, y, scale, color);
        x += (int)((float)(offsets[index][2] + 1) * scale * SCALING); // width
    }
}

unsigned int FontTRLE::heightText(float scale, unsigned int maxWidth, std::string s) {
    assert(mFontInit == true);
    assert(s.length() > 0);

    unsigned int x = 0;
    unsigned int yMax = 0;
    unsigned int yReturn = 0;

    for (unsigned int i = 0; i < s.length(); i++) {
        // index into offset table
        int index = s[i] - '!';

        if (index == -1) // space
            x += (unsigned int)(14.0f * scale * SCALING);

        if ((index < 0) || (index > 105))
            continue; // skip unprintable chars

        if (yMax < (unsigned int)(((float)offsets[index][3]) * scale * SCALING))
            yMax = (unsigned int)(((float)offsets[index][3]) * scale * SCALING);

        x += (int)((float)(offsets[index][2] + 1) * scale * SCALING); // width
        if (x > maxWidth) {
            // go to the next line
            yReturn += yMax + 2;
            yMax = 0;
            x = (int)((float)(offsets[index][2] + 1) * scale * SCALING);
        }
    }

    return yReturn + yMax + 2;
}

void FontTRLE::drawTextWrapped(unsigned int x, unsigned int y, float scale,
        const unsigned char color[4], unsigned int maxWidth, std::string s) {
    assert(mFontInit == true);
    assert(s.length() > 0);

    unsigned int xStart = x;
    unsigned int yMax = 0;

    for (unsigned int i = 0; i < s.length(); i++) {
        // index into offset table
        int index = s[i] - '!';

        if (index == -1) // space
            x += (unsigned int)(14.0f * scale * SCALING);

        if ((index < 0) || (index > 105))
            continue; // skip unprintable chars

        if (yMax < (unsigned int)(((float)offsets[index][3]) * scale * SCALING))
            yMax = (unsigned int)(((float)offsets[index][3]) * scale * SCALING);

        x += (int)((float)(offsets[index][2] + 1) * scale * SCALING); // width
        if ((x - xStart) > maxWidth) {
            // go to the next line
            y += yMax + 2;
            yMax = 0;
            x = xStart;
        } else {
            x -= (int)((float)(offsets[index][2] + 1) * scale * SCALING);
        }

        writeChar((unsigned int)index, x, y, scale, color);
        x += (int)((float)(offsets[index][2] + 1) * scale * SCALING); // width
    }
}

