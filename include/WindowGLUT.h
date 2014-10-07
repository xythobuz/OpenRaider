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

    /*!
     * \brief Constructs an object of WindowGLUT
     */
    WindowGLUT();

    /*!
     * \brief Deconstructs an object of WindowGLUT
     */
    virtual ~WindowGLUT();

    virtual void setSize(unsigned int width, unsigned int height);

    virtual void setFullscreen(bool fullscreen);

    virtual void setMousegrab(bool grab);

    virtual bool getMousegrab();

    virtual int initialize();

    virtual void eventHandling();

    virtual void setTextInput(bool on);

    virtual bool getTextInput();

    virtual void delay(unsigned int ms);

    virtual void swapBuffersGL();

private:
};

#endif

