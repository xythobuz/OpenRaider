/*!
 * \file src/RunTime.cpp
 * \brief run time configuration storage
 *
 * \author xythobuz
 */

#include "global.h"
#include "RunTime.h"

RunTime::RunTime() {
    gameIsRunning = false;

#ifdef DEBUG
    showFPS = true;
#else
    showFPS = false;
#endif

    for (int i = 0; i < ActionEventCount; i++)
        keyBindings[i] = unknownKey;

    // Default key bindings
    keyBindings[menuAction] = escapeKey;
    keyBindings[debugAction] = qKey;
    keyBindings[forwardAction] = wKey;
    keyBindings[backwardAction] = sKey;
    keyBindings[leftAction] = aKey;
    keyBindings[rightAction] = dKey;
}

std::string RunTime::getBaseDir() {
    return baseDir;
}

void RunTime::setBaseDir(std::string dir) {
    baseDir = dir;
}

std::string RunTime::getPakDir() {
    return pakDir;
}

void RunTime::setPakDir(std::string dir) {
    pakDir = dir;
}

std::string RunTime::getAudioDir() {
    return audioDir;
}

void RunTime::setAudioDir(std::string dir) {
    audioDir = dir;
}

std::string RunTime::getDataDir() {
    return dataDir;
}

void RunTime::setDataDir(std::string dir) {
    dataDir = dir;
}

KeyboardButton RunTime::getKeyBinding(ActionEvents event) {
    assert(event < ActionEventCount);
    return keyBindings[event];
}

void RunTime::setKeyBinding(ActionEvents event, KeyboardButton button) {
    assert(event < ActionEventCount);
    keyBindings[event] = button;
}

bool RunTime::isRunning() {
    return gameIsRunning;
}

void RunTime::setRunning(bool run) {
    gameIsRunning = run;
}

bool RunTime::getFPS() {
    return showFPS;
}

void RunTime::setFPS(bool fps) {
    showFPS = fps;
}

