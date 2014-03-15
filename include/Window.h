/*!
 * \file include/Window.h
 * \brief Windowing interface
 *
 * \author xythobuz
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <ctime>

typedef struct {
    char *text;
    unsigned int x;
    unsigned int y;
    float scale;
    unsigned char color[3];
} WindowString;

/*!
 * \brief Windowing interface
 */
class Window {
public:

    /*!
     * \brief Deconstructs an object of Window
     */
    virtual ~Window();

    virtual void setDriver(const char *driver) = 0;

    virtual void setSize(unsigned int width, unsigned int height) = 0;

    virtual void setFullscreen(bool fullscreen) = 0;

    virtual void setMousegrab(bool grab) = 0;

    virtual int initialize() = 0;

    virtual void eventHandling() = 0;

    virtual void delay(clock_t ms) = 0;

    virtual void swapBuffersGL() = 0;

    virtual void resizeGL(unsigned int w, unsigned int h);

    virtual void glEnter2D(unsigned int width, unsigned int height);

    virtual void glExit2D();

    virtual void setFont(const char *font) = 0;

    virtual int initializeFont() = 0;

    virtual void writeString(WindowString *s) = 0;
};

#endif
