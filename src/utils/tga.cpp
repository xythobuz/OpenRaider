/*!
 * \file src/utils/tga.cpp
 * \brief TGA image reader/writer
 *
 * \todo type should pass more info (2 bits for RGBA|RGB|GREY; val for depth)
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "global.h"
#include "utils/tga.h"

typedef enum {
    //TGA_TYPE__NO_DATA    = 0,
    //TGA_TYPE__MAPPED     = 1,
    TGA_TYPE__COLOR      = 2,
    TGA_TYPE__GREYSCALE  = 3,
    //TGA_TYPE__MAPPED_RLE = 9,
    TGA_TYPE__COLOR_RLE  = 10
} tga_type_t;

typedef struct {
    unsigned char comment_lenght;   //!< Number of bytes in comment
    unsigned char colormap_type;    //!< 0 No colormap; 1 Has colormap
    unsigned char image_type;       //!< 1 Colormapped; 2 Unmapped; 9 Colormapped RLE; 10 Unmapped RLE
    unsigned short colormap_index;  //!< Index of first color map entry
    unsigned short colormap_lenght; //!< Number of color map entries
    unsigned char colormap_bbp;     //!< 16, 24, or 32 bits per pixel format
    unsigned short origin_x;        //!< X coor of lower-left corner
    unsigned short origin_y;        //!< Y coor of lower-left corner
    unsigned short width;           //!< Width in pixels
    unsigned short height;          //!< Height in pixels
    unsigned char bpp;              //!< Number of bits in a pixel index
    unsigned char desc_flags;       //!< Various magic bits
} tga_t;

int tgaCheck(const char* filename) {
    char buffer[10];

    assert(filename != nullptr);
    assert(filename[0] != '\0');

    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("tgaCheck> File not found\n");
        return -1;
    }

    /* Read the header */
    fseek(f, 0, SEEK_SET);
    fread(buffer, 8, 1, f);

    // buffer[1] = 0 - Means not color mapped (1 would mean mapped)
    if (!(buffer[1] == 0 && (buffer[2] == TGA_TYPE__COLOR ||
                             //buffer[2] == TGA_TYPE__GREYSCALE ||
                             buffer[2] == TGA_TYPE__COLOR_RLE))) {
        printf("tgaCheck> Inavlid or unknown TGA format.\n");
        fclose(f);
        return -2;
    }

    fclose(f);
    return 0;
}

int tgaLoad(const char* filename, unsigned char** image, unsigned int* width, unsigned int* height,
            char* type) {
    tga_t header;
    char comment[256];
    unsigned char pixel[4];
    unsigned char* swap_row = nullptr;
    unsigned char tmp, packet;
    bool must_flip = 0;
    unsigned int size;
    unsigned int i, j;

    assert(filename != nullptr);
    assert(filename[0] != '\0');
    assert(image != nullptr);
    assert(width != nullptr);
    assert(height != nullptr);
    assert(type != nullptr);

    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("tgaLoad> File not found\n");
        return -1;
    }

    fseek(f, 0, SEEK_SET);

    // Read TGA header
    fread(&header.comment_lenght, 1, 1, f);
    fread(&header.colormap_type, 1, 1, f);
    fread(&header.image_type, 1, 1, f);
    fread(&header.colormap_index, 2, 1, f);
    fread(&header.colormap_lenght, 2, 1, f);
    fread(&header.colormap_bbp, 1, 1, f);

    fread(&tmp, 1, 1, f);
    header.origin_x = tmp;
    fread(&tmp, 1, 1, f);
    header.origin_x += tmp * 256;

    fread(&tmp, 1, 1, f);
    header.origin_y = tmp;
    fread(&tmp, 1, 1, f);
    header.origin_y += tmp * 256;

    fread(&tmp, 1, 1, f);
    header.width = tmp;
    fread(&tmp, 1, 1, f);
    header.width += tmp * 256;

    fread(&tmp, 1, 1, f);
    header.height = tmp;
    fread(&tmp, 1, 1, f);
    header.height += tmp * 256;

    fread(&header.bpp, 1, 1, f);
    fread(&header.desc_flags, 1, 1, f);

    *width = header.width;
    *height = header.height;

    switch (header.bpp) {
        case 32:
            *type = 2; //32;
            break;
        case 24:
            *type = 1; //24;
            break;
        case 8:
        default:
            *type = 0; //8;
            break;
    }

