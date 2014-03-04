/*!
 * \file include/SDLSystem.h
 * \brief SDL System interface implementation
 *
 * \author Mongoose
 */

#ifndef _SDLSYSTEM_H_
#define _SDLSYSTEM_H_

#include "SDL.h"

#include "Config.h"
#include "System.h"

/*!
 * \brief SDL System interface implementation
 */
class SDLSystem : public System {
public:

    /*!
     * \brief Constructs an object of SDLSystem
     */
    SDLSystem();

    /*!
     * \brief Deconstructs an object of SDLSystem
     */
    virtual ~SDLSystem();

    /*!
     * \brief Get ticks
     * \returns number of milliseconds since start of program
     */
    virtual unsigned int getTicks();

    /*
     * \brief Sets Event binding Cmd to Key press
     * \param cmd valid command string
     * \param key valid keyboard code
     * \param event valid game event id
     */
    //! \fixme void bindKeyCommand(const char *cmd, int key, int event);

    /*!
     * \brief Renders string in OpenGL (2D projection).
     *
     * Requires Texture::glEnterMode2d() call before entry.
     *
     * System::bufferString(..) can cache printf() style calls
     * for use with this method.
     *
     * Call Texture::glExitMode2d after finishing calls to this method
     * and other 2D rendering.
     *
     * \param x valid X world coordinate
     * \param y valid Y world coordinate
     * \param string valid string
     *
     * \sa System::bufferString()
     * \sa Texture::glEnterMode2d()
     * \sa Texture::glExitMode2d()
     */
    void glPrintf2d(float x, float y, char *string);

    /*!
     * \brief Renders string in OpenGL (3D projection).
     *
     * System::bufferString(..) can cache printf() style calls
     * for use with this method.
     *
     * \param x valid X world coordinate
     * \param y valid Y world coordinate
     * \param z valid Z world coordinate
     * \param string valid string
     *
     * \sa System::bufferString()
     */
    void glPrintf3d(float x, float y, float z, char *string);

    /*!
     * \brief Start up video subsystem
     * \param width valid video mode width
     * \param height valid video mode height
     * \param fullscreen enables fullscreen rendering
     */
    void initVideo(unsigned int width, unsigned int height, bool fullscreen);

    /*!
     * \brief Resizes game window
     * \param width new width
     * \param height new height
     */
    void resize(unsigned int width, unsigned int height);

    /*!
     * \brief Start game loop
     */
    void runGame();

    void setGrabMouse(bool on);

    /*!
     * \brief Shuts down the game subsystems, exits game loop
     * \param i exit code
     */
    void shutdown(int i);

    /*!
     * \brief Swaps OpenGL buffers.
     * Call at end of frame.
     */
    void swapBuffersGL();

    /*!
     * \brief Toggle fullscreen windowing mode
     */
    void toggleFullscreen();

protected:
    bool mFullscreen;      //!< Current Fullscreen/Windowed mode

private:
    SDL_Window *mWindow;      //!< This is the pointer to the SDL surface
    SDL_GLContext mGLContext; //!< The OpenGL Context
};

#endif
