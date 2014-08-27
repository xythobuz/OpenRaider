/*!
 * \file src/commands/CommandSound.cpp
 * \brief Sound command
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "OpenRaider.h"
#include "Sound.h"
#include "commands/CommandSound.h"

std::string CommandSound::name() {
    return "sound";
}

std::string CommandSound::brief() {
    return "INT - Test play sound";
}

void CommandSound::printHelp() {
    getConsole() << "sound-Command Usage:" << Console::endl;
    getConsole() << "sound-Command Usage:" << Console::endl;
    getConsole() << "  sound INT" << Console::endl;
    getConsole() << "Where INT is a valid sound ID integer" << Console::endl;
}

int CommandSound::execute(std::istream& args) {
    if ((!getOpenRaider().mRunning) || (!getGame().isLoaded())) {
        getConsole() << "Use sound command interactively!" << Console::endl;
        return -1;
    }

    std::string s;
    args >> s;

    getSound().play(atoi(s.c_str()));
    return 0;
}

