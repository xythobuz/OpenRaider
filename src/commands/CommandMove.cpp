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
    Log::get(LOG_USER) << "move-Command Usage:" << Log::endl;
    Log::get(LOG_USER) << "  move COMMAND" << Log::endl;
    Log::get(LOG_USER) << "Where COMMAND is one of the following:" << Log::endl;
    Log::get(LOG_USER) << "  walk" << Log::endl;
    Log::get(LOG_USER) << "  fly" << Log::endl;
    Log::get(LOG_USER) << "  noclip" << Log::endl;
}

int CommandMove::execute(std::istream& args) {
    if ((!RunTime::isRunning()) || (!Game::isLoaded())) {
        Log::get(LOG_USER) << "Use move command interactively!" << Log::endl;
        return -1;
    }

    std::string s;
    args >> s;
    if (s.compare("walk") == 0) {
        Game::getLara().setMoveType(Entity::MoveTypeWalk);
    } else if (s.compare("fly") == 0) {
        Game::getLara().setMoveType(Entity::MoveTypeFly);
    } else if (s.compare("noclip") == 0) {
        Game::getLara().setMoveType(Entity::MoveTypeNoClipping);
    } else {
        Log::get(LOG_USER) << "Invalid use of move command (" << s << ")!" << Log::endl;
        return -2;
    }

    Log::get(LOG_USER) << s  << "ing" << Log::endl;
    return 0;
}