#ifdef DEBUG_TGA
    printf("TGA [%ix%i@%ibpp, %it, %ix, %iy, %uf]\n",
           header.width, header.height, header.bpp, header.image_type,
           header.origin_x, header.origin_y,
           header.desc_flags);
#endif

    // Comments can be 0 - 255
    if (header.comment_lenght) {
        fread(&comment, 1, header.comment_lenght, f);
        for (i = 0; i < 255; ++i) {
            if (!(comment[i] > 32 && comment[i] < 127))
                comment[i] = '.'; // print a dot for invalid text
        }
        comment[255] = 0;
        printf("Comment: '%s'\n", comment);
    }

    *image = nullptr;
    size = header.width * header.height;

    if (!size || (!(header.colormap_type == 0 && (header.image_type == 2
                    || header.image_type == 10)))) {
        fprintf(stderr, "tgaLoad> Unknown image format.\n");
        fclose(f);
        return -2;
    }

    // Mongoose: Added 'screen origin bit' support back here
    if (!(header.desc_flags & 32)) {
        must_flip = true;
    }

    switch (header.bpp) {
        case 32:
            size *= 4;
            *image = new unsigned char [size];
            switch (header.image_type) {
                case TGA_TYPE__COLOR_RLE:
                    for (i = 0; i < size;) {
                        fread(&packet, 1, 1, f);
                        if (packet & 0x80) { // Run Length
                            packet = (packet & 0x7F) + 1;
                            fread(&pixel, 4, 1, f);
                            for (j = 0; j < packet; j++) {
                                (*image)[i++] = pixel[2];
                                (*image)[i++] = pixel[1];
                                (*image)[i++] = pixel[0];
                                (*image)[i++] = pixel[3];
                            }
                        } else { // RAW
                            packet = (packet & 0x7F) + 1;
                            for (j = 0; j < packet; j++) {
                                fread(&pixel, 4, 1, f);
                                (*image)[i++] = pixel[2];
                                (*image)[i++] = pixel[1];
                                (*image)[i++] = pixel[0];
                                (*image)[i++] = pixel[3];
                            }
                        }
                    }
                    break;
                case TGA_TYPE__COLOR:
                    if (fread((*image), size, 1, f) < 1) {
                        fprintf(stderr, "tgaLoad> Image fread failed.\n");
                        delete [] *image;
                        fclose(f);
                        return -4;
                    }
                    for (i = 0; i < size; i += 4) {
                        tmp = (*image)[i];
                        (*image)[i] = (*image)[i + 2];
                        (*image)[i + 2] = tmp;
                    }
                    break;
                default:
                    ;
            }
            if (must_flip) {
                swap_row = new unsigned char [header.width * 4];
                for (i = 0, j = header.height - 1; (int)i < header.height / 2; i++, j--) {
                    memcpy(swap_row, &(*image)[i * header.width * 4], header.width * 4);
                    memcpy(&(*image)[i * header.width * 4], &(*image)[j * header.width * 4],
                           header.width * 4);
                    memcpy(&(*image)[j * header.width * 4], swap_row, header.width * 4);
                }
                delete [] swap_row;
            }
            break;
        case 24:
            size *= 3;
            *image = new unsigned char [size];
            switch (header.image_type) {
                case TGA_TYPE__COLOR_RLE:
                    for (i = 0; i < size;) {
                        fread(&packet, 1, 1, f);
                        if (packet & 0x80) { // Run Length
                            packet = (packet & 0x7F) + 1;
                            fread(&pixel, 3, 1, f);
                            for (j = 0; j < packet; j++) {
                                (*image)[i++] = pixel[2];
                                (*image)[i++] = pixel[1];
                                (*image)[i++] = pixel[0];
                            }
                        } else { // RAW
                            packet = (packet & 0x7F) + 1;
                            for (j = 0; j < packet; j++) {
                                fread(&pixel, 3, 1, f);
                                (*image)[i++] = pixel[2];
                                (*image)[i++] = pixel[1];
                                (*image)[i++] = pixel[0];
                            }
                        }
                    }
                    break;
                case TGA_TYPE__COLOR:
                    if (fread((*image), size, 1, f) < 1) {
                        fprintf(stderr, "tgaLoad> Image fread failed.\n");
                        delete [] *image;
                        fclose(f);
                        return -4;
                    }
                    for (i = 0; i < size; i += 3) {
                        tmp = (*image)[i];
                        (*image)[i] = (*image)[i + 2];
                        (*image)[i + 2] = tmp;
                    }
                    break;
                default:
                    ;
            }
            if (must_flip) {
                swap_row = new unsigned char [header.width * 3];
                for (i = 0, j = header.height - 1; (int)i < header.height / 2; i++, j--) {
                    memcpy(swap_row, &(*image)[i * header.width * 3], header.width * 3);
                    memcpy(&(*image)[i * header.width * 3], &(*image)[j * header.width * 3],
                           header.width * 3);
                    memcpy(&(*image)[j * header.width * 3], swap_row, header.width * 3);
                }
                delete [] swap_row;
            }
            break;
        case 8:
            printf("tgaLoad> 8bpp Not implemented\n");
            break;
        default:
            ;
    }

