/*!
 * \file include/Window.h
 * \brief Windowing interface
 *
 * \author xythobuz
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <ctime>

typedef enum {
    zero = '0', one = '1',
    two = '2', three = '3',
    four = '4', five = '5',
    six = '6', seven = '7',
    eight = '8', nine = '9',
    a = 'a', b = 'b',
    c = 'c', d = 'd',
    e = 'e', f = 'f',
    g = 'g', h = 'h',
    i = 'i', j = 'j',
    k = 'k', l = 'l',
    m = 'm', n = 'n',
    o = 'o', p = 'p',
    q = 'q', r = 'r',
    s = 's', t = 't',
    u = 'u', v = 'v',
    w = 'w', x = 'x',
    y = 'y', z = 'z',
    quote, backslash, backspace, capslock,
    comma, del, up, down, left, right,
    end, equals, escape, f1, f2, f3, f4, f5,
    f6, f7, f8, f9, f10, f11, f12, backquote,
    home, insert, leftalt, leftctrl, leftbracket,
    leftgui, leftshift, minus, numlock, pagedown,
    pageup, pause, dot, rightalt, rightctrl, enter,
    rightgui, rightbracket, rightshift, scrolllock,
    semicolon, slash, space, tab, unknown
} KeyboardButton;

typedef struct {
    char *text;
    unsigned int x;
    unsigned int y;
    int w;
    int h;
    float scale;
    unsigned char color[4];
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

    virtual void writeString(WindowString *s) = 0;

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

#endif
