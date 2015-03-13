/*!
 * \file src/system/FontTTF.cpp
 * \brief TrueType Font Implementation
 *
 * \author xythobuz
 */

#include <fstream>

#include "global.h"
#include "Log.h"
#include "TextureManager.h"
#include "utils/pixel.h"
#include "system/FontTTF.h"

#define MAP_WIDTH 512
#define MAP_HEIGHT 512
#define FONT_SIZE 33
#define MAP_NUM_CHARS 100

FontMapTTF::FontMapTTF() : begin(-1), texture(-1), charInfo(nullptr) { }

FontMapTTF::FontMapTTF(FontMapTTF&& other) {
    begin = other.begin;
    texture = other.texture;
    other.begin = other.texture = -1;
    charInfo = other.charInfo;
    other.charInfo = nullptr;
}

FontMapTTF::~FontMapTTF() {
    if (charInfo != nullptr) {
        delete [] charInfo;
    }

    if (texture > -1) {
        //! \fixme Free the TextureManager buffer slot
    }
}

int FontMapTTF::initialize(unsigned char* fontData, int firstChar) {
    stbtt_pack_context context;
    unsigned char* pixels = new unsigned char[MAP_WIDTH * MAP_HEIGHT];
    if (!stbtt_PackBegin(&context, pixels, MAP_WIDTH, MAP_HEIGHT, 0, 1, nullptr)) {
        delete [] pixels;
        Log::get(LOG_ERROR) << "Error initializing font map in stbtt_PackBegin!" << Log::endl;
        return -1;
    }

    stbtt_PackSetOversampling(&context, 2, 2);

    if (charInfo != nullptr)
        delete [] charInfo;

    charInfo = new stbtt_packedchar[MAP_NUM_CHARS];
    if (!stbtt_PackFontRange(&context, fontData, 0, FONT_SIZE, firstChar, MAP_NUM_CHARS, charInfo)) {
        stbtt_PackEnd(&context);
        delete [] pixels;
        delete [] charInfo;
        charInfo = nullptr;
        Log::get(LOG_ERROR) << "Error packing font map!" << Log::endl;
        return -2;
    }

    stbtt_PackEnd(&context);
    unsigned char* rgb = grayscale2rgba(pixels, MAP_WIDTH, MAP_HEIGHT);
    delete [] pixels;

    texture = TextureManager::loadBufferSlot(rgb, MAP_WIDTH, MAP_HEIGHT, ColorMode::RGBA,
              32, TextureStorage::SYSTEM, texture);
    delete [] rgb;
    if (texture < 0) {
        delete [] charInfo;
        charInfo = nullptr;
        Log::get(LOG_ERROR) << "Error loading new font map texture!" << Log::endl;
        return -3;
    }

    begin = firstChar;
    return 0;
}

bool FontMapTTF::contains(int c) {
    assert(begin >= 0);
    return (begin >= 0) && (c >= begin) && (c < (begin + MAP_NUM_CHARS));
}

void FontMapTTF::getQuad(int c, float* xpos, float* ypos, stbtt_aligned_quad* quad) {
    assert(contains(c));
    stbtt_GetPackedQuad(charInfo, MAP_WIDTH, MAP_HEIGHT, c - begin, xpos, ypos, quad, 0);
}

// ----------------------------------------------------------------------------

unsigned char* FontTTF::fontData = nullptr;
std::vector<FontMapTTF> FontTTF::maps;
ShaderBuffer FontTTF::vertexBuffer;
ShaderBuffer FontTTF::uvBuffer;

int FontTTF::initialize(std::string f) {
    assert(f.length() > 0);

    std::ifstream file(f, std::ios::binary);
    if (!file) {
        Log::get(LOG_ERROR) << "Couldn't open file \"" << f << "\"!" << Log::endl;
        return -1;
    }

    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    assert(size > 0);
    file.seekg(0);

    maps.clear();
    if (fontData != nullptr) {
        delete [] fontData;
        fontData = nullptr;
    }

    fontData = new unsigned char[size];
    if (!file.read(reinterpret_cast<char*>(fontData), size)) {
        Log::get(LOG_ERROR) << "Couldn't read data in \"" << f << "\"" << Log::endl;
        delete [] fontData;
        fontData = nullptr;
        return -2;
    }

    maps.emplace_back();
    if (maps.at(0).initialize(fontData, ' ') < 0) {
        delete [] fontData;
        fontData = nullptr;
        return -3;
    }

    return 0;
}

void FontTTF::shutdown() {
    if (fontData != nullptr) {
        delete [] fontData;
        fontData = nullptr;
    }

    maps.clear();
}

