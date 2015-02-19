/*!
 * \file include/system/Sound.h
 * \brief This is the audio manager Header
 *
 * \author xythobuz
 */

#ifndef _SOUND_H_
#define _SOUND_H_

class Sound {
  public:
    static int initialize();
    static void shutdown();
    static void clear();

    static int numBuffers();
    static int loadBuffer(unsigned char* buffer, unsigned int length);

    static int numSources(bool atListener = false);
    static int addSource(int buffer, float volume = 1.0f, bool atListener = false, bool loop = false);

    static int sourceAt(int source, glm::vec3 pos);
    static void listenAt(glm::vec3 pos, glm::vec3 at, glm::vec3 up);

    static void play(int source, bool atListener = false);
    static void stopAll();

    static void setEnabled(bool on = true);
    static bool getEnabled();

    static void setVolume(float vol = 1.0f);
    static float getVolume();
};

#endif

