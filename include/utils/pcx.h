/*!
 * \file include/utils/pcx.h
 * \brief PCX image reader
 *
 * Based on official technical documentation from ZSoft:
 * http://bespin.org/~qz/pc-gpe/pcx.txt
 *
 * \author xythobuz
 */

#ifndef _UTILS_PCX_H_
#define _UTILS_PCX_H_

// Returns 0 on success
int pcxLoad(const char *filename, unsigned char **image, unsigned int *width, unsigned int *height);

#endif

