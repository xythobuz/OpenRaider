/*!
 * \file src/commands/CommandGame.cpp
 * \brief Game meta-commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "RunTime.h"
#include "World.h"
#include "commands/CommandGame.h"

std::string CommandPos::name() {
    return "pos";
}

std::string CommandPos::brief() {
    return "Print position info";
}

int CommandPos::execute(std::istream& args) {
    if ((!RunTime::isRunning()) || (!Game::isLoaded())) {
        Log::get(LOG_USER) << "Use pos command interactively!" << Log::endl;
        return -1;
    }

    Game::getLara().print();
    return 0;
}

// --------------------------------------

std::string CommandViewmodel::name() {
    return "viewmodel";
}

std::string CommandViewmodel::brief() {
    return "INT - Change Laras model";
}

int CommandViewmodel::execute(std::istream& args) {
    if ((!RunTime::isRunning()) || (!Game::isLoaded())) {
        Log::get(LOG_USER) << "Use viewmodel command interactively!" << Log::endl;
        return -1;
    }

    //! \fixme Validate input

    std::string s;
    args >> s;
    unsigned int n = atoi(s.c_str());

    if (n < getWorld().sizeSkeletalModel()) {
        Game::getLara().setSkeletalModel(n);
        return 0;
    } else {
        Log::get(LOG_USER) << "Invalid SkeletalModel index!" << Log::endl;
        return -2;
    }
}

