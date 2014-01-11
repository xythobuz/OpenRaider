/*!
 * \file test/mtk_tga.cpp
 * \brief The TGA reader Unit Test
 *
 * \author xythobuz
 */
#include <stdio.h>
#include <stdlib.h>
#include <mtk_tga.h>
#include "greatest.h"

//! \todo generate list?
const char *testFiles[] = {
    "data/particle.tga",
    "data/snow.tga",
    "data/snow2.tga",
    "data/splash.tga",
    "data/white.tga"
};

TEST checkFile(FILE *f) {
    ASSERTm("File wasn't opened.", f != NULL);
    ASSERT_FALSEm("File is invalid?!", mtk_image__tga_check(f));
    PASS();
}

TEST loadFile(FILE *f) {
    unsigned char *image;
    unsigned int width, height;
    char type;
    ASSERTm("File wasn't opened.", f != NULL);
    ASSERT_FALSEm("File couldn't be loaded!", mtk_image__tga_load(f, &image, &width, &height, &type));
    printf("\nWidth: %u\nHeight: %u\nType: %d\n", width, height, type);
    PASS();
}

SUITE(tgaSuite) {
    for (int i = 0; i < (sizeof(testFiles) / sizeof(testFiles[0])); i++) {
        FILE *f = fopen(testFiles[i], "r");
        RUN_TESTp(checkFile, f);
        RUN_TESTp(loadFile, f);
        fclose(f);
    }
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[]) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(tgaSuite);
    GREATEST_MAIN_END();
}

