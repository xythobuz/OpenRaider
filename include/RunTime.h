/*!
 * \file include/RunTime.h
 * \brief run time configuration storage
 *
 * \author xythobuz
 */

#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include <string>
#include <vector>

class RunTime {
  public:
    static void initialize();
    static void updateFPS();
    static void display();

    static KeyboardButton getKeyBinding(ActionEvents event);
    static void setKeyBinding(ActionEvents event, KeyboardButton button);

    static std::string getBaseDir() { return baseDir; }
    static void setBaseDir(std::string dir) { baseDir = dir; }

    static std::string getPakDir() { return pakDir; }
    static void setPakDir(std::string dir) { pakDir = dir; }

    static std::string getAudioDir() { return audioDir; }
    static void setAudioDir(std::string dir) { audioDir = dir; }

    static std::string getDataDir() { return dataDir; }
    static void setDataDir(std::string dir) { dataDir = dir; }

    static bool isRunning() { return gameIsRunning; }
    static void setRunning(bool run) { gameIsRunning = run; }

    static bool getShowFPS() { return showFPS; }
    static void setShowFPS(bool fps) { showFPS = fps; }

    static unsigned long getFPS() { return fps; }
    static const std::vector<float>& getHistoryFPS() { return history; }
    static float getLastFrameTime() { return lastFrameTime / 1000.0f; }

  private:
    static std::string baseDir;
    static std::string pakDir;
    static std::string audioDir;
    static std::string dataDir;

    static KeyboardButton keyBindings[ActionEventCount];
    static bool gameIsRunning;
    static bool showFPS;

    static unsigned long lastTime, lastFrameTime;
    static unsigned long frameCount, frameCount2;
    static unsigned long frameTimeSum, frameTimeSum2;
    static unsigned long fps;
    static std::vector<float> history;
};

#endif

