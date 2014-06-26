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
 * \brief Check if a file is a valid PCX image
 * \param filename path of file to read
 * \returns 0 on success
 */
int pcxCheck(const char *filename);

/*!
 * \brief Load a PCX image file into a buffer
 * \param filename path of file to read
 * \param image place where allocated buffer of size (width * height * (bpp / 8)) will be allocated
 * \param width place where image width will be stored
 * \param height place where image height will be stored
 * \param mode place where Color Mode of image will be stored
 * \param bpp place where pixel width will be stored (8, 24, 32)
 * \returns 0 on success
 */
int pcxLoad(const char *filename, unsigned char **image,
        unsigned int *width, unsigned int *height, ColorMode *mode, unsigned int *bpp);

#endif

