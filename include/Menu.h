/*!
 * \file include/Menu.h
 * \brief Menu 'overlay' interface
 *
 * \author xythobuz
 */

#ifndef _MENU_H_
#define _MENU_H_

#include <functional>

class Menu {
  public:

    virtual ~Menu() { }

    virtual int initialize() = 0;

    virtual void display() = 0;

    virtual void handleKeyboard(KeyboardButton key, bool pressed) = 0;

    virtual void handleMouseClick(unsigned int x, unsigned int y,
                                  KeyboardButton button, bool released) = 0;

    virtual void handleMouseScroll(int xrel, int yrel) = 0;

    bool isVisible() { return visible; }

    void setVisible(bool v) { visible = v; }

  protected:

    virtual void showDialog(std::string msg, std::string btn1, std::string btn2,
                            std::function<int (bool state)> callback = std::function<int (bool)>());

    virtual void ackDialog();

    virtual bool handleKeyboardDialog(KeyboardButton key, bool pressed);

    virtual bool handleMouseClickDialog(unsigned int x, unsigned int y,
                                        KeyboardButton button, bool released);

    virtual bool handleMouseScrollDialog(int xrel, int yrel);

    virtual void displayDialog();

    bool dialogState;
    std::string dialogText;
    std::string dialogButton1;
    std::string dialogButton2;
    std::function<int (bool state)> dialogFunction;

    bool visible;
};

Menu& getMenu();

#endif

