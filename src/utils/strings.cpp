/*!
 * \file include/utils/strings.h
 * \brief String handling utilities
 *
 * \author xythobuz
 */

#include <cstdarg>
#include <cstdlib>
#include <stdio.h>
#include <string.h>

#include "utils/strings.h"

bool stringEndsWith(const char *str, const char *suffix) {
    if (!str || !suffix)
        return false;

    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);

    if (lensuffix > lenstr)
        return false;

    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

char *bufferString(const char *string, ...)
{
    int sz = 60;
    int n;
    char *text;
    va_list args;


    // Mongoose 2002.01.01, Only allow valid strings
    //   we must assume it's NULL terminated also if it passes...
    if (!string || !string[0])
    {
        return NULL;
    }

    text = new char[sz];

    va_start(args, string);

    // Mongoose 2002.01.01, Get exact size needed if the first try fails
    n = vsnprintf(text, sz, string, args);

    // Mongoose 2002.01.01, Realloc correct amount if truncated
    while (1)
    {
        if (n > -1 && n < sz)
        {
            break;
        }

        // Mongoose 2002.01.01, For glibc 2.1
        if (n > -1)
        {
            sz = n + 1;
            delete [] text;
            text = new char[sz];
            n = vsnprintf(text, sz, string, args);
            break;
        }
        else // glibc 2.0
        {
            sz *= 2;
            delete [] text;
            text = new char[sz];
            n = vsnprintf(text, sz, string, args);
        }
    }

    va_end(args);

    return text;
}


char *fullPath(const char *path, char end)
{
    unsigned int i, lenPath, lenEnv, len;
    char *env, *dir;


    if (!path || !path[0])
        return 0;

    if (path[0] == '~')
    {
#if defined(unix) || defined(__APPLE__)
        env = getenv("HOME");

        if (!env || !env[0])
        {
            return 0;
        }

        lenEnv = strlen(env);
        lenPath = strlen(path);
        len = lenEnv + lenPath;

        dir = new char[len+1];

        // Mongoose 2002.08.17, Copy ENV,  strip '~', append rest of path
        for (i = 0; i < len; ++i)
        {
            if (i < lenEnv)
            {
                dir[i] = env[i];
            }
            else
            {
                dir[i] = path[1+(i-lenEnv)];
            }
        }
#else
#error Platform not supported!
#endif
    }
    else
    {
        lenPath = strlen(path);
        dir = new char[lenPath+1];
        strncpy(dir, path, lenPath);

        i = lenPath;
    }

    // Make sure ends in "end" char
    if (end && dir[i-1] != end)
    {
        dir[i++] = end;
    }

    dir[i] = 0;

    return dir;
}


char *getFileFromFullPath(char *filename)
{
    int i, j, len;
    char *str;


    len = strlen(filename);

    for (i = len, j = 0; i > 0; --i, ++j)
    {
        if (filename[i] == '/' || filename[i] == '\\')
            break;
    }

    j--;

    str = new char[len - j + 1];

    for (i = 0; i < len - j; ++i)
    {
        str[i] = filename[i + len - j];
    }

    str[i] = 0;

    return str;
}

// Mongoose 2002.03.23, Checks command to see if it's same
//   as symbol, then returns the arg list in command buffer
bool rc_command(const char *symbol, char *command)
{
    int i, j, lens, lenc;


    if (!symbol || !symbol[0] || !command || !command[0])
    {
        return false;
    }

    lens = strlen(symbol);

    if (strncmp(command, symbol, lens) == 0)
    {
        lenc = strlen(command);

        // lens+1 skips '=' or ' '
        for (i = 0, j = lens+1; j < lenc; ++i, ++j)
        {
            command[i] = command[j];
            command[i+1] = 0;
        }

        return true;
    }

    return false;
}


int rc_get_bool(char *buffer, bool *val)
{
    if (!buffer || !buffer[0])
    {
        return -1;
    }

    if ((strncmp(buffer, "true", 4) == 0) || (buffer[0] == '1'))
        *val = true;
    else if ((strncmp(buffer, "false", 5) == 0) || (buffer[0] == '0'))
        *val = false;
    else
        return -2;

    return 0;
}

