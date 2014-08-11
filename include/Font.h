/*!
 * \file include/Font.h
 * \brief Font interface
 *
 * \author xythobuz
 */

#ifndef _FONT_H_
#define _FONT_H_

#include  <string>

/*!
 * \brief Font interface
 */
class Font {
public:

    /*!
     * \brief Deconstructs an object of Font
     */
    virtual ~Font();

    virtual void setFont(const char *font);

    virtual int initialize() = 0;

    virtual unsigned int widthText(float scale, std::string s) = 0;

    virtual void drawText(unsigned int x, unsigned int y, float scale,
            const unsigned char color[4], std::string s) = 0;

    virtual unsigned int heightText(float scale, unsigned int maxWidth, std::string s) = 0;

    virtual void drawTextWrapped(unsigned int x, unsigned int y, float scale,
            const unsigned char color[4], unsigned int maxWidth, std::string s) = 0;

    // Implemented in Font.cpp using widthText & drawText
    virtual void drawTextCentered(unsigned int x, unsigned int y, float scale,
            const unsigned char color[4], unsigned int width, std::string s);

protected:
    bool mFontInit;
    char *mFontName;
};

Font &getFont();

#endif

