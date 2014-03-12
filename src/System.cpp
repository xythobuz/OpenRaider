/*!
 * \file src/System.cpp
 * \brief Mostly defines the interface of System implementations.
 *
 * Currently only SDL is used, but there was a GLUT implementation.
 *
 * \author Mongoose
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>
#include <cmath>
#include <assert.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "utils/math.h"
#include "utils/time.h"
#include "System.h"

System::System() {
    m_width = 800;
    m_height = 600;

    m_driver = NULL;

    m_clipFar  = 4000.0f;
    m_clipNear = 4.0f;
    m_fovY     = 45.0f;

    mConsoleMode = false;
    mCommandMode = 0;

    printf("[System.Core]\n");

    // Hack for bad Map class, as well as reserved commands
    addCommandMode("[System.Console]");
    mConsoleKey = '`';
    bindKeyCommand("+console", mConsoleKey, 0);

#ifdef WIN32
    setDriverGL("libGL32.dll");
#else
    setDriverGL("/usr/lib/libGL.so.1");
#endif
}

System::~System() {
}

unsigned int System::addCommandMode(const char *command) {
    assert(command != NULL);
    assert(command[0] == '[');

    mCmdModes.pushBack(command);
    return (mCmdModes.size() - 1);
}

//! \fixme Modifer support later
void System::bindKeyCommand(const char *cmd, unsigned int key, int event) {
    assert(cmd != NULL);
    assert(cmd[0] != '\0');

    printf("Bound command '%s' -> event %i (0x%x key)\n", cmd, event, key);
    mKeyEvents[key] = event;
}

void System::command(const char *cmd) {
    bool modeFound = false;
    char *cmdbuf;

    assert(cmd != NULL);
    assert(cmd[0] != '\0');

    if (cmd[0] == '[') { // Set a mode, eg "[Engine.OpenGL.Driver]"
        for (mCmdModes.start(); mCmdModes.forward(); mCmdModes.next()) {
            if (strcmp(cmd, mCmdModes.current()) == 0) {
                mCommandMode = mCmdModes.getCurrentIndex();
                modeFound = true;
            }
        }

        if (!modeFound) {
            //  mCommandMode = 0;
            printf("Command> Unknown mode '%s'\n", cmd);
        }
    } else { // Execute a command in current mode, eg "stat fps"
        cmdbuf = new char[strlen(cmd) + 1];
        strncpy(cmdbuf, cmd, strlen(cmd) + 1);
        handleCommand(cmdbuf, mCommandMode);
    }
}

int System::loadResourceFile(const char *filename) {
    char buffer[256];
    bool line_comment = false;
    FILE *f;
    char c;
    int i, j;

    assert(filename != NULL);
    assert(filename[0] != '\0');

    f = fopen(filename, "r");

    if (!f) {
        perror(filename);
        return -1;
    }

    printf("Loading %s...\n", filename);

    i = 0;
    buffer[0] = 0;

    // Strip out whitespace and comments
    while (fscanf(f, "%c", &c) != EOF) {
        if (line_comment && c != '\n')
            continue;

        if (i > 254) {
            printf("loadResourceFile> Overflow handled\n");
            i = 254;
        }

        switch (c) {
            case '\v':
            case '\t':
                break;
            case '#':
                buffer[i++] = 0;
                line_comment = true;
                break;
            case '\n':
                if (line_comment)
                    line_comment = false;

                if (buffer[0] == 0) {
                    i = 0;
                    continue;
                }

                buffer[i] = 0;
                //printf("'%s'\n", buffer);

                // 'Preprocessor' commands
                if (buffer[0] == '@') {
                    if (strncmp((buffer + 1), "include ", 8) == 0) {
                        for (j = 9; j < i; ++j) {
                            buffer[j-9] = buffer[j];
                            buffer[j-8] = 0;
                        }

                        printf("Importing '%s'\n", buffer);

                        loadResourceFile(fullPath(buffer, 0));
                    }
                } else {
                    command(buffer);
                }

                i = 0;
                buffer[0] = 0;
                break;
            default:
                buffer[i++] = c;
        }
    }

    fclose(f);

    return 0;
}

void System::setDriverGL(const char *driver) {
    assert(driver != NULL);
    assert(driver[0] != '\0');

    if (m_driver)
        delete [] m_driver;

    unsigned int len = strlen(driver);
    m_driver = new char[len + 1];
    strncpy(m_driver, driver, len);
    m_driver[len] = 0;
}

void System::resizeGL(unsigned int w, unsigned int h) {
    assert(w > 0);
    assert(h > 0);

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Adjust clipping
    // gluPerspective is deprecated!
    // gluPerspective(m_fovY, ((GLdouble)w)/((GLdouble)h), m_clipNear, m_clipFar);
    // Fix: http://stackoverflow.com/a/2417756
    GLfloat fH = tanf(m_fovY * HEL_PI / 360.0f) * m_clipNear;
    GLfloat fW = fH * ((GLfloat)w)/((GLfloat)h);
    glFrustum(-fW, fW, -fH, fH, m_clipNear, m_clipFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

