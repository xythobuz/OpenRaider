/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : UnRaider, OpenRaider, RaiderUnification 2003
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : System
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments:
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- Test Defines -----------------------------------------------
 *
 * UNIT_TEST_SYSTEM - Builds System class as a console unit test
 *
 *-- History ------------------------------------------------
 *
 * 2002.08.09:
 * Mongoose - Created
 ================================================================*/


#ifndef GUARD__UNRAIDER_MONGOOSE_SYSTEM_H_
#define GUARD__UNRAIDER_MONGOOSE_SYSTEM_H_

#include "mstl/Map.h"
#include "mstl/Vector.h"

// TODO: Replace with unicode compatible key codes
#define SYS_MOUSE_LEFT    6000
#define SYS_MOUSE_RIGHT   6001
#define SYS_MOUSE_MIDDLE  6002
#define SYS_KEY_ESC       27
#define SYS_KEY_ENTER     13
#define SYS_KEY_UP        5000
#define SYS_KEY_DOWN      5001
#define SYS_KEY_RIGHT     5002
#define SYS_KEY_LEFT      5003
#define SYS_KEY_PAGEDOWN  5004
#define SYS_KEY_PAGEUP    5005
#define SYS_KEY_F1        1000
#define SYS_KEY_F2        1001
#define SYS_KEY_F3        1002
#define SYS_KEY_F4        1003
#define SYS_KEY_F5        1004
#define SYS_KEY_F6        1005
#define SYS_KEY_F7        1006
#define SYS_KEY_F8        1007
#define SYS_KEY_F9        1008
#define SYS_KEY_F10       1009
#define SYS_KEY_F11       1010
#define SYS_KEY_F12       1011

typedef enum
{
  SYS_MOD_KEY_LSHIFT = 1,
  SYS_MOD_KEY_RSHIFT = 2,
  SYS_MOD_KEY_LCTRL  = 4,
  SYS_MOD_KEY_RCTRL  = 8,
  SYS_MOD_KEY_LALT   = 16,
  SYS_MOD_KEY_RALT   = 32,

  SYS_MOD_KEY_LMETA = 64,
  SYS_MOD_KEY_RMETA = 128,

} sdl_sys_mod_key_t;


class System
{
public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	System();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of System
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual ~System();
	/*------------------------------------------------------
	 * Pre  : System object is allocated
	 * Post : Deconstructs an object of System
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

   static char *bufferString(const char *string, ...);
   /*------------------------------------------------------
    * Pre  : <String> is a valid string with valid args
	 *
    * Post : Generates a buufered string for the printf
	 *        call
    *
    *-- History ------------------------------------------
    *
	 * 2003.06.03:
	 * Mongoose - Made into a printf string caching system
	 *
    * 2001.12.31:
    * Mongoose - Created, was GLString::glPrintf
    ------------------------------------------------------*/

	static char *fullPath(const char *path, char end);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns allocated string of path, with
	 *        expansion of unix home enviroment char and
	 *        makes sure string ends in "end" char
	 *
	 *        end = 0 appends no addional char
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.17:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	static char *getFileFromFullPath(char *filename);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.07.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

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

