/*!
 * \file src/SoundNull.cpp
 * \brief This is the null audio manager Implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "SoundNull.h"

SoundNull::SoundNull() {
}

SoundNull::~SoundNull() {
}

int SoundNull::initialize() {
    return 0;
}

void SoundNull::setEnabled(bool on) {
}

void SoundNull::setVolume(float vol) {
}

int SoundNull::registeredSources() {
    return 0;
}

void SoundNull::clear() {
}

void SoundNull::listenAt(float pos[3], float angle[3]) {
}

void SoundNull::sourceAt(int source, float pos[3]) {
}

int SoundNull::addFile(const char *filename, int *source, unsigned int flags) {
    return 0;
}

int SoundNull::addWave(unsigned char *wav, unsigned int length, int *source, unsigned int flags) {
    return 0;
}

void SoundNull::play(int source) {
}

void SoundNull::stop(int source) {
}

