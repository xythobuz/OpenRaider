/*!
 * \file src/commands/CommandEngine.cpp
 * \brief Engine commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "Log.h"
#include "Menu.h"
#include "RunTime.h"
#include "Render.h"
#include "commands/CommandEngine.h"

std::string CommandLoad::name() {
    return "load";
}

std::string CommandLoad::brief() {
    return "load a level file";
}

void CommandLoad::printHelp() {
    getLog() << "load-Command Usage:" << Log::endl;
    getLog() << "  load /path/to/level" << Log::endl;
}

int CommandLoad::execute(std::istream& args) {
    if (!getRunTime().isRunning()) {
        getLog() << "Use load command interactively!" << Log::endl;
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
    getLog() << "sshot-Command Usage:" << Log::endl;
    getLog() << "  sshot [console|menu] [console|menu]" << Log::endl;
    getLog() << "Add console/menu to capture them too" << Log::endl;
}

int CommandScreenshot::execute(std::istream& args) {
    if (!getRunTime().isRunning()) {
        getLog() << "Use sshot command interactively!" << Log::endl;
        return -1;
    }

    std::string filename(getRunTime().getBaseDir());
    filename += "/sshots/";
    filename += VERSION_SHORT;

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

    renderFrame();
    renderFrame(); // Double buffered
    getRender().screenShot(filename.c_str());

    getMenu().makeInvisible();
    getConsole().moveToTop();

    getLog() << "Screenshot stored..." << Log::endl;
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