	static int downloadToBuffer(char *urlString,
										 unsigned char **buffer, unsigned int *size);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Downloads something into passed buffer,
	 *        Returns < 0 on error, 0 on sucess
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.07.12:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	static int downloadToFile(char *urlString, char *filename);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Downloads something into a disk file,
	 *        Returns < 0 on error, 0 on sucess
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.07.12:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	static int createDir(char *path);
	/*------------------------------------------------------
	 * Pre  : Creates directory
	 * Post : Returns -1 on error
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.07.12:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	virtual unsigned int addCommandMode(const char *command);
	/*------------------------------------------------------
	 * Pre  : <Command> is valid command mode for the
	 *        resource file, eg "[Engine.OpenGL.Driver]"
	 *
	 * Post : Returns Id given to mode
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.03.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void bindKeyCommand(const char *cmd, unsigned int key, int event);
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

	virtual void command(const char *cmd);
	/*------------------------------------------------------
	 * Pre  : cmd is a valid command string
	 * Post : cmd sets it's var
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.27:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void gameFrame() = 0;
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void handleMouseMotionEvent(float x, float y) = 0;
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void handleBoundKeyPressEvent(unsigned int key) = 0;
	/*------------------------------------------------------
	 * Pre  : <Key> is a valid keyboard code
	 *
	 * Post : Recieves <Event> bound to <Cmd> from <Key> press
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void handleBoundKeyReleaseEvent(unsigned int key) = 0;
	/*------------------------------------------------------
	 * Pre  : <Key> is a valid keyboard code
	 *
	 * Post : Recieves <Event> bound to <Cmd> from <Key> release
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void handleCommand(char *command, unsigned int mode) = 0;
	/*------------------------------------------------------
	 * Pre  : <Command> is valid keyword optionally followed
	 *        by ' ' (space) seperated and argument(s)
	 *
	 *        <Mode> is the current type or resource mode
	 *
	 * Post : Executes valid command based on keyword
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.03.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void handleConsoleKeyPressEvent(unsigned int key,
														 unsigned int mod) = 0;
	/*------------------------------------------------------
	 * Pre  : <Key> is a valid keyboard code
	 *
	 * Post : Recieves <Key> code from text input in console mode
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void handleKeyPressEvent(unsigned int key, unsigned int mod) = 0;
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void handleKeyReleaseEvent(unsigned int key, unsigned int mod) = 0;
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void initGL();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void initVideo(unsigned int width, unsigned int height,
								  bool fullscreen) = 0;
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual int loadResourceFile(const char *filename);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Init the resource vars
	 *
	 *        Returns less than zero value on error
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.27:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	static void resetTicks();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void resizeGL(unsigned int width, unsigned int height);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void runGame() = 0;
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setConsoleMode(bool on);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Turns console key events on/off,
	 *        mostly for allowing text entry vs key
	 *        impluse commands
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setDriverGL(const char *driver);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setFastCardPerformance(bool isFast);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void shutdown(int code) = 0;
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void swapBuffersGL() = 0;
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	virtual void toggleFullscreen() = 0;
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.08.09?:
	 * Mongoose - Created
	 ------------------------------------------------------*/

 protected:

	unsigned int m_width;      /* Width of the viewport */

	unsigned int m_height;     /* Height of the viewport */

	bool m_fastCard;           /* Assume expensive calls are fine if true */

	char *m_driver;            /* String for dynamic use of GL library */

	float m_clipNear;          /* Clip near distance */

	float m_clipFar;           /* Clip far distance */

	float m_fovY;              /* Field of vision */

	Map<unsigned int, int> mKeyEvents;	/* Single key press event mappings */

	bool mConsoleMode;         /* Using text (console) event handler? */

	Vector<const char *> mCmdModes;	/* Dynamic resource command collection */

	unsigned int mCommandMode;	/* Current resource command mode */

	unsigned int mConsoleKey;	/* Console toggle event now handled lower */

    bool mFullscreen;

 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////
};


// Could make these static methods later, depends on API evolution

	bool rc_command(const char *symbol, char *command);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns true if <Command> matches <Symbol>
	 *        command string
	 *
	 *        Returns the rest of the argument list back in
	 *        <Command> buffer, if any
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.03.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int rc_get_bool(char *buffer, bool *val);
	/*------------------------------------------------------
	 * Pre  : Buffer is "true" or "false"
	 *
	 * Post : Returns 0 if <Buffer> is "true" or "false"
	 *        and sets <Val> accordingly
	 *
	 *        Returns -1 for null string
	 *        Returns -2 if string isn't "true" or "false"
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.03.23:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int system_timer(int state);
	/*------------------------------------------------------
	 * Pre  : 0 - reset, 1 - get number of ticks
	 * Post : Sets timer state and returns number of ticks
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.06:
	 * Mongoose - Created
	 ------------------------------------------------------*/
#endif
