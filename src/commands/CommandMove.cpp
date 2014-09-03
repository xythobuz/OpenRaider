/*!
 * \file src/commands/CommandMove.cpp
 * \brief Move command
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "RunTime.h"
#include "commands/CommandMove.h"

std::string CommandMove::name() {
    return "move";
}

std::string CommandMove::brief() {
    return "[walk|fly|noclip]";
}

void CommandMove::printHelp() {
    getConsole() << "move-Command Usage:" << Console::endl;
    getConsole() << "  move COMMAND" << Console::endl;
    getConsole() << "Where COMMAND is one of the following:" << Console::endl;
    getConsole() << "  walk" << Console::endl;
    getConsole() << "  fly" << Console::endl;
    getConsole() << "  noclip" << Console::endl;
}

int CommandMove::execute(std::istream& args) {
    if ((!getRunTime().isRunning()) || (!getGame().isLoaded())) {
        getConsole() << "Use move command interactively!" << Console::endl;
        return -1;
    }

    std::string s;
    args >> s;
    if (s.compare("walk") == 0) {
        getGame().getLara().setMoveType(Entity::MoveTypeWalk);
    } else if (s.compare("fly") == 0) {
        getGame().getLara().setMoveType(Entity::MoveTypeFly);
    } else if (s.compare("noclip") == 0) {
        getGame().getLara().setMoveType(Entity::MoveTypeNoClipping);
    } else {
        getConsole() << "Invalid use of move command (" << s << ")!" << Console::endl;
        return -2;
    }

    getConsole() << s  << "ing" << Console::endl;
    return 0;
}

