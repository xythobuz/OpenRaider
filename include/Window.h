/*!
 * \file include/Window.h
 * \brief Windowing interface
 *
 * \author xythobuz
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

typedef struct {
    char *text;
    unsigned int x;
    unsigned int y;
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

    virtual void writeString(WindowString *s) = 0;

    virtual void resizeGL(unsigned int w, unsigned int h);
};

#endif
