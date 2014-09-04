/*!
 * \file src/commands/CommandSound.cpp
 * \brief Sound command
 *
 * \author xythobuz
 */

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "RunTime.h"
#include "Sound.h"
#include "commands/CommandSound.h"

std::string CommandSound::name() {
    return "sound";
}

std::string CommandSound::brief() {
    return "INT - Test play sound";
}

void CommandSound::printHelp() {
    getLog() << "sound-Command Usage:" << Log::endl;
    getLog() << "sound-Command Usage:" << Log::endl;
    getLog() << "  sound INT" << Log::endl;
    getLog() << "Where INT is a valid sound ID integer" << Log::endl;
}

int CommandSound::execute(std::istream& args) {
    if (!getGame().isLoaded()) {
        getLog() << "Use sound command interactively!" << Log::endl;
        return -1;
    }

    std::string s;
    args >> s;

    getSound().play(atoi(s.c_str()));
    return 0;
}

