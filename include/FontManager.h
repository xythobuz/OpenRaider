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

    virtual void writeString(FontString &s);

private:

    void add(Font *f, const char *e);

    int font;
    std::vector<Font *> fonts;
    std::vector<const char *> extensions;
};

#endif

