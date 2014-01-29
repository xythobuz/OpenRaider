/*!
 * \file src/GLString.cpp
 * \brief Open GL rendering font/string class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <Texture.h>
#include <GLString.h>

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif


GLString::GLString() {
    _num_string_max = 0;
    _num_string = 0;
    _scale = 1.0;
    _string = NULL;
}


GLString::~GLString() {
    if (_string) {
        for (unsigned int i = 0; i < _num_string; ++i) {
            if (_string[i].text)
                delete [] _string[i].text;
        }
        delete [] _string;
    }
}


void GLString::Init(unsigned int max_strings) {
    if (!max_strings)
        return;

    _num_string_max = max_strings;
    _string = new gl_string_t[max_strings];
}


void GLString::SetChar(unsigned int string, unsigned int pos, char c) {
    gl_string_t *str = GetString(string);
    if (str && pos < str->len)
        str->text[pos] = c;
}


unsigned int GLString::GetStringLen(unsigned int string) {
    gl_string_t *str = GetString(string);
    if (str)
        return str->len;
    return 0;
}


char *GLString::GetBuffer(unsigned int string) {
    gl_string_t *str = GetString(string);
    if (str)
        return str->text;
    return 0;
}


void GLString::setActive(unsigned int string, bool active) {
    gl_string_t *str = GetString(string);
    if (str)
        str->active = active;
}


void GLString::SetString(unsigned int string, const char *s, ...) {
    va_list args;
    gl_string_t *str;
    unsigned int len;

    str = GetString(string);

    if (s && s[0] && str) {
        str->active = true;
        len = strlen(s);
        if (len > str->len)
            len = str->len - 1;

        va_start(args, s);
        vsnprintf(str->text, str->len-2, s, args);
        str->text[str->len-1] = 0;
        va_end(args);
    }
}


void GLString::Scale(float scale) {
    _scale = scale;
}


int GLString::glPrintf(int x, int y, const char *string, ...) {
    int sz = 60;
    int n;
    va_list args;


    // Mongoose 2002.01.01, Only allow valid strings
    //   we must assume it's NULL terminated also if it passes...
    if (!string || !string[0])
        return -1;

    if (_num_string > _num_string_max)
        return -2;

    // Mongoose 2002.01.01, Assume no longer than 'sz' wide lines
    //   on first try
    _string[_num_string].text = new char[sz];

    // Mongoose 2002.01.03, Setup scale factor
    _string[_num_string].scale = _scale;

    //  Mongoose 2002.01.01, Setup position
    _string[_num_string].x = x;
    _string[_num_string].y = y;

    va_start(args, string);

    // Mongoose 2002.01.01, Get exact size needed if the first try fails
    n = vsnprintf(_string[_num_string].text, sz, string, args);

    // Mongoose 2002.01.01, Realloc correct amount if truncated
    while (1) {
        if (n > -1 && n < sz)
            break;

        if (n > -1) { // glibc 2.1
            sz = n + 1;
            delete [] _string[_num_string].text;
            _string[_num_string].text = new char[sz];
            n = vsnprintf(_string[_num_string].text, sz, string, args);
            break;
        } else { // glibc 2.0
            sz *= 2;
            delete [] _string[_num_string].text;
            _string[_num_string].text = new char[sz];
            n = vsnprintf(_string[_num_string].text, sz, string, args);
        }
    }
    va_end(args);

    // Mongoose 2002.01.04, Remeber string size, for future rebuffering use
    _string[_num_string].len = (unsigned short)sz;

    // Mongoose 2002.01.01, Incement string counter, since we just
    //   allocated a string
    ++_num_string;

    return 0;
}


void GLString::Render() {
    for (unsigned int i = 0; i < _num_string; ++i) {
        if (_string[i].active)
            glPrint2d(_string[i].x, _string[i].y, _string[i].scale, _string[i].text);
    }
}


gl_string_t *GLString::GetString(unsigned int id) {
    if (id < _num_string)
        return _string + id;
    return NULL;
}

