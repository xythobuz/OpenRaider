/*!
 * \file include/OpenRaider.h
 * \brief Main Game Singleton
 *
 * \author Mongoose
 */

#ifndef _OPENRAIDER_H_
#define _OPENRAIDER_H_

#include <map>

#include "Config.h"
#include "TombRaider.h"
#include "Camera.h"
#include "Render.h"
#include "Sound.h"
#include "SDLSystem.h"
#include "Network.h"
#include "World.h"
#include "templates/List.h"
#include "templates/Vector.h"
#include "utils/strings.h"

/*!
 * \brief OpenRaider key events.
 *
 * 0 event reserved for console in System.
 */
typedef enum {
    OpenRaiderKey_console = 1, //!< Toggle console
    OpenRaiderKey_attack,      //!< Attack
    OpenRaiderKey_forward,     //!< Move forward
    OpenRaiderKey_left,        //!< Move left
    OpenRaiderKey_right,       //!< Move right
    OpenRaiderKey_backward,    //!< Move backward
    OpenRaiderKey_jump,        //!< Jump
    OpenRaiderKey_tiltUp,      //!< Tilt camera up
    OpenRaiderKey_tiltDown,    //!< Tilt camera down
    OpenRaiderKey_panLeft,     //!< Pan camera left
    OpenRaiderKey_panRight,    //!< Pan camera right
    OpenRaiderKey_crouch       //!< Crouch
} OpenRaider_KeyEvent;

typedef enum {
    OpenRaider_ShowFPS     = (1 << 0),
    OpenRaider_DebugMap    = (1 << 1),
    OpenRaider_DebugModel  = (1 << 2),
    OpenRaider_EnableSound = (1 << 3),
    OpenRaider_DumpTexture = (1 << 4),
    OpenRaider_FullScreen  = (1 << 5),
    OpenRaider_Loading     = (1 << 6)
} OpenRaider_Flags;

/*!
 * \brief Main Game Singleton
 */
class OpenRaider : public SDLSystem {
public:

    /*!
     * \brief Constructs or returns the OpenRaider Singleton
     * \returns OpenRaider Singleton
     */
    static OpenRaider *Instance();

    /*!
     * \brief Deconstructs an object of OpenRaider
     */
    ~OpenRaider();

    /*!
     * \brief Initialize the Game
     */
    void start();

    /*!
     * \brief Mouse motion input
     * \param x relative x motion
     * \param y relative y motion
     */
    void handleMouseMotionEvent(float x, float y);

    /*!
     * \brief Receives `Event` bound to `Cmd` from `Key` press
     * \param key valid keyboard code
     */
    void handleBoundKeyPressEvent(unsigned int key);

    /*!
     * \brief Receives `Event` bound to `Cmd` from `Key` release
     * \param key valid keyboard code
     */
    void handleBoundKeyReleaseEvent(unsigned int key);

    /*!
     * \brief Executes valid command based on keyword
     * \param command valid keyword optionally followed by space separated arguments
     * \param mode current type or resource mode
     */
    void handleCommand(char *command, unsigned int mode);

    /*!
     * \brief Receives `Key` code from text input in console mode
     * \param key valid keyboard code
     * \param mod valid modifier code
     */
    void handleConsoleKeyPressEvent(unsigned int key, unsigned int mod);

    /*!
     * \brief Executes command associated with key press, if any
     * \param key valid keyboard code
     * \param mod valid modifier code
     */
    void handleKeyPressEvent(unsigned int key, unsigned int mod);

    /*!
     * \brief Executes command associated with key release, if any
     * \param key valid keyboard code
     * \param mod valid modifier code
     */
    void handleKeyReleaseEvent(unsigned int key, unsigned int mod);

    void initGL();

    /*!
     * \brief Pass a time frame and render a new frame
     */
    void gameFrame();

    /*!
     * \brief Outputs message in game console
     * \param dump_stdout if true, also print to stdout
     * \param format printf() style format string
     */
    void print(bool dump_stdout, const char *format, ...) __attribute__((format(printf, 3, 4)));

protected:

    /*!
     * \brief Constructs an object of OpenRaider
     */
    OpenRaider();

private:

    void consoleCommand(char *cmd);

    void soundEvent(int type, int id, vec3_t pos, vec3_t angles);

    //void entityEvent(entity_t &e, RaiderEvent event);

    void processPakSounds();

    /*!
     * \brief Loads and positions level sounds and music.
     *
     * Sound system has to be initialized.
     */
    void initSound();

    /*!
     * \brief Generates textures or mipmaps for fonts, splash,
     * external particles.
     */
    void initTextures();

    /*!
     * \brief Generates tombraider textures or mipmaps for sprites,
     * rooms and models.
     */
    void processTextures();

    /*!
     * \brief Generates render sprite sequences
     */
    void processSprites();

    void processMoveables();

    void processMoveable(int index, int i, int *ent, List <skeletal_model_t *> &cache2, List <unsigned int> &cache, int object_id);

    /*!
     * \brief Generates render mesh and any textures needed
     * for the specified model.
     * \param index valid model index
     */
    void processModel(int index);

    /*!
     * \brief Generates render mesh and any textures needed
     * for the specified room
     * \param index valid room index
     */
    void processRoom(int index);

    /*!
     * \brief Loads validated level pak from diskfile using maplist
     * \param filename level pak file name
     */
    void loadLevel(char *filename);

    void loadPakFolderRecursive(const char *dir);

    void menuMapListMove(char dir, bool show);

    static OpenRaider *mInstance; //!< Singleton use
    TombRaider m_tombraider;      //!< Tombraider data support
    Sound mSound;                 //!< 3d Audio support
    Render m_render;              //!< Rendering support
    Camera m_camera;              //!< Client camera support
    GLString *mText;              //!< Hook to textbox like output

    // RC vars

    unsigned int mMode[8];            //!< Translate System's mode ids to OR's
    unsigned int m_flags;             //!< Set options by flags
    int m_testSFX;                    //!< Used for mixed channel sound tests
    float m_mouseX, m_mouseY;         //!< XY axis rotation deltas
    unsigned int m_texOffset;         //!< Offset of TombRaider textures in list
    unsigned int mLevelTextureOffset;

    // Game vars

    Vector <char *> mMusicList;  //!< List of game level music
    Vector <char *> mMapList;    //!< List of game maps
    char m_mapName[32];          //!< Current map filename
    char *m_pakDir;              //!< Current pak directory
    char *m_audioDir;            //!< Current audio directory
    char *m_homeDir;             //!< Current home directory
    Vector<entity_t *> mClients; //!< Player entity/clients
};

#endif
