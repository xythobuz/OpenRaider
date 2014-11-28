/*!
 * \file src/RunTime.cpp
 * \brief run time configuration storage
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "RunTime.h"

RunTime::RunTime()
    : baseDir(expandHomeDirectory("~/.OpenRaider")) {
    pakDir = baseDir + "/paks";
    audioDir = baseDir + "/music";
    dataDir = baseDir + "/data";

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

    lastTime = 0;
    frameCount = 0;
    frameCount2 = 0;
    frameTimeSum = 0;
    frameTimeSum2 = 0;
    fps = 0;
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

bool RunTime::getShowFPS() {
    return showFPS;
}

void RunTime::setShowFPS(bool fps) {
    showFPS = fps;
}

void RunTime::updateFPS() {
    frameCount++;
    frameTimeSum += (systemTimerGet() - lastTime);
    frameTimeSum2 += (systemTimerGet() - lastTime);
    lastTime = systemTimerGet();
    if (frameTimeSum >= 200) {
        fps = frameCount * (1000 / frameTimeSum);
        frameCount = frameTimeSum = 0;
    }

    if (frameTimeSum2 >= 1000) {
        history.push_back(frameCount2);
        frameCount2 = frameTimeSum2 = 0;
    }
    frameCount2++;
}

unsigned long RunTime::getFPS() {
    return fps;
}

const std::vector<float>& RunTime::getHistoryFPS() {
    return history;
}

