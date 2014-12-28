/*!
 * \file include/system/FontTRLE.h
 * \brief Tomb Raider Level Editor Font loader
 *
 * \author xythobuz
 */

#ifndef _FONT_TRLE_H_
#define _FONT_TRLE_H_

#include <vector>
#include <glm/vec2.hpp>

/*!
 * \brief Tomb Raider Level Editor Font loader
 */
class FontTRLE {
  public:
    static void shutdown();

    static int initialize(std::string font);

    static unsigned int widthText(float scale, std::string s);

    static unsigned int heightText(float scale, unsigned int maxWidth, std::string s);

    static void drawText(unsigned int x, unsigned int y, float scale,
                         const unsigned char color[4], std::string s);

    static void drawTextWrapped(unsigned int x, unsigned int y, float scale,
                                const unsigned char color[4], unsigned int maxWidth, std::string s);

  private:
    static void setDefaultOffsets();
    static void loadLPS(std::string f);
    static void writeChar(unsigned int index, unsigned int xDraw, unsigned int yDraw, float scale,
                          std::vector<glm::vec2>& vertices, std::vector<glm::vec2>& uvs);

    static bool mFontInit;
    static unsigned int mFontTexture;

    // 106 entries: (x, y, w, h, offset)
    static int offsets[106][5];
    static int defaultOffsets[106][5];
};

#endif

