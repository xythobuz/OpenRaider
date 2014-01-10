/*!
 * \file test/Sound.cpp
 * \brief This is the audio manager Unit Test
 *
 * \author Mongoose
 * \author xythobuz
 */
#include <stdio.h>
#include <unistd.h>

#include <Sound.h>

#include "greatest.h"

#define TESTFILE "data/sample.wav"

Sound sound;
bool inited = false;

GREATEST_MAIN_DEFS();

TEST initSound() {
    int ret;
    if ((ret = sound.init()) != 0) {
        printf("Couldn't initialize Sound (%d)!", ret);
        FAIL();
    }
    inited = true;
    PASS();
}

TEST playFile() {
    int id, ret;
    ASSERT_EQm("Sound not initialized!", inited, true);
    if ((ret = sound.addFile(TESTFILE, &id, sound.SoundFlagsNone)) != 0) {
        printf("Couldn't add file (%d)!", ret);
        FAIL();
    }
    sound.play(id);
    sleep(2);
    PASS();
}

TEST loadFile() {
    int id, ret;
    unsigned int l;
    unsigned char *buf;
    FILE *f;
    ASSERT_EQm("Sound not initialized!", inited, true);
    if (!(f = fopen(TESTFILE, "rb"))) {
        printf("Couldn't open %s!", TESTFILE);
        FAIL();
    }
    fseek(f, 0, SEEK_END);
	l = ftell(f);
	fseek(f, 0, SEEK_SET);
	buf = new unsigned char[l];
	fread(buf, l, 1, f);
	fclose(f);
	if ((ret = sound.addWave(buf, l, &id, sound.SoundFlagsNone)) != 0) {
        printf("Couldn't add buffer (%d)!", ret);
        FAIL();
    }
	sound.play(id);
	delete [] buf;
	sleep(2);
    PASS();
}

SUITE(soundSuite) {
    RUN_TEST(initSound);
    RUN_TEST(playFile);
    RUN_TEST(loadFile);
}

int main(int argc, char *argv[]) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(soundSuite);
    GREATEST_MAIN_END();
}

