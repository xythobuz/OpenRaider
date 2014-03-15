/*!
 * \file src/OpenRaider.cpp
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#include <cstdio>
#include <assert.h>

#include "WindowSDL.h"

#include "config.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "OpenRaider.h"

#define MAX_MS_PER_FRAME (1000 / MAXIMUM_FPS)

OpenRaider::OpenRaider() {
    mInit = false;
    mRunning = false;
    mWindow = NULL;
}

OpenRaider::~OpenRaider() {
    if (mWindow)
        delete mWindow;
}

int OpenRaider::loadConfig(const char *config) {
    assert(config != NULL);

    char *configFile = fullPath(config, 0);
    printf("Trying to load \"%s\"...\n", configFile);

    return 0;
}

int OpenRaider::initialize() {
    assert(mWindow == NULL);
    assert(mInit == false);
    assert(mRunning == false);

    mWindow = new WindowSDL();
    if (mWindow->initialize() != 0)
        return -1;

    mInit = true;

    return 0;
}

void OpenRaider::run() {
    assert(mInit == true);
    assert(mRunning == false);

    mRunning = true;
    while (mRunning) {
        clock_t startTime = systemTimerGet();

        mWindow->eventHandling();

        clock_t stopTime = systemTimerGet();
        if (MAX_MS_PER_FRAME > (stopTime - startTime))
            mWindow->delay(MAX_MS_PER_FRAME - (stopTime - startTime));
    }
}

