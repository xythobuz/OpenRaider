/*!
 * \file src/commands/CommandSet.cpp
 * \brief Set command
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "Console.h"
#include "Font.h"
#include "OpenRaider.h"
#include "Sound.h"
#include "Window.h"
#include "utils/strings.h"
#include "commands/CommandSet.h"

std::string CommandSet::name() {
    return "set";
}

std::string CommandSet::brief() {
    return "set a parameter";
}

void CommandSet::printHelp() {
    getConsole() << "set-Command Usage:" << Console::endl;
    getConsole() << "  set VAR VAL" << Console::endl;
    getConsole() << "Available Variables:" << Console::endl;
    getConsole() << "  basedir    STRING" << Console::endl;
    getConsole() << "  pakdir     STRING" << Console::endl;
    getConsole() << "  audiodir   STRING" << Console::endl;
    getConsole() << "  datadir    STRING" << Console::endl;
    getConsole() << "  font       STRING" << Console::endl;
    getConsole() << "  size       INT INT" << Console::endl;
    getConsole() << "  fullscreen BOOL" << Console::endl;
    getConsole() << "  audio      BOOL" << Console::endl;
    getConsole() << "  volume     BOOL" << Console::endl;
    getConsole() << "  mouse_x    FLOAT" << Console::endl;
    getConsole() << "  mouse_y    FLOAT" << Console::endl;
    getConsole() << "  fps        BOOL" << Console::endl;
    getConsole() << "Enclose STRINGs with \"\"!" << Console::endl;
}

namespace {
    char *expandDirectoryNames(const char *s) {
        assert(s != NULL);
        assert(s[0] != '\0');

        char *result = bufferString("%s", s);

        if (getOpenRaider().mPakDir != NULL) {
            char *tmp = stringReplace(s, "$(pakdir)", getOpenRaider().mPakDir);
            delete [] result;
            result = tmp;
        }

        if (getOpenRaider().mAudioDir != NULL) {
            char *tmp = stringReplace(s, "$(audiodir)", getOpenRaider().mAudioDir);
            delete [] result;
            result = tmp;
        }

        if (getOpenRaider().mDataDir != NULL) {
            char *tmp = stringReplace(s, "$(datadir)", getOpenRaider().mDataDir);
            delete [] result;
            result = tmp;
        }

        if (getOpenRaider().mBaseDir != NULL) {
            char *tmp = stringReplace(result, "$(basedir)", getOpenRaider().mBaseDir);
            delete [] result;
            result = tmp;
        }

        return result;
    }
}

#define CHANGE_DIR_WITH_EXPANSION(a) do {     \
    std::string temp;                         \
    args >> temp;                             \
    const char *value = temp.c_str();         \
    char *quotes = stringRemoveQuotes(value); \
    char *tmp = expandDirectoryNames(quotes); \
    a = fullPath(tmp, 0);                     \
    delete [] tmp;                            \
    delete [] quotes;                         \
} while(false)

int CommandSet::execute(std::istream& args) {
    std::string var;
    args >> var;

    if (var.compare("size") == 0) {
        unsigned int w = DEFAULT_WIDTH, h = DEFAULT_HEIGHT;
        if (!(args >> w >> h)) {
            getConsole() << "set-size-Error: Invalid value(s)" << Console::endl;
            return -2;
        }
        getWindow().setSize(w, h);
    } else if (var.compare("fullscreen") == 0) {
        bool fullscreen = false;
        if (!(args >> fullscreen)) {
            getConsole() << "set-fullscreen-Error: Invalid value" << Console::endl;
            return -3;
        }
        getWindow().setFullscreen(fullscreen);
    } else if (var.compare("audio") == 0) {
        bool audio = false;
        if (!(args >> audio)) {
            getConsole() << "set-audio-Error: Invalid value" << Console::endl;
            return -4;
        }
        getSound().setEnabled(audio);
    } else if (var.compare("volume") == 0) {
        float vol = 1.0f;
        if (!(args >> vol)) {
            getConsole() << "set-volume-Error: Invalid value" << Console::endl;
            return -5;
        }
        getSound().setVolume(vol);
    } else if (var.compare("mouse_x") == 0) {
        float sense = 1.0f;
        if (!(args >> sense)) {
            getConsole() << "set-mouse_x-Error: Invalid value" << Console::endl;
            return -6;
        }
        getCamera().setSensitivityX(OR_DEG_TO_RAD(sense));
    } else if (var.compare("mouse_y") == 0) {
        float sense = 1.0f;
        if (!(args >> sense)) {
            getConsole() << "set-mouse_y-Error: Invalid value" << Console::endl;
            return -7;
        }
        getCamera().setSensitivityY(OR_DEG_TO_RAD(sense));
    } else if (var.compare("fps") == 0) {
        bool fps = false;
        if (!(args >> fps)) {
            getConsole() << "set-fps-Error: Invalid value" << Console::endl;
            return -8;
        }
        getOpenRaider().mFPS = fps;
    } else if (var.compare("basedir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(getOpenRaider().mBaseDir);
    } else if (var.compare("pakdir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(getOpenRaider().mPakDir);
    } else if (var.compare("audiodir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(getOpenRaider().mAudioDir);
    } else if (var.compare("datadir") == 0) {
        CHANGE_DIR_WITH_EXPANSION(getOpenRaider().mDataDir);
    } else if (var.compare("font") == 0) {
        std::string temp;
        args >> temp;
        const char *value = temp.c_str();
        char *quotes = stringReplace(value, "\"", "");
        char *tmp = expandDirectoryNames(quotes);
        getFont().setFont(tmp);
        delete [] tmp;
        delete [] quotes;
    } else {
        getConsole() << "set-Error: Unknown variable (" << var.c_str() << ")" << Console::endl;
        return -1;
    }

    return 0;
}

