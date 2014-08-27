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
    getConsole() << "  sshot [console|menu]" << Console::endl;
    getConsole() << "Add console/menu to capture them too" << Console::endl;
}

int CommandScreenshot::execute(std::istream& args) {
    if (!getOpenRaider().mRunning) {
        getConsole() << "Use sshot command interactively!" << Console::endl;
        return -1;
    }

    std::string filename(getOpenRaider().mBaseDir);
    filename += "/sshots/";
    filename += VERSION_SHORT;

    bool console = false, menu = false;
    std::string temp;
    args >> temp;
    if (temp.compare("console") == 0)
        console = true;
    if (temp.compare("menu") == 0)
        menu = true;

    if (!console) {
        getConsole().setVisible(false);
        if (menu)
            getMenu().setVisible(true);
        getOpenRaider().frame();
        getOpenRaider().frame(); // Double buffered
    }

    getRender().screenShot(filename.c_str());

    if (!console) {
        getConsole().setVisible(true);
        if (menu)
            getMenu().setVisible(false);
    }

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

