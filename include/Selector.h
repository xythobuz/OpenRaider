/*!
 * \file include/Selector.h
 * \brief Selector Window
 *
 * \author xythobuz
 */

#ifndef _SELECTOR_H_
#define _SELECTOR_H_

class Selector {
  public:
    static void display();

    static bool isVisible() { return visible; }
    static void setVisible(bool v) { visible = v; }

    static void handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released);

  private:
    static bool visible;
};

#endif

