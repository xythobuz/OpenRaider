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
 * \param f file to be checked
 * \returns 0 if valid, else error condition
 */
int tgaCheck(FILE *f);

/*!
 * \brief Load a TGA image from file
 * \param f valid image file
 * \param image Where the pixmap will be stored (or NULL)
 * \param width where the width will be stored
 * \param height where the height will be stored
 * \param type where the type will be stored (tga_type_t)
 * \returns 0 on success, else error condition
 */
int tgaLoad(FILE *f, unsigned char **image,
                unsigned int *width, unsigned int *height, char *type);

/*!
 * \brief Save a pixel buffer into a file on disk
 * \param f file in which image will be saved
 * \param image pixmap to be stored
 * \param width width of pixmap/image
 * \param height height of pixmap/image
 * \param type tga type to use
 * \returns 0 on success, else error condition
 */
int tgaSave(FILE *f, unsigned char *image,
                unsigned int width, unsigned int height, char type);

#endif

