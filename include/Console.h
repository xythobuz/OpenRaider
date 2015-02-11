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

struct ImGuiTextEditCallbackData;

class Console {
  public:
    static void display();

    static bool isVisible() { return visible; }
    static void setVisible(bool v) { visible = v; }

  private:
    static int callback(ImGuiTextEditCallbackData* data);

    const static int bufferLength = 256;

    static bool visible;
    static char buffer[bufferLength + 1];
    static unsigned long lastLogLength;
    static std::vector<std::string> lastCommands;
    static long lastCommandIndex;
    static std::string bufferedCommand;
};

#endif

