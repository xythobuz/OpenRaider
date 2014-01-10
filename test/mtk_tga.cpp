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

#define TESTFILE "data/font-0.tga"
FILE *f = NULL;

TEST checkFile() {
    ASSERTm("File wasn't opened. Please run the suite!", f != NULL);
    ASSERT_FALSEm("File is invalid?!", mtk_image__tga_check(f));
    PASS();
}

TEST loadFile() {
    unsigned char *image;
    unsigned int width, height;
    char type;
    ASSERTm("File wasn't opened. Please run the suite!", f != NULL);
    ASSERT_FALSEm("File couldn't be loaded!", mtk_image__tga_load(f, &image, &width, &height, &type));
    printf("\nWidth: %u\nHeight: %u\nType: %d\n", width, height, type);
    PASS();
}

SUITE(tgaSuite) {
    f = fopen(TESTFILE, "r");
    RUN_TEST(checkFile);
    RUN_TEST(loadFile);
    fclose(f);
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[]) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(tgaSuite);
    GREATEST_MAIN_END();
}

