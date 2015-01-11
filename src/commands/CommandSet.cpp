/*!
 * \file src/commands/CommandSet.cpp
 * \brief Set command
 *
 * \author xythobuz
 */

#include "global.h"
#include "Camera.h"
#include "Log.h"
#include "RunTime.h"
#include "system/Font.h"
#include "system/Sound.h"
#include "system/Window.h"
#include "utils/strings.h"
#include "commands/CommandSet.h"

std::string CommandSet::name() {
    return "set";
}

std::string CommandSet::brief() {
    return "set a parameter";
}

void CommandSet::printHelp() {
    Log::get(LOG_USER) << "set-Command Usage:" << Log::endl;
    Log::get(LOG_USER) << "  set VAR VAL" << Log::endl;
    Log::get(LOG_USER) << "Available Variables:" << Log::endl;
    Log::get(LOG_USER) << "  basedir    STRING" << Log::endl;
    Log::get(LOG_USER) << "  pakdir     STRING" << Log::endl;
    Log::get(LOG_USER) << "  audiodir   STRING" << Log::endl;
    Log::get(LOG_USER) << "  datadir    STRING" << Log::endl;
    Log::get(LOG_USER) << "  font       STRING" << Log::endl;
    Log::get(LOG_USER) << "  size       INT INT" << Log::endl;
    Log::get(LOG_USER) << "  fullscreen BOOL" << Log::endl;
    Log::get(LOG_USER) << "  audio      BOOL" << Log::endl;
    Log::get(LOG_USER) << "  volume     BOOL" << Log::endl;
    Log::get(LOG_USER) << "  mouse_x    FLOAT" << Log::endl;
    Log::get(LOG_USER) << "  mouse_y    FLOAT" << Log::endl;
    Log::get(LOG_USER) << "  fps        BOOL" << Log::endl;
    Log::get(LOG_USER) << "Enclose STRINGs with \"\"!" << Log::endl;
}

static std::string expandNames(std::string s) {
    // Remove quotes
    if ((s.length() >= 3) &&
        (((s[0] == '"') && (s[s.length() - 1] == '"'))
         || ((s[0] == '\'') && (s[s.length() - 1] == '\'')))) {
        s.erase(0, 1);
        s.erase(s.length() - 1, 1);
    }

    // Expand Names
    s = findAndReplace(s, "$(pakdir)", RunTime::getPakDir());
    s = findAndReplace(s, "$(audiodir)", RunTime::getAudioDir());
    s = findAndReplace(s, "$(datadir)", RunTime::getDataDir());
    s = findAndReplace(s, "$(basedir)", RunTime::getBaseDir());

    // Full path
    s = expandHomeDirectory(s);

    return s;
}

int CommandSet::execute(std::istream& args) {
    std::string var;
    args >> var;

    if (var.compare("size") == 0) {
        unsigned int w = DEFAULT_WIDTH, h = DEFAULT_HEIGHT;
        if (!(args >> w >> h)) {
            Log::get(LOG_USER) << "set-size-Error: Invalid value(s)" << Log::endl;
            return -2;
        }
        Window::setSize(glm::i32vec2(w, h));
    } else if (var.compare("fullscreen") == 0) {
        bool fullscreen = false;
        if (!(args >> fullscreen)) {
            Log::get(LOG_USER) << "set-fullscreen-Error: Invalid value" << Log::endl;
            return -3;
        }
        Window::setFullscreen(fullscreen);
    } else if (var.compare("audio") == 0) {
        bool audio = false;
        if (!(args >> audio)) {
            Log::get(LOG_USER) << "set-audio-Error: Invalid value" << Log::endl;
            return -4;
        }
        Sound::setEnabled(audio);
    } else if (var.compare("volume") == 0) {
        float vol = 1.0f;
        if (!(args >> vol)) {
            Log::get(LOG_USER) << "set-volume-Error: Invalid value" << Log::endl;
            return -5;
        }
        Sound::setVolume(vol);
    } else if (var.compare("mouse_x") == 0) {
        float sense = 1.0f;
        if (!(args >> sense)) {
            Log::get(LOG_USER) << "set-mouse_x-Error: Invalid value" << Log::endl;
            return -6;
        }
        Camera::setSensitivityX(glm::radians(sense));
    } else if (var.compare("mouse_y") == 0) {
        float sense = 1.0f;
        if (!(args >> sense)) {
            Log::get(LOG_USER) << "set-mouse_y-Error: Invalid value" << Log::endl;
            return -7;
        }
        Camera::setSensitivityY(glm::radians(sense));
    } else if (var.compare("fps") == 0) {
        bool fps = false;
        if (!(args >> fps)) {
            Log::get(LOG_USER) << "set-fps-Error: Invalid value" << Log::endl;
            return -8;
        }
        RunTime::setShowFPS(fps);
    } else if (var.compare("basedir") == 0) {
        std::string temp;
        args >> temp;
        RunTime::setBaseDir(expandNames(temp));
    } else if (var.compare("pakdir") == 0) {
        std::string temp;
        args >> temp;
        RunTime::setPakDir(expandNames(temp));
    } else if (var.compare("audiodir") == 0) {
        std::string temp;
        args >> temp;
        RunTime::setAudioDir(expandNames(temp));
    } else if (var.compare("datadir") == 0) {
        std::string temp;
        args >> temp;
        RunTime::setDataDir(expandNames(temp));
    } else if (var.compare("font") == 0) {
        std::string temp;
        args >> temp;
        int error = Font::initialize(expandNames(temp));
        if (error != 0)
            Log::get(LOG_USER) << "Error initializing font: " << expandNames(temp) << "(" << error << ")" << Log::endl;
    } else {
        Log::get(LOG_USER) << "set-Error: Unknown variable (" << var.c_str() << ")" << Log::endl;
        return -1;
    }

    return 0;
}

