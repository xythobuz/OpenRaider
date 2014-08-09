/*!
 * \file include/Menu.h
 * \brief Menu 'overlay'
 *
 * \author xythobuz
 */

#ifndef _MENU_H_
#define _MENU_H_

#include <memory>

#include "Font.h"
#include "utils/Folder.h"

/*!
 * \brief Menu 'overlay'
 */
class Menu {
public:

    /*!
     * \brief Constructs an object of Menu
     */
    Menu();

    /*!
     * \brief Deconstructs an object of Menu
     */
    ~Menu();

    int initialize();

    void setVisible(bool visible);

    bool isVisible();

    void display();

    void handleKeyboard(KeyboardButton key, bool pressed);

    void handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released);

private:

    void play();

    bool mVisible;
    long mCursor;
    long mMin;

    FontString mainText; //!< Used to draw heading centered

    Folder *mapFolder;
};

Menu &getMenu();

#endif
