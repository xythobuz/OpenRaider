/*!
 * \file include/Menu.h
 * \brief Main Menu
 *
 * \author xythobuz
 */

#ifndef _MENU_H_
#define _MENU_H_

#include <vector>

#include "Script.h"
#include "utils/Folder.h"

class Menu {
  public:
    static int initialize();
    static void shutdown();
    static void display();

    static bool isVisible() { return visible; }
    static void setVisible(bool v) { visible = v; }

    static const glm::vec4 textColor;
    static const glm::vec4 selectedColor;

  private:
    static bool visible;
    static Folder* mapFolder;
    static std::vector<Script> scripts;
    static std::vector<Folder> paths;
    static std::vector<int> images;
};

#endif

