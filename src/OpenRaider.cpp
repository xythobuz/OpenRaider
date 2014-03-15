/*!
 * \file src/OpenRaider.cpp
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#include <cstdio>
#include <cstring>
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
    mBaseDir = NULL;
    mPakDir = NULL;
    mAudioDir = NULL;
    mDataDir = NULL;

    mSound = new Sound();
    mWindow = new WindowSDL();
}

OpenRaider::~OpenRaider() {
    if (mSound)
        delete mSound;

    if (mWindow)
        delete mWindow;

    if (mBaseDir)
        delete mBaseDir;

    if (mPakDir)
        delete mPakDir;

    if (mAudioDir)
        delete mAudioDir;

    if (mDataDir)
        delete mDataDir;
}

int OpenRaider::loadConfig(const char *config) {
    assert(config != NULL);
    assert(config[0] != '\0');

    char *configFile = fullPath(config, 0);
    printf("Loading config from \"%s\"...\n", configFile);

    FILE *f = fopen(configFile, "r");
    if (f == NULL) {
        printf("Could not open file!\n");
        return -1;
    }

    char buffer[256];
    while (fgets(buffer, 256, f) != NULL) {
        command(buffer);
    }

    fclose(f);

    return 0;
}

int OpenRaider::command(const char *command) {
    assert(command != NULL);
    assert(command[0] != '\0');

    int returnValue = 0;
    char *cmd = bufferString("%s", command);
    size_t length = strlen(cmd);

    // Command ends at '\n' or # when a comment begins
    for (size_t i = 0; i < length; i++) {
        if (cmd[i] == '\n' || cmd[i] == '#') {
            cmd[i] = '\0';
            break;
        }
    }

    char *token = strtok(cmd, " \t");
    if (token != NULL) {
        // token is the command to execute
        // get arguments
        std::vector<char *> args;
        char *next;
        while ((next = strtok(NULL, " \t")) != NULL) {
            args.push_back(next);
        }

        // Execute
        returnValue = this->command(token, &args);
    }

    free(cmd);
    return returnValue;
}

int OpenRaider::command(const char *command, std::vector<char *> *args) {
    assert(command != NULL);
    assert(command[0] != '\0');
    assert(args != NULL);

    if (strcmp(command, "set") == 0) {
        if (args->size() != 2) {
            printf("Invalid use of set-command ");
            printStringVector(args);
            printf("\n");
            return -2;
        } else {
            return set(args->at(0), args->at(1));
        }
    } else if (strcmp(command, "bind") == 0) {
        if (args->size() != 2) {
            printf("Invalid use of bind-command ");
            printStringVector(args);
            printf("\n");
            return -3;
        } else {
            return bind(args->at(0), args->at(1));
        }
    } else {
        printf("Unknown command: %s ", command);
        printStringVector(args);
        printf("\n");
        return -1;
    }
}

char *OpenRaider::expandDirectoryNames(const char *s) {
    const char *base = "$(basedir)";
    const char *pak = "$(pakdir)";
    const char *audio = "$(audiodir)";
    const char *data = "$(datadir)";

    if (mBaseDir != NULL) {
        if (strstr(s, base) != NULL) {
            return stringReplace(s, base, mBaseDir);
        }
    }

    if (mPakDir != NULL) {
        if (strstr(s, pak) != NULL) {
            return stringReplace(s, pak, mPakDir);
        }
    }

    if (mAudioDir != NULL) {
        if (strstr(s, audio) != NULL) {
            return stringReplace(s, audio, mAudioDir);
        }
    }

    if (mDataDir != NULL) {
        if (strstr(s, data) != NULL) {
            return stringReplace(s, data, mDataDir);
        }
    }

    return NULL;
}

int OpenRaider::set(const char *var, const char *value) {
    if (strcmp(var, "size") == 0) {
        // value has format like "\"1024x768\""
        unsigned int w = DEFAULT_WIDTH, h = DEFAULT_HEIGHT;
        if (sscanf(value, "\"%dx%d\"", &w, &h) != 2) {
            printf("set-size-Error: Invalid value (%s)\n", value);
            return -2;
        }
        mWindow->setSize(w, h);
    } else if (strcmp(var, "fullscreen") == 0) {
        bool fullscreen = false;
        if (readBool(value, &fullscreen) != 0) {
            printf("set-fullscreen-Error: Invalid value (%s)\n", value);
            return -3;
        }
        mWindow->setFullscreen(fullscreen);
    } else if (strcmp(var, "gldriver") == 0) {
        mWindow->setDriver(value);
    } else if (strcmp(var, "audio") == 0) {
        bool audio = false;
        if (readBool(value, &audio) != 0) {
            printf("set-audio-Error: Invalid value (%s)\n", value);
            return -4;
        }
        mSound->setEnabled(audio);
    } else if (strcmp(var, "volume") == 0) {
        float vol = 1.0f;
        if (sscanf(value, "%f", &vol) != 1) {
            printf("set-volume-Error: Invalid value (%s)\n", value);
            return -5;
        }
        mSound->setVolume(vol);
    } else if (strcmp(var, "mouse_x") == 0) {
        // TODO mouse support
    } else if (strcmp(var, "mouse_y") == 0) {
        // TODO mouse support
    } else if (strcmp(var, "basedir") == 0) {
        char *quotes = stringReplace(value, "\"", "");
        char *tmp = expandDirectoryNames(quotes);
        if (tmp == NULL) {
            mBaseDir = fullPath(quotes, 0);
        } else {
            mBaseDir = fullPath(tmp, 0);
            delete [] tmp;
        }
        delete [] quotes;
    } else if (strcmp(var, "pakdir") == 0) {
        char *quotes = stringReplace(value, "\"", "");
        char *tmp = expandDirectoryNames(quotes);
        if (tmp == NULL) {
            mPakDir = fullPath(quotes, 0);
        } else {
            mPakDir = fullPath(tmp, 0);
            delete [] tmp;
        }
        delete [] quotes;
    } else if (strcmp(var, "audiodir") == 0) {
        char *quotes = stringReplace(value, "\"", "");
        char *tmp = expandDirectoryNames(quotes);
        if (tmp == NULL) {
            mAudioDir = fullPath(quotes, 0);
        } else {
            mAudioDir = fullPath(tmp, 0);
            delete [] tmp;
        }
        delete [] quotes;
    } else if (strcmp(var, "datadir") == 0) {
        char *quotes = stringReplace(value, "\"", "");
        char *tmp = expandDirectoryNames(quotes);
        if (tmp == NULL) {
            mDataDir = fullPath(quotes, 0);
        } else {
            mDataDir = fullPath(tmp, 0);
            delete [] tmp;
        }
        delete [] quotes;
    } else if (strcmp(var, "font") == 0) {
        char *quotes = stringReplace(value, "\"", "");
        char *tmp = expandDirectoryNames(quotes);
        if (tmp == NULL) {
            mWindow->setFont(quotes);
        } else {
            mWindow->setFont(tmp);
            delete [] tmp;
        }
        delete [] quotes;
    } else {
        printf("set-Error: Unknown variable (%s = %s)\n", var, value);
        return -1;
    }

    return 0;
}

int OpenRaider::bind(const char *action, const char *key) {
    if (strcmp(action, "console") == 0) {

    } else if (strcmp(action, "forward") == 0) {

    } else if (strcmp(action, "backward") == 0) {

    } else if (strcmp(action, "jump") == 0) {

    } else if (strcmp(action, "crouch") == 0) {

    } else if (strcmp(action, "left") == 0) {

    } else if (strcmp(action, "right") == 0) {

    } else {
        printf("bind-Error: Unknown action (%s --> %s)\n", key, action);
        return -1;
    }
    return 0;
}

int OpenRaider::initialize() {
    assert(mInit == false);
    assert(mRunning == false);

    // Initialize Windowing
    if (mWindow->initialize() != 0)
        return -1;

    // Initialize OpenGL
    if (mWindow->initializeGL() != 0)
        return -2;

    // Initialize window font
    if (mWindow->initializeFont() != 0)
        return -3;

    // Initialize sound
    if (mSound->initialize() != 0)
        return -4;

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

        glClear(GL_COLOR_BUFFER_BIT);

        WindowString s;
        s.text = bufferString("This text is not fixed-width...");
        s.x = 100;
        s.y = 100;
        s.scale = 1.5f;
        s.color[0] = s.color[1] = s.color[2] = 0xFF;
        mWindow->writeString(&s);

        mWindow->swapBuffersGL();

        clock_t stopTime = systemTimerGet();
        if (MAX_MS_PER_FRAME > (stopTime - startTime))
            mWindow->delay(MAX_MS_PER_FRAME - (stopTime - startTime));
    }
}

