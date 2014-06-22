/*!
 * \file src/utils/png.cpp
 * \brief PNG image reader
 *
 * https://github.com/DavidEGrayson/ahrs-visualizer/blob/master/png_texture.cpp
 * http://zarb.org/~gc/html/libpng.html
 *
 * \author xythobuz
 */

#include <png.h>
#include <cstdio>

#include "global.h"
#include "utils/png.h"

#ifdef DEBUG
#include "Console.h"
#define pngPrint getConsole().print
#else
void pngPrint(...) { }
#endif

int pngLoad(const char *filename, unsigned char **image, unsigned int *width, unsigned int *height) {
    png_byte header[8];

    assert(filename != NULL);
    assert(filename[0] != '\0');
    assert(image != NULL);
    assert(width != NULL);
    assert(height != NULL);

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        pngPrint("Could not open %s", filename);
        return -1;
    }

    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8)) {
        pngPrint("File %s is not a PNG.", filename);
        fclose(fp);
        return -2;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        pngPrint("png_create_read_struct returned 0.");
        fclose(fp);
        return -3;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        pngPrint("png_create_info_struct returned 0.");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return -4;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        pngPrint("png_create_info_struct returned 0.");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return -5;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        pngPrint("Error from libpng");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return -6;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    int bit_depth, color_type;

    png_get_IHDR(png_ptr, info_ptr, width, height, &bit_depth, &color_type,
            NULL, NULL, NULL);

    if (bit_depth != 8) {
        pngPrint("%s: Unsupported bit depth %d.  Must be 8.", filename, bit_depth);
        return -7;
    }

    png_read_update_info(png_ptr, info_ptr);

    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    png_byte *image_data = new png_byte[(rowbytes * *height) + 15];
    png_bytep *row_pointers = new png_bytep[*height];

    for (unsigned int i = 0; i < *height; i++)
        row_pointers[*height - 1 - i] = image_data + i * rowbytes;

    png_read_image(png_ptr, row_pointers);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    delete [] row_pointers;
    fclose(fp);

    // Fix alpha
    if (color_type != PNG_COLOR_TYPE_RGB_ALPHA) {
        unsigned char *tmpimage = image_data;
        image_data = new unsigned char[*width * *height * 4];
        if (color_type == PNG_COLOR_TYPE_RGB) {
            for (unsigned int i = 0; i < (*width * *height); i++) {
                image_data[i * 4] = tmpimage[i * 3];
                image_data[(i * 4) + 1] = tmpimage[(i * 3) + 1];
                image_data[(i * 4) + 2] = tmpimage[(i * 3) + 2];
                image_data[(i * 4) + 3] = 255;
            }
        } else {
            pngPrint("%s: Unknown libpng color type %d.", filename, color_type);
            delete [] image_data;
            delete [] tmpimage;
            return -8;
        }
        delete [] tmpimage;
    }

    // Flip
    *image = new unsigned char[*width * *height * 4];
    for (unsigned int y = 0; y < (*height); y++) {
        for (unsigned int x = 0; x < (*width); x++) {
            (*image)[((y * *width) + x) * 4] = image_data[(((*height - y - 1) * *width) + x) * 4];
            (*image)[(((y * *width) + x) * 4) + 1] = image_data[((((*height - y - 1) * *width) + x) * 4) + 1];
            (*image)[(((y * *width) + x) * 4) + 2] = image_data[((((*height - y - 1) * *width) + x) * 4) + 2];
            (*image)[(((y * *width) + x) * 4) + 3] = image_data[((((*height - y - 1) * *width) + x) * 4) + 3];
        }
    }

    delete [] image_data;
    return 0;
}

int pngSave(const char *filename, unsigned char *image, unsigned int width, unsigned int height) {
    assert(filename != NULL);
    assert(filename[0] != '\0');
    assert(image != NULL);
    assert(width > 0);
    assert(height > 0);

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        pngPrint("File %s could not be opened for writing", filename);
        return -1;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
        pngPrint("png_create_write_struct failed");
        fclose(fp);
        return -2;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        pngPrint("png_create_info_struct failed");
        fclose(fp);
        return -3;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        pngPrint("Error during init_io");
        fclose(fp);
        return -4;
    }

    png_init_io(png_ptr, fp);

    if (setjmp(png_jmpbuf(png_ptr))) {
        pngPrint("Error during writing header");
        fclose(fp);
        return -5;
    }

    png_set_IHDR(png_ptr, info_ptr, width, height,
            8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    png_bytep *row_pointers = new png_bytep[height];

    for (unsigned int i = 0; i < height; i++)
        row_pointers[height - 1 - i] = image + (i * width * 4);

    if (setjmp(png_jmpbuf(png_ptr))) {
        pngPrint("Error during writing bytes");
        delete [] row_pointers;
        fclose(fp);
        return -6;
    }

    png_write_image(png_ptr, row_pointers);

    if (setjmp(png_jmpbuf(png_ptr))) {
        pngPrint("Error during end of write");
        delete [] row_pointers;
        fclose(fp);
        return -7;
    }

    png_write_end(png_ptr, NULL);

    delete [] row_pointers;
    fclose(fp);

    return 0;
}

