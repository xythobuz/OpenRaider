/*!
 * \file src/commands/CommandMove.cpp
 * \brief Move command
 *
 * \author xythobuz
 */

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "RunTime.h"
#include "commands/CommandMove.h"

std::string CommandMove::name() {
    return "move";
}

std::string CommandMove::brief() {
    return "[walk|fly|noclip]";
}

void CommandMove::printHelp() {
    getLog() << "move-Command Usage:" << Log::endl;
    getLog() << "  move COMMAND" << Log::endl;
    getLog() << "Where COMMAND is one of the following:" << Log::endl;
    getLog() << "  walk" << Log::endl;
    getLog() << "  fly" << Log::endl;
    getLog() << "  noclip" << Log::endl;
}

int CommandMove::execute(std::istream& args) {
    if ((!getRunTime().isRunning()) || (!getGame().isLoaded())) {
        getLog() << "Use move command interactively!" << Log::endl;
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
        getLog() << "Invalid use of move command (" << s << ")!" << Log::endl;
        return -2;
    }

    getLog() << s  << "ing" << Log::endl;
    return 0;
}

