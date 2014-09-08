/*!
 * \file include/Console.h
 * \brief Console class
 *
 * \author xythobuz
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

class Console {
public:
    static void display();

private:
    const static int bufferLength = 256;
    static char buffer[bufferLength];
    static bool scrollToBottom;
    static unsigned long lastLogLength;
};

#endif

