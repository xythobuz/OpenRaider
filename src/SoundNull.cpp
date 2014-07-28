/*!
 * \file src/SoundNull.cpp
 * \brief This is the null audio manager Implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "SoundNull.h"

SoundNull::SoundNull() {
    sources = 0;
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

unsigned long SoundNull::registeredSources() {
    return sources;
}

void SoundNull::clear() {
    sources = 0;
}

void SoundNull::listenAt(float pos[3], float angle[3]) {
}

void SoundNull::sourceAt(unsigned long source, float pos[3]) {
    assert(source < sources);
}

int SoundNull::addFile(const char *filename, unsigned long *source, unsigned int flags) {
    *source = sources;
    sources++;
    return 0;
}

int SoundNull::addWave(unsigned char *wav, unsigned int length, unsigned long *source, unsigned int flags) {
    *source = sources;
    sources++;
    return 0;
}

void SoundNull::play(unsigned long source) {
    assert(source < sources);
}

void SoundNull::stop(unsigned long source) {
    assert(source < sources);
}

