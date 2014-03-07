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
#include <assert.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "Texture.h"
#include "utils/strings.h"
#include "GLString.h"

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
    assert(max_strings > 0);

    _num_string_max = max_strings;
    _string = new gl_string_t[max_strings];
}


void GLString::SetChar(unsigned int string, unsigned int pos, char c) {
    gl_string_t *str = GetString(string);
    assert(str != NULL);
    if (pos < str->len)
        str->text[pos] = c;
}


unsigned int GLString::GetStringLen(unsigned int string) {
    gl_string_t *str = GetString(string);
    assert(str != NULL);
    return str->len;
}


char *GLString::GetBuffer(unsigned int string) {
    gl_string_t *str = GetString(string);
    assert(str != NULL);
    return str->text;
}


void GLString::setActive(unsigned int string, bool active) {
    gl_string_t *str = GetString(string);
    assert(str != NULL);
    str->active = active;
}


void GLString::SetString(unsigned int string, const char *s, ...) {
    va_list args;
    gl_string_t *str = GetString(string);

    assert(s != NULL);
    assert(s[0] != '\0');
    assert(str != NULL);

    delete [] str->text;

    va_start(args, s);
    str->text = bufferString(s, args);
    va_end(args);

    str->active = true;
    str->len = (unsigned short)strlen(str->text);
}

void GLString::Scale(float scale) {
    _scale = scale;
}

int GLString::glPrintf(int x, int y, const char *string, ...) {
    va_list args;

    assert(string != NULL);
    assert(string[0] != '\0');

    if (_num_string > _num_string_max)
        return -2;

    va_start(args, string);

    _string[_num_string].active = true;
    _string[_num_string].scale = _scale;
    _string[_num_string].x = x;
    _string[_num_string].y = y;
    _string[_num_string].text = bufferString(string, args);

    //! \fixme All this is really unnecessary complex!
    _string[_num_string].len = (unsigned short)strlen(_string[_num_string].text);

    ++_num_string;

    va_end(args);

    return 0;
}

void GLString::Render() {
    for (unsigned int i = 0; i < _num_string; ++i) {
        if (_string[i].active)
            glPrint2d(_string[i].x, _string[i].y, _string[i].scale, _string[i].text);
    }
}

gl_string_t *GLString::GetString(unsigned int id) {
    assert(id < _num_string);

    return _string + id;
}

