/*!
 * \file include/Console.h
 * \brief Console 'overlay'
 *
 * \author xythobuz
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <vector>
#include <cstddef>

/*!
 * \brief Console 'overlay'
 */
class Console {
public:

    /*!
     * \brief Constructs an object of Console
     */
    Console();

    /*!
     * \brief Deconstructs an object of Console
     */
    ~Console();

    void setVisible(bool visible);

    bool isVisible();

    void print(const char *s, ...) __attribute__((format(printf, 2, 3)));

    void display();

    void handleKeyboard(KeyboardButton key, bool pressed);

    void handleText(char *text, bool notFinished);

    void handleMouseScroll(int xrel, int yrel);

private:

    void moveInHistory(bool up);

    bool mVisible;
    char *mInputBuffer;
    size_t mInputBufferPointer;
    char *mPartialInput;
    std::vector<char *> mHistory;

    size_t mHistoryPointer;
    std::vector<char *> mCommandHistory;
    char *mUnfinishedInput;

    unsigned int mLineOffset;
};

Console &getConsole();

#endif
