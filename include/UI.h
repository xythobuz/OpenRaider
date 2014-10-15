/*!
 * \file include/UI.h
 * \brief Abstract UI interface
 *
 * \author xythobuz
 */

#ifndef _UI_H_
#define _UI_H_

#include <functional>
#include <list>
#include <memory>
#include <tuple>

#include "imgui/imgui.h"

class UI {
public:
    static int initialize();
    static void eventsFinished();
    static void display();
    static void shutdown();

    static void setVisible(bool v);
    static bool isVisible();

    static void handleKeyboard(KeyboardButton key, bool pressed);
    static void handleText(char *text, bool notFinished);
    static void handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released);
    static void handleMouseMotion(int xrel, int yrel, int xabs, int yabs);
    static void handleMouseScroll(int xrel, int yrel);

    static void renderImGui(ImDrawList** const draw_lists, int count);

private:
    static bool visible;
    static unsigned int fontTex;
    static std::string iniFilename;
    static std::string logFilename;
    static bool metaKeyIsActive;

    static std::list<std::tuple<KeyboardButton, bool>> keyboardEvents;
    static std::list<std::tuple<unsigned int, unsigned int, KeyboardButton, bool>> clickEvents;
    static std::list<std::tuple<int, int, int, int>> motionEvents;
    static std::list<std::tuple<int, int>> scrollEvents;
};

#endif

