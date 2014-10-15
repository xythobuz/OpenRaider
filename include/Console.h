/*!
 * \file include/Console.h
 * \brief Console class
 *
 * \author xythobuz
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <string>
#include <vector>

class Console {
public:
    static void display();

private:
    const static int bufferLength = 256;
    static char buffer[bufferLength + 1];
    static bool scrollToBottom;
    static bool focusInput;
    static unsigned long lastLogLength;
    static std::vector<std::string> lastCommands;
    static long lastCommandIndex;
};

#endif

