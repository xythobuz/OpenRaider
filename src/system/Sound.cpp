/*!
 * \file src/system/Sound.cpp
 * \brief This is the audio manager Implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "system/Sound.h"

#ifdef USING_AL
#include "system/SoundAL.h"
#endif

int Sound::initialize() {
#ifdef USING_AL
    return SoundAL::initialize();
#else
    return 0;
#endif
}

void Sound::shutdown() {
#ifdef USING_AL
    SoundAL::shutdown();
#endif
}

void Sound::clear() {
#ifdef USING_AL
    SoundAL::clear();
#endif
}

int Sound::numBuffers() {
#ifdef USING_AL
    return SoundAL::numBuffers();
#else
    return 0;
#endif
}

int Sound::loadBuffer(unsigned char* buffer, unsigned int length) {
#ifdef USING_AL
    return SoundAL::loadBuffer(buffer, length);
#else
    return 0;
#endif
}

int Sound::numSources(bool atListener) {
#ifdef USING_AL
    return SoundAL::numSources(atListener);
#else
    return 0;
#endif
}

int Sound::addSource(int buffer, float volume, bool atListener, bool loop) {
#ifdef USING_AL
    return SoundAL::addSource(buffer, volume, atListener, loop);
#else
    return 0;
#endif
}

int Sound::sourceAt(int source, glm::vec3 pos) {
#ifdef USING_AL
    return SoundAL::sourceAt(source, pos);
#else
    return 0;
#endif
}

void Sound::listenAt(glm::vec3 pos, glm::vec3 at, glm::vec3 up) {
#ifdef USING_AL
    SoundAL::listenAt(pos, at, up);
#endif
}

void Sound::play(int source, bool atListener) {
#ifdef USING_AL
    SoundAL::play(source, atListener);
#endif
}

void Sound::stopAll() {
#ifdef USING_AL
    SoundAL::stopAll();
#endif
}

void Sound::setEnabled(bool on) {
#ifdef USING_AL
    SoundAL::setEnabled(on);
#endif
}

bool Sound::getEnabled() {
#ifdef USING_AL
    return SoundAL::getEnabled();
#else
    return false;
#endif
}

void Sound::setVolume(float vol) {
#ifdef USING_AL
    SoundAL::setVolume(vol);
#endif
}

float Sound::getVolume() {
#ifdef USING_AL
    return SoundAL::getVolume();
#else
    return 0.0f;
#endif
}

