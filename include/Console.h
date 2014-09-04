/*!
 * \file include/Console.h
 * \brief Console 'overlay'
 *
 * \author xythobuz
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <string>
#include <vector>

#include "UI.h"

/*!
 * \brief Console 'overlay'
 */
class Console : public UI {
public:

    Console();
    ~Console();

    virtual void moveToTop();
    virtual void makeInvisible();
    virtual void display();
    virtual void handleKeyboard(KeyboardButton key, bool pressed);
    virtual void handleText(char *text, bool notFinished);
    virtual void handleMouseScroll(int xrel, int yrel);

private:

    void moveInHistory(bool up);

    std::string mInputBuffer;
    std::string mPartialInput;

    size_t mHistoryPointer;
    std::vector<std::string> mCommandHistory;
    std::string mUnfinishedInput;

    unsigned int mLineOffset;
};

Console &getConsole();

#endif
