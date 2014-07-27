/*!
 * \file include/utils/binary.h
 * \brief Binary file reading utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_BINARY_H_
#define _UTILS_BINARY_H_

void binOpen(const char *file);
void binClose();

unsigned long binTell();
void binSeek(unsigned long pos);

int8_t binRead8();
uint8_t binReadU8();

int16_t binRead16();
uint16_t binReadU16();

int32_t binRead32();
uint32_t binReadU32();

int64_t binRead64();
uint64_t binReadU64();

#endif

