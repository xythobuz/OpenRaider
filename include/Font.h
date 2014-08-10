/*!
 * \file include/Font.h
 * \brief Font interface
 *
 * \author xythobuz
 */

#ifndef _FONT_H_
#define _FONT_H_

typedef struct {
    char *text;
    unsigned int x;
    unsigned int y;
    int w;
    int h;
    float scale;
    unsigned char color[4];
} FontString;

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

    virtual void writeString(FontString &s) = 0;

    virtual void drawText(unsigned int x, unsigned int y, float scale,
            const unsigned char color[4], const char *s, ...)
            __attribute__((format(printf, 6, 0)));

    virtual void drawTextCentered(unsigned int x, unsigned int y, float scale,
            const unsigned char color[4], unsigned int width, const char *s, ...)
            __attribute__((format(printf, 7, 0)));

protected:
    bool mFontInit;
    char *mFontName;
};

Font &getFont();

#endif

