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

#if defined(unix) || defined(__APPLE__)
#include <wordexp.h>
#endif

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

char *bufferString(const char *string, ...) {
    int sz = 60;
    int n;
    char *text;
    va_list args;

    if (!string || !string[0])
        return NULL;

    text = new char[sz];

    va_start(args, string);

    n = vsnprintf(text, sz, string, args);

    if (n < 0) {
        delete [] text;
        return NULL; // encoding error
    } else if (n >= sz) {
        sz = n + 1; // buffer too small
        delete [] text;
        text = new char[sz];
        n = vsnprintf(text, sz, string, args);
    }

    va_end(args);

    return text;
}


char *fullPath(const char *path, char end) {
    unsigned int lenPath, offset;
    wordexp_t word;
    char *dir;

    if (!path || !path[0])
        return NULL;

    if (path[0] == '~') {
#if defined(unix) || defined(__APPLE__)
        // Expand string into segments
        if (wordexp(path, &word, 0) != 0) {
            return NULL;
        }

        // Get length of complete string
        lenPath = 0;
        for (unsigned int i = 0; i < word.we_wordc; i++) {
            lenPath += strlen(word.we_wordv[i]);
        }

        // Allocate new string
        dir = new char[lenPath + 2]; // space for end char

        // Copy segments into new string
        offset = 0;
        for (unsigned int i = 0; i < word.we_wordc; i++) {
            unsigned int len = strlen(word.we_wordv[i]);
            strncpy(dir + offset, word.we_wordv[i], len);
            offset += len;
        }

        wordfree(&word);
#else
#error Platform not supported!
#endif
    } else {
        lenPath = strlen(path);
        dir = new char[lenPath + 2]; // space for end char
        strncpy(dir, path, lenPath);
    }

    // Make sure ends in "end" char
    if (end && (dir[lenPath - 1] != end)) {
        dir[lenPath] = end;
        dir[lenPath + 1] = 0;
    } else {
        dir[lenPath] = 0;
    }

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