std::string CommandGet::name() {
    return "get";
}

std::string CommandGet::brief() {
    return "get a parameter";
}

void CommandGet::printHelp() {
    Log::get(LOG_USER) << "get-Command Usage:" << Log::endl;
    Log::get(LOG_USER) << "  get VAR" << Log::endl;
    Log::get(LOG_USER) << "Available Variables:" << Log::endl;
    Log::get(LOG_USER) << "  basedir" << Log::endl;
    Log::get(LOG_USER) << "  pakdir" << Log::endl;
    Log::get(LOG_USER) << "  audiodir" << Log::endl;
    Log::get(LOG_USER) << "  datadir" << Log::endl;
    Log::get(LOG_USER) << "  font" << Log::endl;
    Log::get(LOG_USER) << "  size" << Log::endl;
    Log::get(LOG_USER) << "  fullscreen" << Log::endl;
    Log::get(LOG_USER) << "  audio" << Log::endl;
    Log::get(LOG_USER) << "  volume" << Log::endl;
    Log::get(LOG_USER) << "  mouse_x" << Log::endl;
    Log::get(LOG_USER) << "  mouse_y" << Log::endl;
    Log::get(LOG_USER) << "  fps" << Log::endl;
}

int CommandGet::execute(std::istream& args) {
    std::string var;
    args >> var;

    if (var.compare("size") == 0) {
        Log::get(LOG_USER) << Window::getSize() << Log::endl;
    } else if (var.compare("fullscreen") == 0) {
        Log::get(LOG_USER) << Window::getFullscreen() << Log::endl;
    } else if (var.compare("audio") == 0) {
        Log::get(LOG_USER) << Sound::getEnabled() << Log::endl;
    } else if (var.compare("volume") == 0) {
        Log::get(LOG_USER) << Sound::getVolume() << Log::endl;
    } else if (var.compare("mouse_x") == 0) {
        Log::get(LOG_USER) << glm::degrees(Camera::getSensitivityX()) << Log::endl;
    } else if (var.compare("mouse_y") == 0) {
        Log::get(LOG_USER) << glm::degrees(Camera::getSensitivityY()) << Log::endl;
    } else if (var.compare("fps") == 0) {
        Log::get(LOG_USER) << RunTime::getShowFPS() << Log::endl;
    } else if (var.compare("basedir") == 0) {
        Log::get(LOG_USER) << RunTime::getBaseDir() << Log::endl;
    } else if (var.compare("pakdir") == 0) {
        Log::get(LOG_USER) << RunTime::getPakDir() << Log::endl;
    } else if (var.compare("audiodir") == 0) {
        Log::get(LOG_USER) << RunTime::getAudioDir() << Log::endl;
    } else if (var.compare("datadir") == 0) {
        Log::get(LOG_USER) << RunTime::getDataDir() << Log::endl;
    } else if (var.compare("font") == 0) {
        Log::get(LOG_USER) << Font::getFontName() << Log::endl;
    } else {
        Log::get(LOG_USER) << "get-Error: Unknown variable (" << var << ")" << Log::endl;
        return -1;
    }

    return 0;
}

