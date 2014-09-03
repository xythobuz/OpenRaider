/*!
 * \file src/UI.cpp
 * \brief UI interface manager
 *
 * \author xythobuz
 */

#include <algorithm>

#include "global.h"
#include "Console.h"
#include "Debug.h"
#include "Menu.h"
#include "RunTime.h"
#include "TextureManager.h"
#include "Window.h"

std::vector<UI*> UI::windows;

UI::~UI() {
}

int UI::initialize() { return 0; }
void UI::eventsFinished() { }
void UI::display() { }
void UI::handleKeyboard(KeyboardButton key, bool pressed) { }
void UI::handleText(char *text, bool notFinished) { }
void UI::handleAction(ActionEvents action, bool isFinished) { }
void UI::handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released) { }
void UI::handleMouseMotion(int xrel, int yrel, int xabs, int yabs) { }
void UI::handleMouseScroll(int xrel, int yrel) { }

void UI::passKeyboard(KeyboardButton key, bool pressed) {
    if (pressed) {
        if (getRunTime().getKeyBinding(menuAction) == key) {
            if (getMenu().isOnTop()) {
                getMenu().makeInvisible();
            } else {
                getMenu().moveToTop();
            }
        } else if (getRunTime().getKeyBinding(consoleAction) == key) {
            if (getConsole().isOnTop()) {
                getConsole().makeInvisible();
            } else {
                getConsole().moveToTop();
            }
        } else if (getRunTime().getKeyBinding(debugAction) == key) {
            if (getDebug().isOnTop()) {
                getDebug().makeInvisible();
            } else {
                getDebug().moveToTop();
            }
        }
    }

    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleKeyboard(key, pressed);

    for (int i = forwardAction; i < ActionEventCount; i++) {
        if (getRunTime().getKeyBinding((ActionEvents)i) == key) {
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
        if (getRunTime().getKeyBinding((ActionEvents)i) == button) {
            (*maxIterator)->handleAction((ActionEvents)i, released);
        }
    }
}

void UI::passMouseMotion(int xrel, int yrel, int xabs, int yabs) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleMouseMotion(xrel, yrel, xabs, yabs);
}

void UI::passMouseScroll(int xrel, int yrel) {
    auto maxIterator = std::max_element(windows.begin(), windows.end(), compareUIs);
    (*maxIterator)->handleMouseScroll(xrel, yrel);
}

void UI::addWindow(UI* window) {
    windows.push_back(window);
}

void UI::removeWindow(UI *window) {
    if (windows.size() == 0) {
        // It seems as if our list was destroyed before the UIs
        return;
    }

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

int UI::passInitialize() {
    for (auto &x : windows) {
        int error = x->initialize();
        if (error != 0)
            return error;
    }

    return 0;
}

void UI::passEvents() {
    for (auto &x : windows) {
        x->eventsFinished();
    }
}

void UI::passDisplay() {
    std::sort(windows.begin(), windows.end(), compareUIs);
    for (auto &x : windows) {
        if (x->zPos >= 0) {
            x->display();
        }
    }
}

