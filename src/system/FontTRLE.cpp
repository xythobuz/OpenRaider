/*!
 * \file src/system/FontTRLE.cpp
 * \brief Tomb Raider Level Editor Font loader
 *
 * \author xythobuz
 */

#include <fstream>
#include <sstream>

#include "global.h"
#include "TextureManager.h"
#include "utils/strings.h"
#include "system/Shader.h"
#include "system/FontTRLE.h"

#define SCALING 2.0f

bool FontTRLE::mFontInit = false;
unsigned int FontTRLE::mFontTexture = 0;
int FontTRLE::offsets[106][5];
ShaderBuffer FontTRLE::vertexBuffer;
ShaderBuffer FontTRLE::uvBuffer;

void FontTRLE::shutdown() {
}

int FontTRLE::initialize(std::string font) {
    assert(stringEndsWith(font, ".pc") == true);

    shutdown();

    // Load .pc file...
    std::ifstream file(font, std::ios::in | std::ios::binary);
    unsigned char* pixels = new unsigned char[256 * 256 * 4];
    if (!file.read((char*)pixels, 256 * 256 * 4)) {
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

    mFontTexture = TextureManager::loadBufferSlot(pixels, 256, 256,
                   ColorMode::BGRA, 32,
                   TextureStorage::SYSTEM);
    delete [] pixels;

    // Try to load .lps file or use default glyph positions
    std::string lpsFile = findAndReplace(font, ".pc", ".lps");
    loadLPS(lpsFile);

    mFontInit = true;
    return 0;
}

void FontTRLE::loadLPS(std::string f) {
    std::ifstream file(f);
    if (!file) {
        std::copy(&defaultOffsets[0][0], &defaultOffsets[0][0] + (106 * 5), &offsets[0][0]);
        return;
    }

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

void FontTRLE::writeChar(unsigned int index, unsigned int xDraw, unsigned int yDraw, float scale,
                         std::vector<glm::vec2>& vertices, std::vector<glm::vec2>& uvs) {
    assert(mFontInit == true);

    float width = ((float)offsets[index][2]) * scale * SCALING;
    float height = ((float)offsets[index][3]) * scale * SCALING;
    float offset = ((float)offsets[index][4]) * scale * SCALING;

    // screen coordinates
    float xMin = xDraw;
    float yMin = yDraw + offset + (10.0f * scale * SCALING);
    float xMax = xMin + width;
    float yMax = yMin + height;

    // texture part
    float txMin = ((float)offsets[index][0]) / 256.0f;
    float txMax = ((float)(offsets[index][0] + offsets[index][2])) / 256.0f;
    float tyMin = ((float)offsets[index][1]) / 256.0f;
    float tyMax = ((float)(offsets[index][1] + offsets[index][3])) / 256.0f;

    vertices.push_back(glm::vec2(xMin, yMax));
    vertices.push_back(glm::vec2(xMin, yMin));
    vertices.push_back(glm::vec2(xMax, yMax));

    vertices.push_back(glm::vec2(xMax, yMin));
    vertices.push_back(glm::vec2(xMax, yMax));
    vertices.push_back(glm::vec2(xMin, yMin));

    uvs.push_back(glm::vec2(txMin, tyMax));
    uvs.push_back(glm::vec2(txMin, tyMin));
    uvs.push_back(glm::vec2(txMax, tyMax));

    uvs.push_back(glm::vec2(txMax, tyMin));
    uvs.push_back(glm::vec2(txMax, tyMax));
    uvs.push_back(glm::vec2(txMin, tyMin));
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

    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;
    for (unsigned int i = 0; i < s.length(); i++) {
        // index into offset table
        int index = s[i] - '!';

        if (index == -1) // space
            x += (unsigned int)(14.0f * scale * SCALING);

        if ((index < 0) || (index > 105))
            continue; // skip unprintable chars

        writeChar((unsigned int)index, x, y, scale, vertices, uvs);
        x += (int)((float)(offsets[index][2] + 1) * scale * SCALING); // width
    }

    vertexBuffer.bufferData(vertices);
    uvBuffer.bufferData(uvs);
    glm::vec4 col(color[0] / 256.0f, color[1] / 256.0f, color[2] / 256.0f, color[3] / 256.0f);
    Shader::drawGL(vertexBuffer, uvBuffer, col, mFontTexture);
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

    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;
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

        writeChar((unsigned int)index, x, y, scale, vertices, uvs);
        x += (int)((float)(offsets[index][2] + 1) * scale * SCALING); // width
    }

    vertexBuffer.bufferData(vertices);
    uvBuffer.bufferData(uvs);
    glm::vec4 col(color[0] / 256.0f, color[1] / 256.0f, color[2] / 256.0f, color[3] / 256.0f);
    Shader::drawGL(vertexBuffer, uvBuffer, col, mFontTexture);
}

int FontTRLE::defaultOffsets[106][5] = {
    { 174,  52,   3,  12, -11 },
    {  98,  58,   6,   4, -10 },
    {  82,  26,  13,  11, -10 },
    {  78,  38,   9,  13, -10 },
    { 214,  13,  14,  11, -9  },
    {  40,  26,  13,  11, -10 },
    { 157,  57,   5,   6, -11 },
    { 204,  39,   5,  15, -12 },
    {  34,  40,   5,  15, -12 },
    { 184,  59,   4,   4, -11 },
    {  22,  40,  10,  10, -9  },
    { 178,  59,   4,   4, -2  },
    { 106,  60,   7,   2, -4  },
    { 114,  60,   4,   3, -2  },
    { 212,  38,   8,  14, -12 },
    {  88,  49,   9,   9, -8  },
    { 200,  55,   5,   9, -8  },
    {  46,  52,   8,   9, -8  },
    {  88,  38,   7,  10, -8  },
    {  62,  40,  10,  10, -8  },
    { 142,  48,   8,  11, -9  },
    { 232,  50,   8,  10, -9  },
    { 120,  47,   8,  11, -9  },
    {  22,  51,   8,  10, -9  },
    { 110,  49,   8,  10, -8  },
    { 152,  57,   4,   7, -7  },
    { 136,  57,   4,   9, -7  },
    { 178,  40,  11,   9, -8  },
    { 210,  53,  10,   6, -7  },
    { 240,  40,  11,   9, -7  },
    {  12,  39,   9,  12, -11 },
    {  66,  13,  15,  13, -10 },
    { 130,  13,  13,  12, -11 },
    { 214,  25,  12,  12, -11 },
    { 132,  35,  10,  12, -11 },
    {   0,  26,  12,  12, -11 },
    {  14,  26,  12,  12, -11 },
    {  66,  27,  11,  12, -11 },
    { 182,  27,  11,  12, -11 },
    { 200,  13,  13,  12, -11 },
    { 222,  54,   4,  12, -11 },
    {  56,  52,   4,  15, -11 },
    { 230,  15,  12,  12, -11 },
    { 144,  35,  10,  12, -11 },
    {  48,  13,  17,  12, -11 },
    { 144,  13,  13,  12, -11 },
    {  54,  26,  11,  12, -11 },
    { 200,  26,  11,  12, -11 },
    { 240,   0,  13,  14, -11 },
    { 158,  13,  13,  12, -11 },
    { 156,  35,  10,  12, -11 },
    { 172,  13,  13,  12, -11 },
    {  98,  13,  14,  12, -11 },
    {  82,  13,  14,  12, -11 },
    {  24,  13,  22,  12, -11 },
    { 186,  13,  12,  13, -11 },
    { 114,  13,  14,  12, -11 },
    { 228,  28,  11,  12, -11 },
    {  62,  60,   5,   3, -4  },
    { 248,  59,   5,   3, -4  },
    {  88,  59,   7,   3, -4  },
    { 142,  60,   6,   2, -3  },
    { 120,  59,   7,   3, -4  },
    { 242,  59,   4,   4, -11 },
    {  98,  49,  10,   8, -7  },
    {  96,  35,  10,  13, -12 },
    {  72,  52,   8,   8, -7  },
    {   0,  39,  10,  11, -10 },
    { 164,  52,   8,   8, -7  },
    { 168,  38,   9,  13, -12 },
    { 120,  35,  11,  11, -7  },
    { 108,  35,  10,  13, -12 },
    { 194,  27,   5,  11, -10 },
    {  40,  51,   5,  15, -10 },
    {  28,  26,  11,  13, -12 },
    {  82,  52,   5,  12, -11 },
    {  96,  26,  17,   8, -7  },
    { 152,  48,  11,   8, -7  },
    {  62,  51,   9,   8, -7  },
    { 244,  15,  10,  12, -7  },
    {  52,  39,   9,  12, -7  },
    {  10,  52,   9,   8, -7  },
    { 190,  52,   8,   8, -7  },
    {   0,  51,   8,  10, -9  },
    { 178,  50,  10,   8, -7  },
    { 130,  48,  11,   8, -7  },
    { 132,  26,  17,   8, -7  },
    { 242,  50,  10,   8, -7  },
    {  40,  38,  10,  12, -7  },
    { 232,  41,   7,   8, -7  },
    { 222,  41,   8,  12, -7  },
    { 130,  57,   5,   8, -7  },
    { 194,  39,   9,  12, -10 },
    {  32,  56,   4,  11, -10 },
    {   1,  14,  22,  11, -10 },
    { 192,   0,  23,  13, -10 },
    { 168,   0,  23,  12, -10 },
    { 216,   0,  23,  12, -10 },
    { 150,  26,  17,   8, -8  },
    { 168,  26,  11,  11, -9  },
    { 114,  26,  17,   8, -8  },
    { 240,  28,  12,  11, -9  },
    {   0,   0,  40,  12, -10 },
    {  84,   0,  39,  11, -10 },
    {  42,   0,  39,  11, -10 },
    { 126,   0,  39,  11, -10 },
};

