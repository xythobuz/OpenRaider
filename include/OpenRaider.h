/*!
 * \file include/OpenRaider.h
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#ifndef _OPENRAIDER_H_
#define _OPENRAIDER_H_

#include <vector>

#include "Sound.h"
#include "Window.h"

/*!
 * \brief Main Game Singleton
 */
class OpenRaider {
public:

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

    int set(const char *var, const char *value);

    int bind(const char *action, const char *key);

    int initialize();

    void run();

private:
    bool mInit;
    bool mRunning;
    Window *mWindow;

    Sound *mSound;
};

#endif
