/*!
 * \file include/system/FontTTF.h
 * \brief TrueType Font implementation
 *
 * \author xythobuz
 */

#ifndef _FONT_TTF_H_
#define _FONT_TTF_H_

#include <string>
#include <vector>

#include "stb/stb_truetype.h"
#include "system/Shader.h"

class FontMapTTF {
  public:
    FontMapTTF();
    FontMapTTF(FontMapTTF&& other);
    ~FontMapTTF();

    int initialize(unsigned char* fontData, int firstChar);
    bool contains(int c);
    int getTexture() { return texture; }
    void getQuad(int c, float* xpos, float* ypos, stbtt_aligned_quad *quad);

  private:
    int begin;
    int texture;
    stbtt_packedchar* charInfo;
};

class FontTTF {
  public:
    static int initialize(std::string f);
    static void shutdown();

    static unsigned int widthText(float scale, std::string s);
    static unsigned int heightText(float scale, unsigned int maxWidth, std::string s);

    static void drawText(unsigned int x, unsigned int y, float scale,
                         const unsigned char color[4], std::string s);
    static void drawTextWrapped(unsigned int x, unsigned int y, float scale,
                                const unsigned char color[4], unsigned int maxWidth, std::string s);

  private:
    static int charIsMapped(int c);
    static int getQuad(int c, float* xpos, float* ypos, stbtt_aligned_quad *quad);

    static unsigned char* fontData;
    static std::vector<FontMapTTF> maps;
    static ShaderBuffer vertexBuffer;
    static ShaderBuffer uvBuffer;
};

#endif

