/*!
 * \file include/WindowSDL.h
 * \brief SDL windowing implementation
 *
 * \author xythobuz
 */

#ifndef _WINDOW_SDL_H_
#define _WINDOW_SDL_H_

#include "Window.h"

/*!
 * \brief SDL windowing implementation
 */
class WindowSDL : public Window {
public:

    /*!
     * \brief Constructs an object of WindowSDL
     */
    WindowSDL();

    /*!
     * \brief Deconstructs an object of WindowSDL
     */
    virtual ~WindowSDL();

    virtual void setSize(unsigned int width, unsigned int height);

    virtual void setFullscreen(bool fullscreen);

    virtual void setMousegrab(bool grab);

    virtual int initialize();

private:
    unsigned int mWidth;
    unsigned int mHeight;
    bool mFullscreen;
    bool mMousegrab;
};

#endif
