/*!
 * \file include/OpenRaider.h
 * \brief Main Game Object
 *
 * \author xythobuz
 */

#ifndef _OPENRAIDER_H_
#define _OPENRAIDER_H_

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

    int initialize();

    void run();

    void cleanup();

private:
    bool mInit;
    bool mRunning;
    Window *mWindow;
};

#endif
