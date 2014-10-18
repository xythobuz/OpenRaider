/*!
 * \file include/WindowGLUT.h
 * \brief GLUT windowing implementation
 *
 * \author xythobuz
 */

#ifndef _WINDOW_GLUT_H_
#define _WINDOW_GLUT_H_

#include "Window.h"

/*!
 * \brief GLUT windowing implementation
 */
class WindowGLUT : public Window {
  public:

    WindowGLUT();

    virtual void setSize(unsigned int width, unsigned int height);

    virtual void setFullscreen(bool fullscreen);

    virtual void setMousegrab(bool grab);

    virtual int initialize();

    virtual void eventHandling();

    virtual void setTextInput(bool on);

    virtual void swapBuffersGL();

  private:
    static void reshapeCallback(int width, int height);
    static void keyboardCallback(unsigned char key, int x, int y);
    static void keyboardUpCallback(unsigned char key, int x, int y);
    static void specialCallback(int key, int x, int y);
    static void specialUpCallback(int key, int x, int y);
    static void mouseCallback(int button, int state, int x, int y);
    static void motionCallback(int x, int y);
    static void mouseWheelCallback(int wheel, int direction, int x, int y);

    static KeyboardButton convertAsciiButton(unsigned char key);
    static KeyboardButton convertKeyCode(int key);
};

#endif

