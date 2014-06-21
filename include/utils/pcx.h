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

/*!
 * \brief Load a PCX image file into a RGBA buffer
 * \param filename path of file to read
 * \param image place where allocated buffer of size (width * height * 4) will be allocated
 * \param width place where image width will be stored
 * \param height place where image height will be stored
 * \returns 0 on success
 */
int pcxLoad(const char *filename, unsigned char **image, unsigned int *width, unsigned int *height);

#endif

