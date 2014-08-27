/*!
 * \file src/commands/CommandBind.cpp
 * \brief Bind command
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "OpenRaider.h"
#include "commands/CommandBind.h"

std::string CommandBind::name() {
    return "bind";
}

std::string CommandBind::brief() {
    return "bind a keyboard/mouse action";
}

void CommandBind::printHelp() {
    getConsole() << "bind-Command Usage:" << Console::endl;
    getConsole() << "  bind ACTION KEY" << Console::endl;
    getConsole() << "Available Actions:" << Console::endl;
    getConsole() << "  menu" << Console::endl;
    getConsole() << "  console" << Console::endl;
    getConsole() << "  forward" << Console::endl;
    getConsole() << "  backward" << Console::endl;
    getConsole() << "  left" << Console::endl;
    getConsole() << "  right" << Console::endl;
    getConsole() << "  jump" << Console::endl;
    getConsole() << "  crouch" << Console::endl;
    getConsole() << "  use" << Console::endl;
    getConsole() << "  holster" << Console::endl;
    getConsole() << "  walk" << Console::endl;
    getConsole() << "Key-Format:" << Console::endl;
    getConsole() << "  'a' or '1'    for character/number keys" << Console::endl;
    getConsole() << "  \"leftctrl\"  for symbols and special keys" << Console::endl;
}

int CommandBind::execute(std::istream& args) {
    std::string a, b;
    if (!(args >> a >> b)) {
        getConsole() << "Invalid use of bind-command" << Console::endl;
        return -1;
    } else {
        ActionEvents e = stringToActionEvent(a.c_str());
        if (e == ActionEventCount) {
            getConsole() << "bind-Error: Unknown action (" << a << ")" << Console::endl;
            return -2;
        }

        KeyboardButton c = stringToKeyboardButton(b.c_str());
        if (c == unknownKey) {
            getConsole() << "bind-Error: Unknown key (" << b << ")" << Console::endl;
            return -3;
        }

        getOpenRaider().keyBindings[e] = c;
        return 0;
    }
}

