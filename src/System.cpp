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

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#if defined(linux) || defined(__APPLE__)
#include <time.h>
#include <sys/time.h>
#endif

#include "utils/math.h"
#include "System.h"

////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

System::System()
{
    m_width = 800;
    m_height = 600;

    m_driver = 0x0;

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


System::~System()
{
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////


unsigned int System::getTicks()
{
    return system_timer(1);
}


int System::createDir(char *path)
{
#ifdef WIN32
    return _mkdir(path);
#else
    return mkdir(path, S_IRWXU | S_IRWXG);
#endif
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

unsigned int System::addCommandMode(const char *command)
{
    if (command && command[0] == '[')
    {
        mCmdModes.pushBack(command);
        return (mCmdModes.size() - 1);
    }
    else
    {
        return 0;
    }
}

//! \fixme Modifer support later
void System::bindKeyCommand(const char *cmd, unsigned int key, int event)
{
    printf("Bound command '%s' -> event %i (0x%x key)\n", cmd, event, key);
    mKeyEvents[key] = event;
}


void System::command(const char *cmd)
{
    bool modeFound = false;
    char *cmdbuf;


    if (!cmd || !cmd[0]) // Null command string
        return;

    if (cmd[0] == '[') // Set a mode, eg "[Engine.OpenGL.Driver]"
    {
        for (mCmdModes.start(); mCmdModes.forward(); mCmdModes.next())
        {
            if (strcmp(cmd, mCmdModes.current()) == 0)
            {
                mCommandMode = mCmdModes.getCurrentIndex();
                modeFound = true;
            }
        }

        if (!modeFound)
        {
            //  mCommandMode = 0;
            printf("Command> Unknown mode '%s'\n", cmd);
        }
    }
    else // Execute a command in current mode, eg "stat fps"
    {
        cmdbuf = new char[strlen(cmd) + 1];
        strncpy(cmdbuf, cmd, strlen(cmd) + 1);
        handleCommand(cmdbuf, mCommandMode);
    }
}


int System::loadResourceFile(const char *filename)
{
    char buffer[256];
    bool line_comment = false;
    FILE *f;
    char c;
    int i, j;


    f = fopen(filename, "r");

    if (!f)
    {
        perror(filename);
        return -1;
    }

    printf("Loading %s...\n", filename);

    i = 0;
    buffer[0] = 0;

    // Strip out whitespace and comments
    while (fscanf(f, "%c", &c) != EOF)
    {
        if (line_comment && c != '\n')
            continue;

        if (i > 254)
        {
            printf("loadResourceFile> Overflow handled\n");
            i = 254;
        }

        switch (c)
        {
            case '\v':
            case '\t':
                break;
            case '#':
                buffer[i++] = 0;
                line_comment = true;
                break;
            case '\n':
                if (line_comment)
                {
                    line_comment = false;
                }

                if (buffer[0] == 0)
                {
                    i = 0;
                    continue;
                }

                buffer[i] = 0;
                //printf("'%s'\n", buffer);

                // 'Preprocessor' commands
                if (buffer[0] == '@')
                {
                    if (strncmp(buffer, "@include ", 9) == 0)
                    {
                        for (j = 9; j < i; ++j)
                        {
                            buffer[j-9] = buffer[j];
                            buffer[j-8] = 0;
                        }

                        printf("Importing '%s'\n", buffer);

                        loadResourceFile(fullPath(buffer, 0));
                    }
                }
                else
                {
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


void System::setDriverGL(const char *driver)
{
    unsigned int len;


    if (m_driver)
    {
        delete [] m_driver;
    }

    if (driver && driver[0])
    {
        len = strlen(driver);
        m_driver = new char[len+1];
        strncpy(m_driver, driver, len);
        m_driver[len] = 0;
    }
}


void System::resetTicks()
{
    system_timer(0);
}


void System::resizeGL(unsigned int w, unsigned int h)
{
    if (!w || !h)
    {
        printf("resizeGL> ERROR assertions 'w > 0', 'h > 0' failed\n");
        return;
    }

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
}

////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
// Gobal helper functions
////////////////////////////////////////////////////////////


unsigned int system_timer(int state)
{
    static struct timeval start;
    static struct timeval stop;
    static struct timezone tz;


    switch (state)
    {
        case 0:
            gettimeofday(&start, &tz);
            break;
        case 1:
            gettimeofday(&stop, &tz);

            if (start.tv_usec > stop.tv_usec)
            {
                stop.tv_usec += 1000000;
                stop.tv_sec--;
            }

            stop.tv_usec -= start.tv_usec;
            stop.tv_sec -= start.tv_sec;

            return ((stop.tv_sec - start.tv_sec) * 1000) + ((stop.tv_usec - start.tv_usec) / 1000);
    }

    return 0;
}

