/*!
 * \file src/main.cpp
 * \brief Where main() is
 *
 * \author xythobuz
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <assert.h>

#include "config.h"
#include "main.h"
#include "utils/time.h"
#include "WindowSDL.h"

Console *gConsole = NULL;
Game *gGame = NULL;
Menu *gMenu = NULL;
OpenRaider *gOpenRaider = NULL;
Sound *gSound = NULL;
Window *gWindow = NULL;
World *gWorld = NULL;

Console &getConsole() {
    assert(gConsole != NULL);
    return *gConsole;
}

Game &getGame() {
    assert(gGame != NULL);
    return *gGame;
}

Menu &getMenu() {
    assert(gMenu != NULL);
    return *gMenu;
}

OpenRaider &getOpenRaider() {
    assert(gOpenRaider != NULL);
    return *gOpenRaider;
}

Sound &getSound() {
    assert(gSound != NULL);
    return *gSound;
}

Window &getWindow() {
    assert(gWindow != NULL);
    return *gWindow;
}

World &getWorld() {
    assert(gWorld != NULL);
    return *gWorld;
}

void cleanupHandler(void) {
    if (gConsole)
        delete gConsole;

    if (gGame)
        delete gGame;

    if (gMenu)
        delete gMenu;

    if (gWorld)
        delete gWorld;

    if (gOpenRaider)
        delete gOpenRaider;

    if (gSound)
        delete gSound;

    if (gWindow)
        delete gWindow;

    printf("\nThanks for testing %s\n", VERSION);
    printf("Build date: %s @ %s\n", __DATE__, __TIME__);
    printf("Build host: %s\n", BUILD_HOST);
    printf("Web site  : http://github.com/xythobuz/OpenRaider\n");
    printf("Contact   : xythobuz@xythobuz.de\n");
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
    printf("Initializing %s\n", VERSION);
    atexit(cleanupHandler);
    gOpenRaider = new OpenRaider();
    gWindow = new WindowSDL();
    gSound = new Sound();
    gWorld = new World();
    gConsole = new Console();
    gMenu = new Menu();
    gGame = new Game();

    // Try to load a configuration
    if (gOpenRaider->loadConfig(config) != 0) {
        if (gOpenRaider->loadConfig(DEFAULT_CONFIG_PATH DEFAULT_CONFIG_FILE) != 0) {
            if (gOpenRaider->loadConfig(DEFAULT_CONFIG_FILE) != 0) {
                printf("Could not find a config file. Aborting...\n");
                return 2;
            }
        }
    }

    // Initialize Windowing
    int error = gWindow->initialize();
    if (error != 0) {
        printf("Could not initialize Window (%d)!\n", error);
        return 3;
    }

    // Initialize OpenGL
    error = gWindow->initializeGL();
    if (error != 0) {
        printf("Could not initialize OpenGL (%d)!\n", error);
        return 4;
    }

    error = gWindow->initializeFont();
    if (error != 0) {
        printf("Could not initialize SDL-TTF (%d)!\n", error);
        return 5;
    }

    error = gSound->initialize();
    if (error != 0) {
        printf("Could not initialize Sound (%d)!\n", error);
        return 7;
    }

    // Initialize game engine
    error = gGame->initialize();
    if (error != 0) {
        printf("Could not initialize Game Engine (%d)!\n", error);
        return 8;
    }

    gMenu->setVisible(true);

    systemTimerReset();

    // Enter Main loop
    gConsole->print("Starting %s", VERSION);
    gOpenRaider->run();

    return 0;
}

