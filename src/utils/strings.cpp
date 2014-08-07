/*!
 * \file src/utils/strings.cpp
 * \brief String handling utilities
 *
 * \author xythobuz
 * \author Mongoose
 */

#include <cstdlib>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) && (!defined(va_copy))
#define va_copy(d,s) ((d) = (s))
#endif

#include "global.h"
#include "utils/filesystem.h"
#include "utils/strings.h"

char *stringRemoveQuotes(const char *s) {
    size_t length = strlen(s);
    if ((s[0] == '"') && (s[length - 1] == '"')) {
        char *buf = new char[length - 1];
        for (size_t i = 1; i < (length - 1); i++)
            buf[i - 1] = s[i];
        buf[length - 2] = '\0';
        return buf;
    } else {
        return bufferString("%s", s);
    }
}

char *stringReplace(const char *s, const char *search, const char *replace) {
    const char *tmp = strstr(s, search);
    if (tmp == NULL)
        return bufferString("%s", s);
    size_t offset = tmp - s;

    size_t length = strlen(s) - strlen(search) + strlen(replace);
    char *buf = new char[length + 1];
    buf[length] = '\0';

    for (size_t i = 0; i < offset; i++)
        buf[i] = s[i];

    for (size_t i = 0; i < strlen(replace); i++)
        buf[offset + i] = replace[i];

    for (size_t i = (offset + strlen(search)); i < strlen(s); i++)
        buf[i + strlen(replace) - strlen(search)] = s[i];

    char *ret = stringReplace(buf, search, replace);
    delete [] buf;
    return ret;
}

int readBool(const char *value, bool *var) {
    if ((strcmp(value, "1") == 0) || (strcmp(value, "true") == 0) || (strcmp(value, "on") == 0)) {
        *var = true;
    } else if ((strcmp(value, "0") == 0) || (strcmp(value, "false") == 0) || (strcmp(value, "off") == 0)) {
        *var = false;
    } else {
        return -1;
    }

    return 0;
}

bool stringEndsWith(const char *str, const char *suffix) {
    assert(str != NULL);
    assert(suffix != NULL);

    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);

    if (lensuffix > lenstr)
        return false;

    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

char *bufferString(const char *string, va_list args) {
    int sz = 60;
    int n;
    char *text;
    va_list tmp;

    assert(string != NULL);
    assert(string[0] != '\0');

    text = new char[sz];

    va_copy(tmp, args);
    n = vsnprintf(text, sz, string, tmp);
    va_end(tmp);

    if (n < 0) {
        delete [] text;
        return NULL; // encoding error
    } else if (n >= sz) {
        sz = n + 1; // buffer too small
        delete [] text;
        text = new char[sz + 1];
        va_copy(tmp, args);
        vsnprintf(text, sz, string, tmp);
        va_end(tmp);
    }

    return text;
}

char *bufferString(const char *string, ...) {
    va_list args;
    va_start(args, string);
    char *text = bufferString(string, args);
    va_end(args);
    return text;
}

char *fullPath(const char *path, char end) {
    size_t lenPath;
    char *dir;

    assert(path != NULL);
    assert(path[0] != '\0');

    if (path[0] == '~') {
        std::string home = getHomeDirectory();
        assert(home.length() > 0);
        lenPath = strlen(path);
        dir = new char[home.length() + lenPath + 3]; // space for end char
        strncpy(dir, home.c_str(), home.length());
        strncpy(dir + home.length(), path + 1, lenPath - 1);
        lenPath = home.length() + lenPath - 1;
    } else {
        lenPath = strlen(path);
        dir = new char[lenPath + 2]; // space for end char
        strncpy(dir, path, lenPath);
    }

    // Make sure ends in "end" char
    if ((lenPath > 0) && (end != 0) && (dir[lenPath - 1] != end)) {
        dir[lenPath] = end;
        dir[lenPath + 1] = '\0';
    } else {
        dir[lenPath] = '\0';
    }

    return dir;
}

