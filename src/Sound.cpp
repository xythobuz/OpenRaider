/*!
 * \file src/Sound.cpp
 * \brief This is the audio manager Implementation
 *
 * \author xythobuz
 */

#include "global.h"
#include "Sound.h"

#ifdef USING_AL
#include "SoundAL.h"
#else
#include "SoundNull.h"
#endif

Sound &getSound() {
#ifdef USING_AL
    static SoundAL gSound;
#else
    static SoundNull gSound;
#endif
    return gSound;
}

Sound::~Sound() {
}

