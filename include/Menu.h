/*!
 * \file include/Menu.h
 * \brief Menu 'overlay'
 *
 * \author xythobuz
 */

#ifndef _MENU_H_
#define _MENU_H_

#include "Window.h"

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

    void setVisible(bool visible);

    bool isVisible();

    void display();

    void handleKeyboard(KeyboardButton key, bool pressed);

    void handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released);

private:

#ifdef WIN32
    void loadPakFolderHelper(std::vector<char *> &list);
#endif

    void loadPakFolderRecursive(const char *dir);

    void fillMapList();

    void displayMapList();

    void play();

    bool mVisible;
    unsigned int mCursor;
    unsigned int mMin;

    WindowString mainText;

    bool mMapListFilled;
    bool mFirstPass;
    std::vector<char *> mMapList;
};

Menu &getMenu();

#endif
