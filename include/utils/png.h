/*!
 * \file include/utils/png.h
 * \brief PNG image reader
 *
 * \author xythobuz
 */

#ifndef _UTILS_PNG_H_
#define _UTILS_PNG_H_

/*!
 * \brief Load a PNG image file into an RGBA buffer
 * \param filename path of file to read
 * \param image place where allocated buffer of size (width * height * 4) will be allocated
 * \param width place where image width will be stored
 * \param height place where image height will be stored
 * \returns 0 on success
 */
int pngLoad(const char *filename, unsigned char **image, unsigned int *width, unsigned int *height);

/*!
 * \brief Create a PNG image file from an RGBA buffer
 * \param filename path of file to create
 * \param image buffer of size (width * height * 4)
 * \param width image width
 * \param height image height
 * \returns 0 on success
 */
int pngSave(const char *filename, unsigned char *image, unsigned int width, unsigned int height);

#endif

