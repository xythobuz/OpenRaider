/*!
 * \file src/commands/Command.cpp
 * \brief Commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "commands/Command.h"

Command::~Command() {

}

void Command::printHelp() {
    getConsole() << "No help available!" << Console::endl;
}

