/*!
 * \file src/UI.cpp
 * \brief UI interface manager
 *
 * \author xythobuz
 */

#include <algorithm>

#include "global.h"
#include "Console.h"
#include "Menu.h"
#include "OpenRaider.h"
#include "Window.h"
#include "UI.h"

std::vector<UI*> UI::windows;

UI::~UI() {
}

void UI::display() { }
void UI::handleKeyboard(KeyboardButton key, bool pressed) { }
void UI::handleText(char *text, bool notFinished) { }
void UI::handleAction(ActionEvents action, bool isFinished) { }
void UI::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) { }
void UI::handleMouseMotion(int xrel, int yrel) { }
void UI::handleMouseScroll(int xrel, int yrel) { }

void UI::addWindow(UI* window) {
    windows.push_back(window);
}

void UI::removeWindow(UI *window) {
    findInList(window, [](unsigned long i){
        windows.erase(windows.begin() + i);
    });
}

bool UI::compareUIs(UI* a, UI* b) {
    return a->zPos < b->zPos;
}

bool UI::isOnTop(unsigned long windowID) {
    assert(windowID < windows.size());
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    unsigned long maxPos = (unsigned long)(maxIterator - windows.begin());
    return (maxPos == windowID);
}

void UI::moveToTop(unsigned long windowID) {
    assert(windowID < windows.size());

    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    long max = (*maxIterator)->zPos;
    unsigned long maxPos = (unsigned long)(maxIterator - windows.begin());

    if (maxPos != windowID) {
        windows.at(windowID)->zPos = max + 1;
    }
}

void UI::makeInvisible(unsigned long windowID) {
    assert(windowID < windows.size());
    windows.at(windowID)->zPos = -1;
}

void UI::findInList(UI *w, std::function<void (unsigned long i)> func) {
    for (unsigned long i = 0; i < windows.size(); i++) {
        auto UIptr = &(*windows.at(i));
        if (w == UIptr) {
            func(i);
            return;
        }
    }

    assert(false); // called UI was not registered
}

bool UI::isOnTop() {
    bool top = false;
    findInList(this, [&](unsigned long i) {
        top = UI::isOnTop(i);
    });
    return top;
}

void UI::moveToTop() {
    findInList(this, [](unsigned long i) {
        UI::moveToTop(i);
    });
}

void UI::makeInvisible() {
    findInList(this, [](unsigned long i) {
        UI::makeInvisible(i);
    });
}

void UI::passKeyboard(KeyboardButton key, bool pressed) {
    if (pressed) {
        if (getOpenRaider().keyBindings[menuAction] == key) {
            if (getMenu().isOnTop()) {
                getMenu().makeInvisible();
            } else {
                getMenu().moveToTop();
            }
        } else if (getOpenRaider().keyBindings[consoleAction] == key) {
            if (getConsole().isOnTop()) {
                getConsole().makeInvisible();
            } else {
                getConsole().moveToTop();
            }
        }
    }

    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleKeyboard(key, pressed);

    for (int i = forwardAction; i < ActionEventCount; i++) {
        if (getOpenRaider().keyBindings[i] == key) {
            (*maxIterator)->handleAction((ActionEvents)i, !pressed);
        }
    }

    bool mousegrab = (*maxIterator)->zPos == 0;
    if (mousegrab != getWindow().getMousegrab())
        getWindow().setMousegrab(mousegrab);
}

void UI::passText(char *text, bool notFinished) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleText(text, notFinished);
}

void UI::passMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleMouseClick(x, y, button, released);

    for (int i = forwardAction; i < ActionEventCount; i++) {
        if (getOpenRaider().keyBindings[i] == button) {
            (*maxIterator)->handleAction((ActionEvents)i, released);
        }
    }
}

void UI::passMouseMotion(int xrel, int yrel) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleMouseMotion(xrel, yrel);
}

void UI::passMouseScroll(int xrel, int yrel) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleMouseScroll(xrel, yrel);
}

void UI::displayInOrder() {
    std::sort(windows.begin(), windows.end(), compareUIs);
    for (auto &x : windows) {
        if (x->zPos >= 0) {
            x->display();
        }
    }
}

