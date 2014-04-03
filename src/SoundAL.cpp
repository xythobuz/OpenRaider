/*!
 * \file src/SoundAL.cpp
 * \brief This is the OpenAL audio manager Implementation
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <AL/alut.h>

#include <cstdio>
#include <cstdlib>
#include <assert.h>

#include "math/math.h"
#include "SoundAL.h"

SoundAL::SoundAL() {
    mEnabled = false;
    mInit = false;
    mVolume = 1.0f;
}

SoundAL::~SoundAL() {
    if (mInit)
        alutExit();
}

int SoundAL::initialize() {
    assert(mInit == false);

    if (!mEnabled)
        return 0;

#ifndef __APPLE__
    int fd = open("/dev/dsp", O_RDWR);
    if (fd < 0) {
        printf("Could not initialize OpenAL (/dev/dsp)\n");
        return -1;
    }
    close(fd);
#endif

    ALCdevice *Device = alcOpenDevice("OSS");
    ALCcontext *Context = alcCreateContext(Device, NULL);
    alcMakeContextCurrent(Context);

    if (alutInitWithoutContext(NULL, NULL) == AL_FALSE) {
        printf("ALUT-Error: %s\n", alutGetErrorString(alutGetError()));
        return -2;
    }

    mInit = true;

    return 0;
}

void SoundAL::setEnabled(bool on) {
    mEnabled = on;
}

void SoundAL::setVolume(float vol) {
    assert(mSource.size() == mBuffer.size());

    if (mEnabled) {
        if ((mSource.size() > 0) && (!equalEpsilon(mVolume, vol))) {
            // Apply new volume to old sources if needed
            for (size_t i = 0; i < mSource.size(); i++)
                alSourcef(mSource[i], AL_GAIN, vol);
        }
    }

    mVolume = vol;
}

int SoundAL::registeredSources() {
    assert(mSource.size() == mBuffer.size());

    if ((!mEnabled) || (!mInit))
        return 0;

    return mSource.size();
}

void SoundAL::clear() {
    assert(mSource.size() == mBuffer.size());

    if ((!mEnabled) || (!mInit))
        return;

    for (size_t i = 0; i < mSource.size(); i++) {
        alDeleteSources(1, &mSource[i]);
        alDeleteBuffers(1, &mBuffer[i]);
    }

    mSource.clear();
    mBuffer.clear();
}

void SoundAL::listenAt(float pos[3], float angle[3]) {
    assert(mSource.size() == mBuffer.size());
    assert(pos != NULL);
    assert(angle != NULL);

    if ((!mEnabled) || (!mInit))
        return;

    alListenerfv(AL_POSITION, pos);
    alListenerfv(AL_ORIENTATION, angle);
}

void SoundAL::sourceAt(int source, float pos[3]) {
    assert(mSource.size() == mBuffer.size());
    assert(source >= 0);
    assert(source < (int)mSource.size());
    assert(pos != NULL);

    if ((!mEnabled) || (!mInit))
        return;

    alSourcefv(mSource[source], AL_POSITION, pos);
}

//! \fixme Seperate sourcing and buffering, Mongoose 2002.01.04
int SoundAL::addFile(const char *filename, int *source, unsigned int flags) {
    ALsizei size;
    ALfloat freq;
    ALenum format;
    ALvoid *data;
    int id;

    assert(mSource.size() == mBuffer.size());
    assert(filename != NULL);
    assert(filename[0] != '\0');
    assert(source != NULL);

    if ((!mEnabled) || (!mInit)) {
        *source = 0;
        return 0;
    }

    *source = -1;
    id = mSource.size();

    mSource.push_back(0);
    mBuffer.push_back(0);

    alGetError();
    alGenBuffers(1, &mBuffer[id]);
    if (alGetError() != AL_NO_ERROR) {
        printf("Could not load wav file (alGenBuffers)\n");
        return -1;
    }

    alGetError();
    alGenSources(1, &mSource[id]);
    if (alGetError() != AL_NO_ERROR) {
        printf("Could not load wav file (alGenSources)\n");
        return -2;
    }

    data = alutLoadMemoryFromFile(filename, &format, &size, &freq);
    if (alutGetError() != ALUT_ERROR_NO_ERROR) {
        printf("Could not load %s\n", filename);
        return -3;
    }

    alBufferData(mBuffer[id], format, data, size, static_cast<ALsizei>(freq));
    alSourcei(mSource[id], AL_BUFFER, mBuffer[id]);

    if (flags & SoundFlagsLoop) {
        alSourcei(mSource[id], AL_LOOPING, 1);
    }

    alSourcef(mSource[id], AL_GAIN, mVolume);

    *source = id;

    return 0;
}

int SoundAL::addWave(unsigned char *wav, unsigned int length, int *source, unsigned int flags) {
    ALsizei size;
    ALfloat freq;
    ALenum format;
    ALvoid *data;
    int error = 0;
    int id;

    assert(mSource.size() == mBuffer.size());
    assert(wav != NULL);
    assert(source != NULL);

    if ((!mEnabled) || (!mInit)) {
        *source = 0;
        return 0;
    }

    *source = -1;
    id = mSource.size();

    mSource.push_back(0);
    mBuffer.push_back(0);

    data = wav;

    alGetError();
    alGenBuffers(1, &mBuffer[id]);
    if (alGetError() != AL_NO_ERROR) {
        printf("Could not load wav (alGenBuffers)\n");
        return -1;
    }

    alGetError();
    alGenSources(1, &mSource[id]);
    if (alGetError() != AL_NO_ERROR) {
        printf("Could not load wav (alGenSources)\n");
        return -2;
    }

    data = alutLoadMemoryFromFileImage(wav, length, &format, &size, &freq);
    if (((error = alutGetError()) != ALUT_ERROR_NO_ERROR) || (data == NULL)) {
        printf("Could not load wav buffer (%s)\n", alutGetErrorString(error));
        return -3;
    }

    alBufferData(mBuffer[id], format, data, size, static_cast<ALsizei>(freq));
    alSourcei(mSource[id], AL_BUFFER, mBuffer[id]);

    if (flags & SoundFlagsLoop) {
        alSourcei(mSource[id], AL_LOOPING, 1);
    }

    alSourcef(mSource[id], AL_GAIN, mVolume);

    *source = id;

    //! \fixme Should free alut buffer?

    return 0;
}

void SoundAL::play(int source) {
    assert(mSource.size() == mBuffer.size());
    assert(source >= 0);
    assert(source < (int)mSource.size());

    if ((!mEnabled) || (!mInit))
        return;

    alSourcePlay(mSource[source]);
}

void SoundAL::stop(int source) {
    assert(mSource.size() == mBuffer.size());
    assert(source >= 0);
    assert(source < (int)mSource.size());

    if ((!mEnabled) || (!mInit))
        return;

    alSourceStop(mSource[source]);
}

