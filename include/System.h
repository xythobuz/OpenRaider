/*!
 * \file include/System.h
 * \brief Mostly defines the interface of System implementations.
 *
 * Currently only SDL is used, but there was a GLUT implementation.
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <map>

#include "Vector.h"
#include "utils/strings.h"

//! \todo Replace with unicode compatible key codes
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

typedef enum {
    SYS_MOD_KEY_LSHIFT = 1,
    SYS_MOD_KEY_RSHIFT = 2,
    SYS_MOD_KEY_LCTRL  = 4,
    SYS_MOD_KEY_RCTRL  = 8,
    SYS_MOD_KEY_LALT   = 16,
    SYS_MOD_KEY_RALT   = 32,
    SYS_MOD_KEY_LMETA  = 64,
    SYS_MOD_KEY_RMETA  = 128
} sdl_sys_mod_key_t;

/*!
 * \brief Basic Interface for System implementations (SDLSystem)
 */
class System {
public:
    /*!
     * \brief Constructs an object of System
     */
    System();

    /*!
     * \brief Deconstructs an object of System
     */
    virtual ~System();

    /*!
     * \brief Gets the game tick
     * \returns number of milliseconds since start of program
     */
    virtual unsigned int getTicks();

    /*!
     * \brief Created a directory
     * \param path Directory to create
     * \returns -1 on error
     */
    static int createDir(char *path);

    /*!
     * \brief Created a new Command Mode.
     * \param command valid command mode for the resource file, eg "[Engine.OpenGL.Driver]"
     * \returns id given to mode
     */
    virtual unsigned int addCommandMode(const char *command);

    /*!
     * \brief Binds a key to a command
     * \param cmd valid command string for event
     * \param key valid keyboard code
     * \param event valid game event id
     */
    virtual void bindKeyCommand(const char *cmd, unsigned int key, int event);

    /*!
     * \brief Executes a command string
     * \param cmd valid command string, cmd sets its var
     */
    virtual void command(const char *cmd);

    virtual void gameFrame() = 0;

    virtual void handleMouseMotionEvent(float x, float y) = 0;

    /*!
     * \brief Receives the event bound to the command from the key press
     * \param key key pressed
     */
    virtual void handleBoundKeyPressEvent(unsigned int key) = 0;

    /*!
     * \brief Receives the event bound to the command from the key release
     * \param key key released
     */
    virtual void handleBoundKeyReleaseEvent(unsigned int key) = 0;

    /*!
     * \brief Executes valid command based on keyword
     * \param command valid keyword, optionally followed by space separated arguments
     * \param mode current type or resource mode
     */
    virtual void handleCommand(char *command, unsigned int mode) = 0;

    /*!
     * \brief Receives key code from text input in console mode
     * \param key is a valid keyboard code
     * \param mod modifier key
     */
    virtual void handleConsoleKeyPressEvent(unsigned int key, unsigned int mod) = 0;

    virtual void handleKeyPressEvent(unsigned int key, unsigned int mod) = 0;

    virtual void handleKeyReleaseEvent(unsigned int key, unsigned int mod) = 0;

    virtual void initGL();

    virtual void initVideo(unsigned int width, unsigned int height, bool fullscreen) = 0;

    /*!
     * \brief Init the resource vars
     * \param filename resource file
     * \returns < 0 on error
     */
    virtual int loadResourceFile(const char *filename);

    static void resetTicks();

    virtual void resizeGL(unsigned int width, unsigned int height);

    virtual void runGame() = 0;

    /*!
     * \brief Turns console key events on/off
     * Mostly for allowing text entry vs key impulse commands
     * \param on new state
     */
    void setConsoleMode(bool on);

    void setDriverGL(const char *driver);

    virtual void shutdown(int code) = 0;

    virtual void swapBuffersGL() = 0;

    virtual void toggleFullscreen() = 0;

protected:
    unsigned int m_width;              //!< Width of the viewport
    unsigned int m_height;             //!< Height of the viewport
    char *m_driver;                    //!< String for dynamic use of GL library
    float m_clipNear;                  //!< Clip near distance
    float m_clipFar;                   //!< Clip far distance
    float m_fovY;                      //!< Field of vision
    std::map<unsigned int, int> mKeyEvents; //!< Single key press event mappings
    bool mConsoleMode;                 //!< Using text (console) event handler?
    Vector<const char *> mCmdModes;    //!< Dynamic resource command collection
    unsigned int mCommandMode;         //!< Current resource command mode
    unsigned int mConsoleKey;          //!< Console toggle event now handled lower
};

//! \todo Could make these static methods later, depends on API evolution

/*!
 * \brief Sets timer state and returns number of ticks
 * \param state 0 - reset, 1 - get number of ticks
 * \returns number of ticks
 */
unsigned int system_timer(int state);

#endif
