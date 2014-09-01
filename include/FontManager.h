/*!
 * \file include/FontManager.h
 * \brief Font manager
 *
 * \author xythobuz
 */

#ifndef _FONT_MANAGER_H_
#define _FONT_MANAGER_H_

#include <vector>

#include "Font.h"

class FontManager : public Font {
public:

    /*!
     * \brief Constructs an object of FontManager
     */
    FontManager();

    /*!
     * \brief Deconstructs an object of FontManager
     */
    virtual ~FontManager();

    virtual int initialize();

    virtual unsigned int widthText(float scale, std::string s);

    virtual void drawText(unsigned int x, unsigned int y, float scale,
            const unsigned char color[4], std::string s);

    virtual unsigned int heightText(float scale, unsigned int maxWidth, std::string s);

    virtual void drawTextWrapped(unsigned int x, unsigned int y, float scale,
            const unsigned char color[4], unsigned int maxWidth, std::string s);

private:

    void add(Font *f, std::string e);

    int font;
    std::vector<Font *> fonts;
    std::vector<std::string> extensions;
};

#endif

