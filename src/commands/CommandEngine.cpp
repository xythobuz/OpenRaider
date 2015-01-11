/*!
 * \file src/commands/CommandEngine.cpp
 * \brief Engine commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "Menu.h"
#include "RunTime.h"
#include "Render.h"
#include "UI.h"
#include "commands/CommandEngine.h"

std::string CommandLoad::name() {
    return "load";
}

std::string CommandLoad::brief() {
    return "load a level file";
}

void CommandLoad::printHelp() {
    Log::get(LOG_USER) << "load-Command Usage:" << Log::endl;
    Log::get(LOG_USER) << "  load /path/to/level" << Log::endl;
}

int CommandLoad::execute(std::istream& args) {
    if (!RunTime::isRunning()) {
        Log::get(LOG_USER) << "Use load command interactively!" << Log::endl;
        return -1;
    }

    std::string map;
    args >> map;
    int error = Game::loadLevel(map.c_str());
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
    Log::get(LOG_USER) << "sshot-Command Usage:" << Log::endl;
    Log::get(LOG_USER) << "  sshot" << Log::endl;
    Log::get(LOG_USER) << "You wont be able to capture imgui..." << Log::endl;
}

int CommandScreenshot::execute(std::istream& args) {
    if (!RunTime::isRunning()) {
        Log::get(LOG_USER) << "Use sshot command interactively!" << Log::endl;
        return -1;
    }

    std::string filename(RunTime::getBaseDir());
    filename += "/sshots/";
    filename += VERSION_SHORT;

    Render::screenShot(filename.c_str());
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
    RunTime::setRunning(false);
    return 0;
}

