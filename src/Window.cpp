/*!
 * \file src/Window.cpp
 * \brief windowing implementation
 *
 * \author xythobuz
 */

#include <assert.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "math/math.h"
#include "Window.h"

Window::~Window() {
}

void Window::resizeGL(unsigned int w, unsigned int h) {
    float fovY = 45.0f;
    float clipNear = 4.0f;
    float clipFar = 4000.0f;

    assert(w > 0);
    assert(h > 0);

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Adjust clipping
    GLfloat fH = tanf(fovY * OR_PI / 360.0f) * clipNear;
    GLfloat fW = fH * ((GLfloat)w) / ((GLfloat)h);
    glFrustum(-fW, fW, -fH, fH, clipNear, clipFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

