/*!
 * \file src/main.cpp
 * \brief Where main() is
 *
 * \author xythobuz
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "config.h"
#include "OpenRaider.h"
#include "main.h"

OpenRaider *gOpenRaider = NULL;

void cleanupHandler() {
    if (gOpenRaider)
        delete gOpenRaider;
}

int main(int argc, char *argv[]) {
    const char *config = NULL;

    // Handle arguments
    if (argc == 1) {
        // Use default rc file path
        config = DEFAULT_CONFIG_PATH DEFAULT_CONFIG_FILE;
    } else if (argc == 2) {
        // Check for command line switches
        if ((strcmp("-h", argv[1]) == 0)
                || (strcmp("--help", argv[1]) == 0)) {
            // Display help text
            printf("%s [OPTIONS | /path/to/config]\n"
                    "Options:\n"
                    "\t--help\n\t-h\tDisplay this help text\n"
                    "\t--version\n\t-v\tDisplay version information\n"
                    "If no options are given, the default config will be loaded from:\n"
                    "\t" DEFAULT_CONFIG_PATH DEFAULT_CONFIG_FILE "\n", argv[0]);
            return 0;
        } else if ((strcmp("-v", argv[1]) == 0)
                || (strcmp("--version", argv[1]) == 0)) {
            // Display version
            printf(VERSION "\n");
            return 0;
        } else {
            // Interpret as rc file name
            config = argv[1];
        }
    } else {
        printf("Usage:\n%s -h\n", argv[0]);
        return 1;
    }

    // Create globals
    atexit(cleanupHandler);
    printf("Initializing " VERSION "\n");
    gOpenRaider = new OpenRaider();

    // Try to load a configuration
    if (gOpenRaider->loadConfig(config) != 0) {
        if (gOpenRaider->loadConfig(DEFAULT_CONFIG_PATH DEFAULT_CONFIG_FILE) != 0) {
            if (gOpenRaider->loadConfig(DEFAULT_CONFIG_FILE) != 0) {
                printf("Could not find a config file. Aborting...\n");
                return 2;
            }
        }
    }

    // Initialize the "subsystems"

    return 0;
}

