/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : OpenRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : SDLSystem
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments:
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2002.06.06:
 * Mongoose - Created
 ================================================================*/


#ifndef _SDLSYSTEM_H_
#define _SDLSYSTEM_H_

#ifdef SDL_INTERFACE
#   include <SDL/SDL.h>
#else
#   error "SDLSystem requires -DSDL_INTERFACE"
#endif

#include <System.h>

class SDLSystem : public System
{
 public:

    ////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////

    SDLSystem();
    /*------------------------------------------------------
     * Pre  :
     * Post : Constructs an object of SDLSystem
     *
     *-- History ------------------------------------------
     *
     * 2002.06.06:
     * Mongoose - Created
     ------------------------------------------------------*/

    virtual ~SDLSystem();
    /*------------------------------------------------------
     * Pre  : SDL object is allocated
     * Post : Deconstructs an object of SDLSystem
     *
     *-- History ------------------------------------------
     *
     * 2002.06.06:
     * Mongoose - Created
     ------------------------------------------------------*/


    ////////////////////////////////////////////////////////////
    // Public Accessors
    ////////////////////////////////////////////////////////////

    virtual unsigned int getTicks();
    /*------------------------------------------------------
     * Pre  :
     * Post : Returns number of milliseconds since start of
     *        program
     *
     *-- History ------------------------------------------
     *
     * 2002.06.06:
     * Mongoose - Created
     ------------------------------------------------------*/


    ////////////////////////////////////////////////////////////
    // Public Mutators
    ////////////////////////////////////////////////////////////

    //FIXME: void bindKeyCommand(const char *cmd, int key, int event);
    /*------------------------------------------------------
     * Pre  : <Cmd> is a valid command string
     *        <Key> is a valid keyboard code
     *        <Event> is a valid game event Id
     *
     * Post : Sets <Event> binding <Cmd> to <Key> press
     *
     *-- History ------------------------------------------
     *
     * 2003.06.03:
     * Mongoose - Created
     ------------------------------------------------------*/

   void glPrintf2d(float x, float y, char *string);
   /*------------------------------------------------------
    * Pre  : <X>, and <Y> are valid world coordinates
     *        <String> is a valid string
     *
     *        Requires glEnterMode2d() call before entry
     *
     *        System::bufferString(..) can cache printf()
     *        style calls for use with this method
     *
    * Post : Renders string in OpenGL ( 2d projection )
     *
     *        Call glExitMode2d() after finishing calls
     *        to this method and other 2d rendering
    *
    *-- History ------------------------------------------
    *
     * 2003.06.03:
     * Mongoose - Ported to SDL_TTF
     *
    * 2001.12.31:
    * Mongoose - Created
    ------------------------------------------------------*/

   void glPrintf3d(float x, float y, float z, char *string);
   /*------------------------------------------------------
    * Pre  : <X>, <Y>, and <Z> are valid world coordinates
     *        <String> is a valid string
     *
     *        System::bufferString(..) can cache printf()
     *        style calls for use with this method
     *
    * Post : Renders string in OpenGL ( 3d projection )
    *
    *-- History ------------------------------------------
    *
     * 2003.06.03:
     * Mongoose - Ported to SDL_TTF
     *
    * 2001.12.31:
    * Mongoose - Created
    ------------------------------------------------------*/

    void initVideo(unsigned int width, unsigned int height,
                        bool fullscreen);
    /*------------------------------------------------------
     * Pre  : Video mode request <Width> x <Height>
     *        <Fullscreen> enables fullscreen rendering
     *
     * Post : Start up video subsystem
     *
     *-- History ------------------------------------------
     *
     * 2002.06.06:
     * Mongoose - Created
     ------------------------------------------------------*/

    void resize(unsigned int width, unsigned int height);
    /*------------------------------------------------------
     * Pre  :
     * Post : Resizes game window
     *
     *-- History ------------------------------------------
     *
     * 2002.06.06:
     * Mongoose - Created, from old SDLUnitTest code
     ------------------------------------------------------*/

    void runGame();
    /*------------------------------------------------------
     * Pre  :
     * Post : Start game loop
     *
     *-- History ------------------------------------------
     *
     * 2002.06.06:
     * Mongoose - Created
     ------------------------------------------------------*/

    void setGrabMouse(bool on);
    /*------------------------------------------------------
     * Pre  :
     * Post :
     *
     *-- History ------------------------------------------
     *
     * 2003.01.05:
     * Mongoose - Created
     ------------------------------------------------------*/

    void shutdown(int i);
    /*------------------------------------------------------
     * Pre  :
     * Post : Shutsdown the game subsystems, exits game loop
     *
     *-- History ------------------------------------------
     *
     * 2002.06.06:
     * Mongoose - Created
     ------------------------------------------------------*/

    void swapBuffersGL();
    /*------------------------------------------------------
     * Pre  :
     * Post : Swaps OpenGL buufers ( call at end of frame )
     *
     *-- History ------------------------------------------
     *
     * 2002.06.06:
     * Mongoose - Created
     ------------------------------------------------------*/

    void toggleFullscreen();
    /*------------------------------------------------------
     * Pre  :
     * Post : Toggle fullscreen windowing mode
     *
     *-- History ------------------------------------------
     *
     * 2002.06.06:
     * Mongoose - Created
     ------------------------------------------------------*/

 protected:
    int m_old_width;
    int m_old_height;
    bool mFirstMouseEvent; // Filter first mouse event (SDL bug)
    bool mFullscreen; //!< Current Fullscreen/Windowed mode

 private:

    ////////////////////////////////////////////////////////////
    // Private Accessors
    ////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////
    // Private Mutators
    ////////////////////////////////////////////////////////////

    SDL_Surface *mWindow;   /* This is the pointer to the SDL surface */
};
#endif
