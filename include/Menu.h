/*!
 * \file include/Menu.h
 * \brief Menu 'overlay'
 *
 * \author xythobuz
 */

#ifndef _MENU_H_
#define _MENU_H_

#include <functional>
#include <memory>

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
    int initialize(std::string folder);
    int initialize(Folder *folder);

    void setVisible(bool visible);

    bool isVisible();

    void display();

    void handleKeyboard(KeyboardButton key, bool pressed);

    void handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released);

    void handleMouseScroll(int xrel, int yrel);

private:

    void loadOrOpen();
    void showDialog(std::string msg, std::string btn1, std::string btn2,
            std::function<int (bool state)> callback = std::function<int (bool)>());
    void ackDialog();
    void displayDialog();

    bool mVisible;
    long mCursor;
    long mMin;

    Folder *mapFolder;
    bool hiddenState;

    std::string dialogText;
    std::string dialogButton1;
    std::string dialogButton2;
    bool dialogState;
    std::function<int (bool state)> dialogFunction;
};

Menu &getMenu();

#endif
