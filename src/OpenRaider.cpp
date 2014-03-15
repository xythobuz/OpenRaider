/*!
 * \file src/OpenRaider.cpp
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#include <cstdio>
#include <assert.h>

#include "WindowSDL.h"

#include "utils/strings.h"
#include "OpenRaider.h"

OpenRaider::OpenRaider() {
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

    mWindow = new WindowSDL();
    mWindow->initialize();

    return 0;
}

