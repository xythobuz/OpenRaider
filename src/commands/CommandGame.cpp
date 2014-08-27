/*!
 * \file src/commands/CommandGame.cpp
 * \brief Game meta-commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "OpenRaider.h"
#include "World.h"
#include "commands/CommandGame.h"

std::string CommandPos::name() {
    return "pos";
}

std::string CommandPos::brief() {
    return "Print position info";
}

int CommandPos::execute(std::istream& args) {
    if ((!getOpenRaider().mRunning) || (!getGame().isLoaded())) {
        getConsole() << "Use pos command interactively!" << Console::endl;
        return -1;
    }

    getGame().getLara().print();
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
    if ((!getOpenRaider().mRunning) || (!getGame().isLoaded())) {
        getConsole() << "Use viewmodel command interactively!" << Console::endl;
        return -1;
    }

    //! \fixme Validate input

    std::string s;
    args >> s;
    unsigned int n = atoi(s.c_str());

    if (n < getWorld().sizeSkeletalModel()) {
        getGame().getLara().setSkeletalModel(n);
        return 0;
    } else {
        getConsole() << "Invalid SkeletalModel index!" << Console::endl;
        return -2;
    }
}

