/*!
 * \file include/MenuFolder.h
 * \brief File-Explorer like Menu
 *
 * \author xythobuz
 */

#ifndef _MENU_FOLDER_H_
#define _MENU_FOLDER_H_

#include "Menu.h"
#include "utils/Folder.h"

/*!
 * \brief Menu 'overlay'
 */
class MenuFolder : public Menu {
public:

    /*!
     * \brief Constructs an object of MenuFolder
     */
    MenuFolder();

    /*!
     * \brief Deconstructs an object of MenuFolder
     */
    virtual ~MenuFolder();

    virtual int initialize();

    virtual void display();
    virtual void handleKeyboard(KeyboardButton key, bool pressed);
    virtual void handleMouseClick(unsigned int x, unsigned int y,
            KeyboardButton button, bool released);
    virtual void handleMouseScroll(int xrel, int yrel);

private:

    int init(std::string s);
    int init(Folder *folder, bool filter = true);
    virtual void loadOrOpen();

    long mCursor;
    long mMin;

    Folder *mapFolder;
    bool hiddenState;
};

#endif

