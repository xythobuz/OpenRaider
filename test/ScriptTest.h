/*!
 * \file test/ScriptTest.h
 * \brief Game script loader unit test payload
 *
 * \author xythobuz
 */

#ifndef _SCRIPT_TEST_H
#define _SCRIPT_TEST_H

#include "ScriptPayload.h"

static const unsigned int testPayloadCount = 4; // 4 gzipped files in data block

static const char *testDescription[testPayloadCount] = {
    "TR2 PC", "TR2 PSX", "TR3 PC", "TR3 PSX"
};

static const unsigned char *testPayloads[testPayloadCount] = {
    tr2_pc_dat_z, tr2_psx_dat_z, tr3_pc_dat_z, tr3_psx_dat_z
};

static const unsigned int testSizes[testPayloadCount] = {
    sizeof(tr2_pc_dat_z), sizeof(tr2_psx_dat_z), sizeof(tr3_pc_dat_z), sizeof(tr3_psx_dat_z)
};

static const unsigned int testExpectedGameStringCount[testPayloadCount] = {
    89, 89, 92, 92
};

static const unsigned int testExpectedPlatformStringCount[testPayloadCount] = {
    41, 80, 41, 80
};

#endif

