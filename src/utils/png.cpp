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
#include <iostream>

#include "global.h"
#include "utils/pixel.h"
#include "utils/png.h"

int pngCheck(const char* filename) {
    png_byte header[8];

    assert(filename != nullptr);
    assert(filename[0] != '\0');

    FILE* fp = fopen(filename, "rb");
    if (fp == nullptr) {
        std::cout << "Could not open " << filename << std::endl;
        return -1;
    }

    fread(header, 1, 8, fp);
    fclose(fp);

    if (png_sig_cmp(header, 0, 8)) {
        std::cout << "File " << filename << " is not a PNG." << std::endl;
        return -2;
    }

    return 0;
}

int pngLoad(const char* filename, unsigned char** image,
            unsigned int* width, unsigned int* height,
            TextureManager::ColorMode* mode, unsigned int* bpp) {
    png_byte header[8];

    assert(filename != nullptr);
    assert(filename[0] != '\0');
    assert(image != nullptr);
    assert(width != nullptr);
    assert(height != nullptr);
    assert(mode != nullptr);
    assert(bpp != nullptr);

    FILE* fp = fopen(filename, "rb");
    if (fp == nullptr) {
        std::cout << "Could not open " << filename << std::endl;
        return -1;
    }

    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8)) {
        std::cout << "File " << filename << " is not a PNG." << std::endl;
        fclose(fp);
        return -2;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        std::cout << "png_create_read_struct returned 0." << std::endl;
        fclose(fp);
        return -3;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        std::cout << "png_create_info_struct returned 0." << std::endl;
        png_destroy_read_struct(&png_ptr, (png_infopp)nullptr, (png_infopp)nullptr);
        fclose(fp);
        return -4;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        std::cout << "png_create_info_struct returned 0." << std::endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) nullptr);
        fclose(fp);
        return -5;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cout << "Error from libpng" << std::endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return -6;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    int bit_depth, color_type;
    png_uint_32 tmpWidth, tmpHeight;

    png_get_IHDR(png_ptr, info_ptr, &tmpWidth, &tmpHeight, &bit_depth, &color_type,
                 nullptr, nullptr, nullptr);

    *width = tmpWidth;
    *height = tmpHeight;

    if (bit_depth != 8) {
        std::cout << filename << ": Unsupported bit depth " << bit_depth << ".  Must be 8." << std::endl;
        return -7;
    }

    png_read_update_info(png_ptr, info_ptr);

    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    png_byte* image_data = new png_byte[(rowbytes** height) + 15];
    png_bytep* row_pointers = new png_bytep[*height];

    for (unsigned int i = 0; i < *height; i++)
        row_pointers[*height - 1 - i] = image_data + i * rowbytes;

    png_read_image(png_ptr, row_pointers);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    delete [] row_pointers;
    fclose(fp);

    if (color_type == PNG_COLOR_TYPE_RGB) {
        *mode = TextureManager::ColorMode::RGB;
        *bpp = 24;
    } else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        *mode = TextureManager::ColorMode::RGBA;
        *bpp = 32;
    } else {
        std::cout << filename << ": Unknown libpng color type " << color_type << std::endl;
        delete [] image_data;
        return -8;
    }

    // Flip
    *image = new unsigned char[*width** height * (*bpp / 8)];
    for (unsigned int y = 0; y < (*height); y++) {
        for (unsigned int x = 0; x < (*width); x++) {
            (*image)[((y** width) + x) * (*bpp / 8)]
                = image_data[(((*height - y - 1)** width) + x) * (*bpp / 8)];
            (*image)[(((y** width) + x) * (*bpp / 8)) + 1]
                = image_data[((((*height - y - 1)** width) + x) * (*bpp / 8)) + 1];
            (*image)[(((y** width) + x) * (*bpp / 8)) + 2]
                = image_data[((((*height - y - 1)** width) + x) * (*bpp / 8)) + 2];
            (*image)[(((y** width) + x) * (*bpp / 8)) + 3]
                = image_data[((((*height - y - 1)** width) + x) * (*bpp / 8)) + 3];
        }
    }

    delete [] image_data;
    return 0;
}

int pngSave(const char* filename, unsigned char* image,
            unsigned int width, unsigned int height,
            TextureManager::ColorMode mode, unsigned int bpp) {
    assert(filename != nullptr);
    assert(filename[0] != '\0');
    assert(image != nullptr);
    assert(width > 0);
    assert(height > 0);

    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        std::cout << "File " << filename << " could not be opened for writing" << std::endl;
        return -1;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!png_ptr) {
        std::cout << "png_create_write_struct failed" << std::endl;
        fclose(fp);
        return -2;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        std::cout << "png_create_info_struct failed" << std::endl;
        fclose(fp);
        return -3;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cout << "Error during init_io" << std::endl;
        fclose(fp);
        return -4;
    }

    png_init_io(png_ptr, fp);

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cout << "Error during writing header" << std::endl;
        fclose(fp);
        return -5;
    }

    int color_type;
    if (((mode == TextureManager::ColorMode::RGB)
         || (mode == TextureManager::ColorMode::BGR)) && (bpp == 24)) {
        if (mode == TextureManager::ColorMode::BGR) {
            bgr2rgb24(image, width, height);
        }
        color_type = PNG_COLOR_TYPE_RGB;
    } else if (((mode == TextureManager::ColorMode::RGBA)
                || (mode == TextureManager::ColorMode::BGRA)) && (bpp == 32)) {
        if (mode == TextureManager::ColorMode::BGRA) {
            bgra2rgba32(image, width, height);
        }
        color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    } else {
        std::cout << "Error invalid color mode" << std::endl;
        fclose(fp);
        return -6;
    }

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    png_bytep* row_pointers = new png_bytep[height];

    for (unsigned int i = 0; i < height; i++)
        row_pointers[height - 1 - i] = image + (i * width * 4);

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cout << "Error during writing bytes" << std::endl;
        delete [] row_pointers;
        fclose(fp);
        return -7;
    }

    png_write_image(png_ptr, row_pointers);

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cout << "Error during end of write" << std::endl;
        delete [] row_pointers;
        fclose(fp);
        return -8;
    }

    png_write_end(png_ptr, nullptr);

    delete [] row_pointers;
    fclose(fp);

    return 0;
}

