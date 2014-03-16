/*!
 * \file include/OpenRaider.h
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#ifndef _OPENRAIDER_H_
#define _OPENRAIDER_H_

#include <vector>

#include "Menu.h"
#include "Sound.h"
#include "Window.h"

/*!
 * \brief Main Game Singleton
 */
class OpenRaider {
public:

    typedef enum {
        menu = 0,
        console,
        forward,
        backward,
        left,
        right,
        jump,
        crouch,
        use,
        holster,

        ActionEventCount // Should always be at the end
    } ActionEvents;

    /*!
     * \brief Constructs an object of OpenRaider
     */
    OpenRaider();

    /*!
     * \brief Deconstructs an object of OpenRaider
     */
    ~OpenRaider();

    /*!
     * \brief Load the configuration file
     * \returns 0 on success
     */
    int loadConfig(const char *config);

    int command(const char *command);

    int command(const char *command, std::vector<char *> *args);

    char *expandDirectoryNames(const char *s);

    int set(const char *var, const char *value);

    int bind(const char *action, const char *key);

    int bind(ActionEvents action, const char *key);

    int initialize();

    void run();

    void handleKeyboard(KeyboardButton key, bool pressed);

    void handleText(char *text, bool notFinished);

    void handleMouseClick(unsigned int x, unsigned int y, MouseButton button, bool released);

    Window *mWindow;
    Sound *mSound;
    Menu *mMenu;

    bool mMapListFilled;
    std::vector<char *> mMapList;

private:

    void loadPakFolderRecursive(const char *dir);

    void fillMapList();

    bool mInit;
    bool mRunning;

    char *mBaseDir;
    char *mPakDir;
    char *mAudioDir;
    char *mDataDir;

    KeyboardButton keyBindings[ActionEventCount];
};

#endif
