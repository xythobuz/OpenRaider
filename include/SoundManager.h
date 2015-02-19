/*!
 * \file include/SoundManager.h
 * \brief This is the audio manager Header
 *
 * \author xythobuz
 */

#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include <vector>

struct SoundSource {
    glm::vec3 pos;
    int id, flags;

    SoundSource(glm::vec3 p, int i, int f)
        : pos(p), id(i), flags(f) { }
};

struct SoundDetail {
    int sample;
    float volume;

    SoundDetail(int s, float v) : sample(s), volume(v) { }
};

class SoundManager {
  public:
    static void clear();
    static int prepareSources();

    static void addSoundSource(glm::vec3 p, int id, int flags);
    static void addSoundMapEntry(int id);
    static void addSoundDetail(int sample, float volume);
    static void addSampleIndex(int index);

    static int getIndex(int index, float* volume = nullptr);

    // index --> SoundMap --> SoundDetails --> SampleIndices --> play
    static int playSound(int index);

    static void display();

  private:
    static std::vector<SoundSource> soundSources;
    static std::vector<int> soundMap;
    static std::vector<SoundDetail> soundDetails;
    static std::vector<int> sampleIndices;
};

#endif

