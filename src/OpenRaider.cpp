/*!
 * \file src/OpenRaider.cpp
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#include <cstdio>
#include <assert.h>

#include "utils/strings.h"
#include "OpenRaider.h"

OpenRaider::OpenRaider() {
}

OpenRaider::~OpenRaider() {
}

int OpenRaider::loadConfig(const char *config) {
    assert(config != NULL);

    char *configFile = fullPath(config, 0);
    printf("Trying to load \"%s\"...\n", configFile);

    return -1;
}

