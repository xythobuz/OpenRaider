/*!
 * \file include/Selector.h
 * \brief Selector Window
 *
 * \author xythobuz
 */

#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include <array>

class Selector {
  public:
    static void displaySelection();
    static void display();

    static bool isVisible() { return visible; }
    static void setVisible(bool v) { visible = v; }

    static void handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released);

  private:
    static bool visible;
    static WorldObjects lastClickedObject;
    static std::array<bool, WorldObjectCount> clickOnObject;
    static glm::i32vec2 rayScreen;
    static glm::vec3 rayWorld, lastIntersectPos, lastIntersectNorm;
    static unsigned long lastIndexA, lastIndexB;
};

#endif

