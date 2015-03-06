/*!
 * \file include/system/FontImGui.h
 * \brief Default Font implementation
 *
 * \author xythobuz
 */

#ifndef _FONT_IMGUI_H_
#define _FONT_IMGUI_H_

#include <string>

/*!
 * \brief Default Font implementation
 */
class FontImGui {
  public:
    static unsigned int widthText(float scale, std::string s);

    static unsigned int heightText(float scale, unsigned int maxWidth, std::string s);

    static void drawText(unsigned int x, unsigned int y, float scale,
                         glm::vec4 color, std::string s);

    static void drawTextWrapped(unsigned int x, unsigned int y, float scale,
                                glm::vec4 color, unsigned int maxWidth, std::string s);
};

#endif

