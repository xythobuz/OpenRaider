/*!
 * \file include/tga.h
 * \brief TGA image reader/writer
 *
 * \author Mongoose
 * \author xythobuz
 */
#ifndef _TGA_H
#define _TGA_H

#include <stdio.h>

/*!
 * \brief Possible TGA image types
 */
typedef enum {
    TGA_TYPE__NO_DATA    = 0,
    TGA_TYPE__MAPPED     = 1,
    TGA_TYPE__COLOR      = 2,
    TGA_TYPE__GREYSCALE  = 3,
    TGA_TYPE__MAPPED_RLE = 9,
    TGA_TYPE__COLOR_RLE  = 10
    // TGA_TYPE__GREYSCALE_COMPRESSED  = 11,
    // TGA_TYPE__COLOR_HUFFMAN_DELTA_RLE  = 32,
    // TGA_TYPE__COLOR_HUFFMAN_DELTA_RLE_4PASS  = 33
} tga_type_t;

/*!
 * \brief Data structure representing TGA file header
 */
typedef struct {
    unsigned char comment_lenght;       //!< Number of bytes in comment
    unsigned char colormap_type;        //!< 0 No colormap; 1 Has colormap
    unsigned char image_type;           //!< 1 Colormapped; 2 Unmapped; 9 Colormapped RLE; 10 Unmapped RLE
    unsigned short colormap_index;      //!< Index of first color map entry
    unsigned short colormap_lenght;     //!< Number of color map entries
    unsigned char colormap_bbp;         //!< 16, 24, or 32 bits per pixel format
    unsigned short origin_x;            //!< X coor of lower-left corner
    unsigned short origin_y;            //!< Y coor of lower-left corner
    unsigned short width;               //!< Width in pixels
    unsigned short height;              //!< Height in pixels
    unsigned char bpp;                  //!< Number of bits in a pixel index
    unsigned char desc_flags;           //!< Various magic bits
} tga_t;

/*!
 * \brief Check if a file is a valid TGA image
 * \param f file to be checked
 * \returns 0 if valid, else error condition
 */
int tga_check(FILE *f);

/*!
 * \brief Load a TGA image from file
 * \param f valid image file
 * \param image Where the pixmap will be stored (or NULL)
 * \param width where the width will be stored
 * \param height where the height will be stored
 * \param type where the type will be stored (tga_type_t)
 * \returns 0 on success, else error condition
 */
int tga_load(FILE *f, unsigned char **image,
        unsigned int *width, unsigned int *height, char *type);

/*!
 * \brief Save a pixel buffer into a file on disk
 * \param f file in which image will be saved
 * \param image pixmap to be stored
 * \param width width of pixmap/image
 * \param height height of pixmap/image
 * \param type tga_type_t to use
 * \returns 0 on success, else error condition
 */
int tga_save(FILE *f, unsigned char *image,
        unsigned int width, unsigned int height, char type);

/*!
 * \brief Save a pixel buffer into a file on disk
 * \param image pixmap to be stored
 * \param width width of pixmap/image
 * \param height height of pixmap/image
 * \param type tga_type_t to use
 * \param s format string for file path/name
 * \returns 0 on success, else error condition
 */
__attribute__((format(printf, 5, 6)))
int tga_save_filename(unsigned char *image,
        unsigned int width, unsigned int height,
        char type,
        char *s, ...);

#endif
