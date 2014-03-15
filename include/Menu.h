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

private:
    bool mVisible;
    WindowString mainText;
};

#endif
