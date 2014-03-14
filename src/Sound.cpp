/*!
 * \file src/Sound.cpp
 * \brief This is the audio manager Implementation
 *
 * \author xythobuz
 * \author Mongoose
 */

#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#include <AL/alut.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "utils/math.h"
#include "Sound.h"

Sound::Sound() {
    mInit = false;
    mVolume = 1.0f;
}

Sound::~Sound() {
    if (mInit)
        alutExit();
}

int Sound::init() {
    assert(mInit == false);

#ifndef __APPLE__
    int fd;

    fd = open("/dev/dsp", O_RDWR);

    if (fd < 0) {
        perror("Sound::Init> Could not open /dev/dsp : ");
        return -1;
    }

    close(fd);
#endif

    ALCdevice *Device = alcOpenDevice("OSS");
    ALCcontext *Context = alcCreateContext(Device, NULL);
    alcMakeContextCurrent(Context);

    if (alutInitWithoutContext(NULL, NULL) == AL_FALSE) {
        printf("Sound::Init> Could not initialize alut (%s)\n", alutGetErrorString(alutGetError()));
        return -2;
    }

    mInit = true;
    printf("Created OpenAL Context\n");

    return 0;
}

int Sound::registeredSources() {
    assert(mInit == true);
    assert(mSource.size() == mBuffer.size());

    return mSource.size();
}

void Sound::clear() {
    assert(mInit == true);
    assert(mSource.size() == mBuffer.size());

    for (size_t i = 0; i < mSource.size(); i++) {
        alDeleteSources(1, &mSource[i]);
        alDeleteBuffers(1, &mBuffer[i]);
    }

    mSource.clear();
    mBuffer.clear();
}

void Sound::setVolume(float vol) {
    assert(mInit == true);
    assert(mSource.size() == mBuffer.size());

    if ((mSource.size() > 0) && (!equalEpsilon(mVolume, vol))) {
        // Apply new volume to old sources if needed
        for (size_t i = 0; i < mSource.size(); i++)
            alSourcef(mSource[i], AL_GAIN, vol);
    }

    mVolume = vol;
}

void Sound::listenAt(float pos[3], float angle[3]) {
    assert(mInit == true);
    assert(mSource.size() == mBuffer.size());
    assert(pos != NULL);
    assert(angle != NULL);

    alListenerfv(AL_POSITION, pos);
    alListenerfv(AL_ORIENTATION, angle);
}

void Sound::sourceAt(int source, float pos[3]) {
    assert(mInit == true);
    assert(mSource.size() == mBuffer.size());
    assert(source >= 0);
    assert(source < (int)mSource.size());
    assert(pos != NULL);

    alSourcefv(mSource[source], AL_POSITION, pos);
}

//! \fixme Seperate sourcing and buffering, Mongoose 2002.01.04
int Sound::addFile(const char *filename, int *source, unsigned int flags)
{
    ALsizei size;
    ALfloat freq;
    ALenum format;
    ALvoid *data;
    int id;

    assert(mInit == true);
    assert(mSource.size() == mBuffer.size());
    assert(filename != NULL);
    assert(filename[0] != '\0');
    assert(source != NULL);

    *source = -1;
    id = mSource.size();

    alGetError();
    alGenBuffers(1, &mBuffer[id]);
    if (alGetError() != AL_NO_ERROR) {
        fprintf(stderr, "Sound::AddFile> alGenBuffers call failed\n");
        return -1;
    }

    alGetError();
    alGenSources(1, &mSource[id]);
    if (alGetError() != AL_NO_ERROR) {
        fprintf(stderr, "Sound::AddFile> alGenSources call failed\n");
        return -2;
    }

    // err = alutLoadWAV(filename, &data, &format, &size, &bits, &freq);
    // is deprecated!
    data = alutLoadMemoryFromFile(filename, &format, &size, &freq);
    if (alutGetError() != ALUT_ERROR_NO_ERROR) {
        fprintf(stderr, "Could not load %s\n", filename);
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

int Sound::addWave(unsigned char *wav, unsigned int length, int *source, unsigned int flags) {
    ALsizei size;
    ALfloat freq;
    ALenum format;
    ALvoid *data;
    int error = 0;
    int id;

    assert(mInit == true);
    assert(mSource.size() == mBuffer.size());
    assert(wav != NULL);
    assert(source != NULL);

    *source = -1;
    id = mSource.size();

    data = wav;

    alGetError();
    alGenBuffers(1, &mBuffer[id]);
    if (alGetError() != AL_NO_ERROR) {
        fprintf(stderr, "Sound::AddWave> alGenBuffers call failed\n");
        return -1;
    }

    alGetError();
    alGenSources(1, &mSource[id]);
    if (alGetError() != AL_NO_ERROR) {
        fprintf(stderr, "Sound::AddWave> alGenSources call failed\n");
        return -2;
    }

    //AL_FORMAT_WAVE_EXT does not exist on Mac!"
    // alBufferData(mBuffer[id], AL_FORMAT_WAVE_EXT, data, size, freq);
    // Idea: Fill Buffer with
    // alutLoadMemoryFromFileImage
    //     (const ALvoid *data, ALsizei length, ALenum *format, ALsizei *size, ALfloat *frequency)

    data = alutLoadMemoryFromFileImage(wav, length, &format, &size, &freq);
    if (((error = alutGetError()) != ALUT_ERROR_NO_ERROR) || (data == NULL)) {
        fprintf(stderr, "Could not load wav buffer (%s)\n", alutGetErrorString(error));
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

void Sound::play(int source) {
    assert(mInit == true);
    assert(mSource.size() == mBuffer.size());
    assert(source >= 0);
    assert(source < (int)mSource.size());

    alSourcePlay(mSource[source]);
}

void Sound::stop(int source) {
    assert(mInit == true);
    assert(mSource.size() == mBuffer.size());
    assert(source >= 0);
    assert(source < (int)mSource.size());

    alSourceStop(mSource[source]);
}

