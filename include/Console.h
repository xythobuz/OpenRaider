/*!
 * \file include/Console.h
 * \brief Console 'overlay'
 *
 * \author xythobuz
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "Window.h"

/*!
 * \brief Console 'overlay'
 */
class Console {
public:

    /*!
     * \brief Constructs an object of Console
     */
    Console();

    /*!
     * \brief Deconstructs an object of Console
     */
    ~Console();

    void setVisible(bool visible);

    bool isVisible();

    void display();

    void handleKeyboard(KeyboardButton key, bool pressed);

    void handleText(char *text, bool notFinished);

private:

    bool mVisible;
};

#endif