unsigned int FontTTF::widthText(float scale, std::string s) {
    float x = 0.0f, y = 0.0f;
    stbtt_aligned_quad q;
    for (int i = 0; i < s.length(); i++) {
        if ((s[i] < 0x20) || (s[i] == 0x7F)) {
            continue;
        }

        getQuad(s[i], &x, &y, &q);
    }
    return x * scale;
}

void FontTTF::drawText(unsigned int x, unsigned int y, float scale,
                       glm::vec4 color, std::string s) {
    drawTextInternal(x, y, scale, color, 0, s, false);
}

unsigned int FontTTF::heightText(float scale, unsigned int maxWidth, std::string s) {
    float x = 0.0f, y = FONT_SIZE;
    stbtt_aligned_quad q;
    for (int i = 0; i < s.length(); i++) {
        if ((s[i] < 0x20) || (s[i] == 0x7F)) {
            continue;
        }

        getQuad(s[i], &x, &y, &q);
        if (x > maxWidth) {
            x = 0.0f;
            y += FONT_SIZE;
        }
    }
    return y * scale;
}

void FontTTF::drawTextWrapped(unsigned int x, unsigned int y, float scale,
                              glm::vec4 color, unsigned int maxWidth, std::string s) {
    drawTextInternal(x, y, scale, color, maxWidth, s, true);
}

void FontTTF::drawTextInternal(unsigned int x, unsigned int y, float scale,
                               glm::vec4 color, unsigned int maxWidth, std::string s,
                               bool drawWrapped) {
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;
    int texture = -1;
    float xpos = x, ypos = y + (FONT_SIZE * scale);
    for (int i = 0; i < s.length(); i++) {
        if ((s[i] < 0x20) || (s[i] == 0x7F)) {
            continue;
        }

        stbtt_aligned_quad quad;
        int tex = getQuad(s[i], &xpos, &ypos, &quad);

        if (drawWrapped && (xpos > (x + maxWidth))) {
            xpos = x;
            ypos += FONT_SIZE * scale;
            if (s[i] != ' ')
                i--;
            continue;
        }

        if ((texture != tex) && (texture != -1)) {
            vertexBuffer.bufferData(vertices);
            uvBuffer.bufferData(uvs);
            Shader::drawGL(vertexBuffer, uvBuffer, color, texture);
            vertices.clear();
            uvs.clear();
        }

        texture = tex;

        float xmin = quad.x0;
        float xmax = quad.x0 + ((quad.x1 - quad.x0) * scale);
        float ymin = quad.y1;
        float ymax = quad.y1 + ((quad.y0 - quad.y1) * scale);

        vertices.emplace_back(xmin, ymin);
        vertices.emplace_back(xmin, ymax);
        vertices.emplace_back(xmax, ymax);
        vertices.emplace_back(xmax, ymin);
        vertices.emplace_back(xmin, ymin);
        vertices.emplace_back(xmax, ymax);

        uvs.emplace_back(quad.s0, quad.t1);
        uvs.emplace_back(quad.s0, quad.t0);
        uvs.emplace_back(quad.s1, quad.t0);
        uvs.emplace_back(quad.s1, quad.t1);
        uvs.emplace_back(quad.s0, quad.t1);
        uvs.emplace_back(quad.s1, quad.t0);
    }

    vertexBuffer.bufferData(vertices);
    uvBuffer.bufferData(uvs);
    Shader::drawGL(vertexBuffer, uvBuffer, color, texture);
}

int FontTTF::charIsMapped(int c) {
    for (int i = 0; i < maps.size(); i++) {
        if (maps.at(i).contains(c)) {
            return i;
        }
    }

    int begin = c;
    if (c >= (MAP_NUM_CHARS / 2))
        begin -= (MAP_NUM_CHARS / 2);

    Log::get(LOG_INFO) << "Unmapped character '" << char(c) << "', new map from " << begin << " to "
                       << begin + MAP_NUM_CHARS - 1 << "..." << Log::endl;

    int p = maps.size();
    maps.emplace_back();
    if (maps.at(p).initialize(fontData, begin) < 0) {
        return -1;
    }

    return p;
}

int FontTTF::getQuad(int c, float* xpos, float* ypos, stbtt_aligned_quad* quad) {
    if (c < 0) {
        //! \fixme This has nothing to do with proper UTF8 support...
        c += 128;
    }

    int map = charIsMapped(c);
    assert(map >= 0);
    maps.at(map).getQuad(c, xpos, ypos, quad);
    return maps.at(map).getTexture();
}

