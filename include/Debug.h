/*!
 * \file include/Debug.h
 * \brief Debug UI
 *
 * \author xythobuz
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "UI.h"

class Debug : public UI {
public:
    Debug();
    virtual ~Debug();

    virtual int initialize();
    virtual void eventsFinished();
    virtual void display();

    virtual void handleKeyboard(KeyboardButton key, bool pressed);
    virtual void handleText(char *text, bool notFinished);
    virtual void handleMouseClick(unsigned int x, unsigned int y, KeyboardButton button, bool released);
    virtual void handleMouseMotion(int xrel, int yrel, int xabs, int yabs);
    virtual void handleMouseScroll(int xrel, int yrel);

private:
    static void renderImGui(ImDrawList** const draw_lists, int count);

    static unsigned int fontTex;
};

Debug &getDebug();

#endif