#ifdef DEBUG_TGA
    char c;
    printf("TGA Comment: ");
    while (fread(&c, 1, 1, f) == 1) {
        printf("%c", c);
    }
    printf("\n");
#endif

    fclose(f);
    return 0;
}

int tgaSave(const char* filename, unsigned char* image, unsigned int width, unsigned int height,
            char type) {
    tga_t header;
    unsigned int size;
    char comment[64];
    //unsigned int i;
    //unsigned char tmp;

    assert(filename != nullptr);
    assert(filename[0] != '\0');
    assert(image != nullptr);
    assert(width > 0);
    assert(height > 0);

    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("tgaSave> File not found\n");
        return -1;
    }

    strncpy(comment, "OpenRaider TGA", 63);
    comment[63] = 0;

    header.comment_lenght = (unsigned char)strlen(comment);
    header.colormap_type = 0;

    // No colormaps
    header.colormap_index = 0;
    header.colormap_lenght = 0;
    header.colormap_bbp = 0;

    header.origin_x = header.origin_y = 0;
    header.width = (unsigned short)width;
    header.height = (unsigned short)height;

    header.desc_flags = 0;

    switch (type) {
        case 4:
            header.image_type = TGA_TYPE__COLOR;
            header.desc_flags = 32;
            header.bpp = 32;
            break;
        case 2:
            header.bpp = 32;
            break;
        case 1:
            header.image_type = TGA_TYPE__GREYSCALE;
            header.bpp = 8;
            break;
        default:
            header.image_type = TGA_TYPE__COLOR;
            header.bpp = 24;
    }

    // Write TGA header
    fwrite(&header.comment_lenght, 1, 1, f);
    fwrite(&header.colormap_type, 1, 1, f);
    fwrite(&header.image_type, 1, 1, f);
    fwrite(&header.colormap_index, 2, 1, f);
    fwrite(&header.colormap_lenght, 2, 1, f);
    fwrite(&header.colormap_bbp, 1, 1, f);
    fwrite(&header.origin_x, 2, 1, f);
    fwrite(&header.origin_y, 2, 1, f);
    fwrite(&header.width, 2, 1, f);
    fwrite(&header.height, 2, 1, f);
    fwrite(&header.bpp, 1, 1, f);
    fwrite(&header.desc_flags, 1, 1, f);

    // Write comment
    fwrite(&comment, 1, header.comment_lenght, f);

    switch (header.bpp) {
        case 32:
            size = header.width * header.height * 4;

            //for (i = 0; i < size; i += 4)
            //{
            //  tmp = image[i];
            //  image[i] = image[i + 2];
            //  image[i + 2] = tmp;
            //}
            break;
        case 24:
            size = header.width * header.height * 3;

            //for (i = 0; i < size; i += 3)
            //{
            //  tmp = image[i];
            //  image[i] = image[i + 2];
            //  image[i + 2] = tmp;
            //}
            break;
        case 8:
        default:
            size = header.width * header.height;
            break;
    }

    // Write image data
    if (fwrite(image, size, 1, f) < 1) {
        perror("tgaSave> Disk write failed.\n");
        fclose(f);
        return -2;
    }

    fclose(f);
    return 0;
}

