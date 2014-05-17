/*!
 * \file src/main.cpp
 * \brief Where main() is
 *
 * \author xythobuz
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "global.h"
#include "Camera.h"
#include "Console.h"
#include "Game.h"
#include "Menu.h"
#include "OpenRaider.h"
#include "Render.h"
#include "World.h"
#include "utils/time.h"

#ifdef USING_AL
#include "SoundAL.h"
#else
#include "SoundNull.h"
#endif

#ifdef USING_SDL
#include "WindowSDL.h"
#else
#error No Windowing Library selected!
#endif

Camera gCamera;
Console gConsole;
Game gGame;
Menu gMenu;
OpenRaider gOpenRaider;
Render gRender;
World gWorld;

#ifdef USING_AL
SoundAL gSound;
#else
SoundNull gSound;
#endif

#ifdef USING_SDL
WindowSDL gWindow;
#endif

Camera &getCamera() {
    return gCamera;
}

Console &getConsole() {
    return gConsole;
}

Game &getGame() {
    return gGame;
}

Menu &getMenu() {
    return gMenu;
}

OpenRaider &getOpenRaider() {
    return gOpenRaider;
}

Render &getRender() {
    return gRender;
}

Sound &getSound() {
    return gSound;
}

Window &getWindow() {
    return gWindow;
}

World &getWorld() {
    return gWorld;
}

void cleanupHandler(void) {
#ifdef DEBUG
    printf("\nThanks for testing %s\n", VERSION);
    printf("Build date: %s @ %s\n", __DATE__, __TIME__);
    printf("Build host: %s\n", BUILD_HOST);
    printf("Web site  : http://github.com/xythobuz/OpenRaider\n");
    printf("Contact   : xythobuz@xythobuz.de\n");
#endif
}

int main(int argc, char *argv[]) {
    const char *config = NULL;

    // Handle arguments
    if (argc == 1) {
        // Use default rc file path
        config = DEFAULT_CONFIG_PATH "/" DEFAULT_CONFIG_FILE;
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
                    "\t" DEFAULT_CONFIG_PATH "/" DEFAULT_CONFIG_FILE "\n", argv[0]);
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

#ifdef DEBUG
    printf("Initializing %s\n", VERSION);
#endif

    atexit(cleanupHandler);

    // Try to load a configuration
    if (gOpenRaider.loadConfig(config) != 0) {
        if (gOpenRaider.loadConfig(DEFAULT_CONFIG_PATH "/" DEFAULT_CONFIG_FILE) != 0) {
            if (gOpenRaider.loadConfig(DEFAULT_CONFIG_FILE) != 0) {
                printf("Could not find a config file. Aborting...\n");
                return 2;
            }
        }
    }

    // Initialize everything
    int error = gOpenRaider.initialize();
    if (error != 0) {
        printf("Could not initialize OpenRaider (%d)!\n", error);
        return 3;
    }

    // Enter Main loop
    gConsole.print("Starting %s", VERSION);
    gOpenRaider.run();

    return 0;
}

#if defined(HAVE_EXECINFO_H) && defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS) && (!defined(NDEBUG))

#include <execinfo.h>

void assertImplementation(const char *exp, const char *file, int line) {
    const unsigned int maxSize = 128;
    void *callstack[maxSize];
    int frames = backtrace(callstack, maxSize);
    char **strs = backtrace_symbols(callstack, frames);

    printf("\nassertion failed:\n");
    printf("\t%s\n", exp);
    printf("in %s:%d\n\n", file, line);

    for (int i = 0; i < frames; i++)
        printf("%s\n", strs[i]);

    delete [] strs;
    abort();
}

#endif

