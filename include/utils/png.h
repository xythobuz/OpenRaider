/*!
 * \file include/utils/png.h
 * \brief PNG image reader
 *
 * \author xythobuz
 */

#ifndef _UTILS_PNG_H_
#define _UTILS_PNG_H_

#include "TextureManager.h"

/*!
 * \brief Check if a file is a valid PNG image
 * \param filename path of file to read
 * \returns 0 on success
 */
int pngCheck(const char *filename);

/*!
 * \brief Load a PNG image file into an RGBA buffer
 * \param filename path of file to read
 * \param image place where allocated buffer of size (width * height * 4) will be allocated
 * \param width place where image width will be stored
 * \param height place where image height will be stored
 * \param mode place where Color Mode of image will be stored
 * \param bpp place where pixel width will be stored (8, 24, 32)
 * \returns 0 on success
 */
int pngLoad(const char *filename, unsigned char **image, unsigned int *width, unsigned int *height, TextureManager::ColorMode *mode, unsigned int *bpp);

/*!
 * \brief Create a PNG image file from an RGBA buffer
 * \param filename path of file to create
 * \param image buffer of size (width * height * 4)
 * \param width image width
 * \param height image height
 * \param mode color mode
 * \param bpp bits per pixel (8, 24, 32)
 * \returns 0 on success
 */
int pngSave(const char *filename, unsigned char *image, unsigned int width, unsigned int height, TextureManager::ColorMode mode, unsigned int bpp);

#endif

