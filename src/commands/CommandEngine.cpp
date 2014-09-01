/*!
 * \file src/commands/CommandEngine.cpp
 * \brief Engine commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "Menu.h"
#include "OpenRaider.h"
#include "Render.h"
#include "commands/CommandEngine.h"

std::string CommandLoad::name() {
    return "load";
}

std::string CommandLoad::brief() {
    return "load a level file";
}

void CommandLoad::printHelp() {
    getConsole() << "load-Command Usage:" << Console::endl;
    getConsole() << "  load /path/to/level" << Console::endl;
}

int CommandLoad::execute(std::istream& args) {
    if (!getOpenRaider().mRunning) {
        getConsole() << "Use load command interactively!" << Console::endl;
        return -1;
    }

    std::string map;
    args >> map;
    int error = getGame().loadLevel(map.c_str());
    return error;
}

// --------------------------------------

std::string CommandScreenshot::name() {
    return "sshot";
}

std::string CommandScreenshot::brief() {
    return "make a screenshot";
}

void CommandScreenshot::printHelp() {
    getConsole() << "sshot-Command Usage:" << Console::endl;
    getConsole() << "  sshot [console|menu] [console|menu]" << Console::endl;
    getConsole() << "Add console/menu to capture them too" << Console::endl;
}

int CommandScreenshot::execute(std::istream& args) {
    if (!getOpenRaider().mRunning) {
        getConsole() << "Use sshot command interactively!" << Console::endl;
        return -1;
    }

    std::string filename(getOpenRaider().getBaseDir());
    filename += "/sshots/";
    filename += VERSION_SHORT;

    bool console = false, menu = false;
    std::string temp, temp2;
    args >> temp >> temp2;

    getConsole().makeInvisible();
    getMenu().makeInvisible();

    if (temp == "console")
        getConsole().moveToTop();
    else if (temp == "menu")
        getMenu().moveToTop();

    if (temp2 == "console")
        getConsole().moveToTop();
    else if (temp2 == "menu")
        getMenu().moveToTop();

    getOpenRaider().frame();
    getOpenRaider().frame(); // Double buffered
    getRender().screenShot(filename.c_str());

    getMenu().makeInvisible();
    getConsole().moveToTop();

    getConsole() << "Screenshot stored..." << Console::endl;
    return 0;
}

// --------------------------------------

std::string CommandQuit::name() {
    return "quit";
}

std::string CommandQuit::brief() {
    return "exit OpenRaider";
}

int CommandQuit::execute(std::istream& args) {
    exit(0);
    return 0;
}

