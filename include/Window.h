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
    int w;
    int h;
    float scale;
    float color[4];
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

    virtual void setTextInput(bool on) = 0;

    virtual void delay(clock_t ms) = 0;

    virtual void swapBuffersGL() = 0;

    virtual int initializeGL();

    virtual void resizeGL();

    virtual void glEnter2D();

    virtual void glExit2D();

    virtual void setFont(const char *font) = 0;

    virtual int initializeFont() = 0;

    virtual void writeString(WindowString &s) = 0;

    virtual void drawText(unsigned int x, unsigned int y, float scale, const float color[4], const char *s, ...)
        __attribute__((format(printf, 6, 0))) = 0;

    //! \fixme should be private
    unsigned int mWidth;
    unsigned int mHeight;

protected:
    bool mInit;
    char *mDriver;
    bool mFullscreen;
    bool mMousegrab;

    bool mFontInit;
    char *mFontName;
};

Window &getWindow();

#endif
