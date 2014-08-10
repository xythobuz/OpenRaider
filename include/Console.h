/*!
 * \file include/Console.h
 * \brief Console 'overlay'
 *
 * \author xythobuz
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <string>
#include <sstream>
#include <vector>

/*!
 * \brief Console 'overlay'
 */
class Console {
public:

    /*!
     * \brief Constructs an object of Console
     */
    Console();

    void setVisible(bool visible);

    bool isVisible();

    template<typename T>
    Console &operator<<(const T t) {
        printBuffer << t;
        if (printBuffer.str().back() == '\n') {
            mHistory.push_back(printBuffer.str().substr(0, printBuffer.str().length() - 1));
#ifdef DEBUG
            std::cout << printBuffer.str().substr(0, printBuffer.str().length() - 1) << std::endl;
#endif
            printBuffer.str("");
        }
        return (*this);
    }

    void display();

    void handleKeyboard(KeyboardButton key, bool pressed);

    void handleText(char *text, bool notFinished);

    void handleMouseScroll(int xrel, int yrel);

    const static char endl = '\n';

private:

    void moveInHistory(bool up);

    bool mVisible;
    std::string mInputBuffer;
    std::string mPartialInput;
    std::vector<std::string> mHistory;

    size_t mHistoryPointer;
    std::vector<std::string> mCommandHistory;
    std::string mUnfinishedInput;

    unsigned int mLineOffset;

    std::ostringstream printBuffer;
};

Console &getConsole();

#endif
