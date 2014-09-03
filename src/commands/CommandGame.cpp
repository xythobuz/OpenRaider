/*!
 * \file src/commands/CommandGame.cpp
 * \brief Game meta-commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Game.h"
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
    if ((!getRunTime().isRunning()) || (!getGame().isLoaded())) {
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
    if ((!getRunTime().isRunning()) || (!getGame().isLoaded())) {
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

// --------------------------------------

std::string CommandPigtail::name() {
    return "pigtail";
}

std::string CommandPigtail::brief() {
    return "BOOL";
}

int CommandPigtail::execute(std::istream& args) {
    if ((!getRunTime().isRunning()) || (!getGame().isLoaded())) {
        getConsole() << "Use pigtail command interactively!" << Console::endl;
        return -1;
    }

    bool b;
    args >> b;
    if (!args) {
        getConsole() << "Pass BOOL to pigtail command!" << Console::endl;
        return -2;
    }

    getGame().getLara().getModel().setPigTail(b);
    getConsole() << "Pigtail is now " << (b ? "on" : "off") << Console::endl;
    return 0;
}

// --------------------------------------

std::string CommandPonypos::name() {
    return "ponypos";
}

std::string CommandPonypos::brief() {
    return "FLOAT FLOAT FLOAT FLOAT - x y z angle";
}

int CommandPonypos::execute(std::istream& args) {
    if ((!getRunTime().isRunning()) || (!getGame().isLoaded())) {
        getConsole() << "Use ponypos command interactively!" << Console::endl;
        return -1;
    }

    float a, b, c, d;
    args >> a >> b >> c >> d;
    if (!args) {
        getConsole() << "Pass four FLOATs to ponypos command!" << Console::endl;
        return -2;
    }

    getGame().getLara().getModel().setPonyPos(a, b, c, d);
    return 0;
}

