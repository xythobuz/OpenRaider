/*!
 * \file include/Window.h
 * \brief Windowing interface
 *
 * \author xythobuz
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

/*!
 * \brief Windowing interface
 */
class Window {
  public:

    virtual ~Window() {}

    virtual void setSize(unsigned int width, unsigned int height) = 0;

    virtual void setFullscreen(bool fullscreen) = 0;

    virtual void setMousegrab(bool grab) = 0;

    virtual bool getMousegrab() { return mMousegrab; }

    virtual int initialize() = 0;

    virtual void eventHandling() = 0;

    virtual void setTextInput(bool on) = 0;

    virtual bool getTextInput() { return mTextInput; }

    virtual void swapBuffersGL() = 0;

    virtual unsigned int getWidth() { return mWidth; }

    virtual unsigned int getHeight() { return mHeight; }

    virtual int initializeGL();

    virtual void resizeGL();

    virtual void glEnter2D();

    virtual void glExit2D();

    static void lookAt(float eyeX, float eyeY, float eyeZ,
                       float lookAtX, float lookAtY, float lookAtZ,
                       float upX, float upY, float upZ);

  protected:
    bool mInit;
    bool mFullscreen;
    bool mMousegrab;
    bool mTextInput;
    unsigned int mWidth;
    unsigned int mHeight;
};

Window& getWindow();

#endif

