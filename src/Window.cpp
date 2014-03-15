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

void Window::glEnter2D(unsigned int width, unsigned int height) {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    /* This allows alpha blending of 2D textures with the scene */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0.0, (GLdouble)width, (GLdouble)height, 0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void Window::glExit2D() {
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib();

    glMatrixMode(GL_MODELVIEW);
}

