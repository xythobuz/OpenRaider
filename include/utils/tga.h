/*!
 * \file include/utils/tga.h
 * \brief TGA image reader/writer
 *
 * \author Mongoose
 * \author xythobuz
 */
#ifndef _UTILS_TGA_H
#define _UTILS_TGA_H

/*!
 * \brief Check if a file is a valid TGA image
 * \param filename path to file
 * \returns 0 if valid, else error condition
 */
int tgaCheck(const char *filename);

/*!
 * \brief Load a TGA image from file
 * \param filename path to file
 * \param image Where the pixmap will be stored (or NULL)
 * \param width where the width will be stored
 * \param height where the height will be stored
 * \param type where the type will be stored (tga_type_t)
 * \returns 0 on success, else error condition
 */
int tgaLoad(const char *filename, unsigned char **image,
                unsigned int *width, unsigned int *height, char *type);

/*!
 * \brief Save a pixel buffer into a file on disk
 * \param filename path to file
 * \param image pixmap to be stored
 * \param width width of pixmap/image
 * \param height height of pixmap/image
 * \param type tga type to use
 * \returns 0 on success, else error condition
 */
int tgaSave(const char *filename, unsigned char *image,
                unsigned int width, unsigned int height, char type);

#endif

