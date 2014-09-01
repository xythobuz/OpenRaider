/*!
 * \file src/Window.cpp
 * \brief windowing implementation
 *
 * \author xythobuz
 */

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cmath>

#include "global.h"
#include "math/math.h"
#include "utils/strings.h"
#include "Window.h"

#ifdef USING_SDL
#include "WindowSDL.h"
#else
#error No Windowing Library selected!
#endif

Window &getWindow() {
#ifdef USING_SDL
    static WindowSDL gWindow;
#endif
    return gWindow;
}

unsigned int Window::getWidth() {
    return mWidth;
}

unsigned int Window::getHeight() {
    return mHeight;
}

int Window::initializeGL() {
    // Print driver support information
    //printf("GL Vendor  : %s\n", glGetString(GL_VENDOR));
    //printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
    //printf("GL Version : %s\n", glGetString(GL_VERSION));

    // Testing for goodies
    const char *s = (const char *)glGetString(GL_EXTENSIONS);
    if ((s != NULL) && (s[0] != '\0')) {
        //! \todo MultiTexture flag
        //if (strstr(s, "GL_ARB_multitexture"))
            //mFlags |= Render::fMultiTexture;
    }

    // Set up Z buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set up culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    //glFrontFace(GL_CCW);
    //glCullFace(GL_FRONT);

    // Set background to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Disable lighting
    glDisable(GL_LIGHTING);

    // Set up alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable(GL_ALPHA_TEST); // Disable per pixel alpha blending
    glAlphaFunc(GL_GREATER, 0);

    glPointSize(5.0);

    // Setup shading
    glShadeModel(GL_SMOOTH);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_FOG_HINT, GL_NICEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DITHER);

    // AA polygon edges
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_AUTO_NORMAL);
    glDisable(GL_LOGIC_OP);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_FOG);

    glDisable(GL_NORMALIZE);

    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_EDGE_FLAG_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glPolygonMode(GL_FRONT, GL_FILL);

    glMatrixMode(GL_MODELVIEW);

    return 0;
}

void Window::resizeGL() {
    float fovY = 40.0f; // 45.0f
    float clipNear = 10.0f; // 4.0f
    float clipFar = 600000.0f; // 4000.0f

    glViewport(0, 0, mWidth, mHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Adjust clipping
    GLfloat fH = tanf(fovY * OR_PI / 360.0f) * clipNear;
    GLfloat fW = fH * ((GLfloat)mWidth) / ((GLfloat)mHeight);
    glFrustum(-fW, fW, -fH, fH, clipNear, clipFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Window::glEnter2D() {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    /* This allows alpha blending of 2D textures with the scene */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, mWidth, mHeight);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0.0, (GLdouble)mWidth, (GLdouble)mHeight, 0.0, 0.0, 1.0);

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

// Replaced the deprecated gluLookAt with slightly modified code from here:
// http://www.khronos.org/message_boards/showthread.php/4991
void Window::lookAt(float eyeX, float eyeY, float eyeZ,
        float lookAtX, float lookAtY, float lookAtZ,
        float upX, float upY, float upZ) {
    // calculating the viewing vector
    float f[3] = {
        lookAtX - eyeX,
        lookAtY - eyeY,
        lookAtZ - eyeZ
    };

    // normalizing the viewing vector
    float fMag = sqrtf(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
    f[0] /= fMag;
    f[1] /= fMag;
    f[2] /= fMag;

    float s[3] = {
        (f[1] * upZ) - (upY * f[2]),
        (upX * f[2]) - (f[0] * upZ),
        (f[0] * upY) - (upX * f[1])
    };

    float u[3] = {
        (s[1] * f[2]) - (f[1] * s[2]),
        (f[0] * s[2]) - (s[0] * f[2]),
        (s[0] * f[1]) - (f[0] * s[1])
    };

    float m[16] = {
        s[0], u[0], -f[0], 0,
        s[1], u[1], -f[1], 0,
        s[2], u[2], -f[2], 0,
           0,    0,     0, 1
    };
    glMultMatrixf(m);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}

