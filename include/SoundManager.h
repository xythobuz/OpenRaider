/*!
 * \file include/SoundManager.h
 * \brief This is the audio manager Header
 *
 * \author xythobuz
 */

#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include <vector>

class SoundSource {
  public:
    SoundSource(glm::vec3 p, int i, int f)
        : pos(p), id(i), flags(f), source(-1) { }
    void prepare();
    glm::vec3 getPos() { return pos; }
    int getID() { return id; }
    int getFlags() { return flags; }
    int getSource() { return source; }

  private:
    glm::vec3 pos;
    int id, flags, source;
};

class SoundDetail {
  public:
    SoundDetail(int s, float v) : sample(s), source(-1), volume(v) { }
    int getSample() { return sample; }
    float getVolume() { return volume; }
    int getSource() { return source; }
    void setSource(int s) { source = s; }

  private:
    int sample, source;
    float volume;
};

class SoundManager {
  public:
    static void clear();
    static int prepareSources();

    static void addSoundSource(glm::vec3 p, int id, int flags);
    static void addSoundMapEntry(int id);
    static void addSoundDetail(int sample, float volume);
    static void addSampleIndex(int index);

    // index --> SoundMap --> SoundDetails --> SampleIndices --> play
    static int getIndex(int index, float* volume = nullptr, SoundDetail** sd = nullptr);
    static int playSound(int index);

    static void listenAt(glm::vec3 pos, glm::vec3 at, glm::vec3 up);
    static void display();

  private:
    static std::vector<SoundSource> soundSources;
    static std::vector<int> soundMap;
    static std::vector<SoundDetail> soundDetails;
    static std::vector<int> sampleIndices;
};

#endif

