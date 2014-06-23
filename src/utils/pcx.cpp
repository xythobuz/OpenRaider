/*!
 * \file src/utils/pcx.cpp
 * \brief PCX image reader
 *
 * Based on official technical documentation from ZSoft:
 * http://bespin.org/~qz/pc-gpe/pcx.txt
 *
 * \author xythobuz
 */

#include <fstream>

#include "global.h"
#include "utils/pcx.h"

#include "Console.h"
#define pcxPrint getConsole().print

int pcxCheck(const char *filename) {
    assert(filename != NULL);
    assert(filename[0] != '\0');

    std::ifstream file(filename, std::ios::in | std::ios::binary);

    // Read raw PCX header, 128 bytes
    unsigned char *header = new unsigned char[128];

    // Basic validation
    if (!file.read((char *)(&header[0]), 128)) {
        pcxPrint("File not big enough for valid PCX header!");
        delete [] header;
        return -1;
    }

    if (header[0] != 0x0A) {
        pcxPrint("Magic number at file start is wrong (0x%X != 0x0A)", header[0]);
        delete [] header;
        return -2;
    }

    if ((header[1] != 0) && ((header[1] < 2) || (header[1] > 5))) {
        // Valid: 0, 2, 3, 4, 5
        pcxPrint("Unknown PCX file format version (%d)", header[1]);
        delete [] header;
        return -3;
    }

    if ((header[2] != 0) && (header[2] != 1)) {
        pcxPrint("Unknown PCX file encoding (%d)", header[2]);
        delete [] header;
        return -4;
    }

    if (header[3] != 8) {
        pcxPrint("Only supporting 8bit (%dbit)", header[3]);
        delete [] header;
        return -5;
    }

    if (header[64] != 0) {
        pcxPrint("Reserved field is  used (%d != 0)", header[64]);
        delete [] header;
        return -6;
    }

    delete [] header;
    return 0;
}

int pcxLoad(const char *filename, unsigned char **image, unsigned int *width, unsigned int *height, TextureManager::ColorMode *mode, unsigned int *bpp) {
    assert(filename != NULL);
    assert(filename[0] != '\0');
    assert(image != NULL);
    assert(width != NULL);
    assert(height != NULL);
    assert(mode != NULL);
    assert(bpp != NULL);

    std::ifstream file(filename, std::ios::in | std::ios::binary);

    // Read raw PCX header, 128 bytes
    unsigned char *header = new unsigned char[128];

    // Basic validation
    if (!file.read((char *)(&header[0]), 128)) {
        pcxPrint("File not big enough for valid PCX header!");
        delete [] header;
        return -1;
    }

    if (header[0] != 0x0A) {
        pcxPrint("Magic number at file start is wrong (0x%X != 0x0A)", header[0]);
        delete [] header;
        return -2;
    }

    if ((header[1] != 0) && ((header[1] < 2) || (header[1] > 5))) {
        // Valid: 0, 2, 3, 4, 5
        pcxPrint("Unknown PCX file format version (%d)", header[1]);
        delete [] header;
        return -3;
    }

    if ((header[2] != 0) && (header[2] != 1)) {
        pcxPrint("Unknown PCX file encoding (%d)", header[2]);
        delete [] header;
        return -4;
    }

    if (header[3] != 8) {
        pcxPrint("Only supporting 8bit (%dbit)", header[3]);
        delete [] header;
        return -5;
    }

    if (header[64] != 0) {
        pcxPrint("Reserved field is  used (%d != 0)", header[64]);
        delete [] header;
        return -6;
    }

    // Read header informations
    bool versionFive = (header[1] == 5);
    bool compressed = (header[2] == 1);
    //unsigned char bitsPerPixel = header[3];
    unsigned int xMin = header[4] | (header[5] << 8);
    unsigned int yMin = header[6] | (header[7] << 8);
    unsigned int xMax = header[8] | (header[9] << 8);
    unsigned int yMax = header[10] | (header[11] << 8);
    //unsigned int hDPI = header[12] | (header[13] << 8);
    //unsigned int vDPI = header[14] | (header[15] << 8);
    //unsigned char *colormap = header + 16;
    unsigned char nPlanes = header[65];
    unsigned int bytesPerLine = header[66] | (header[67] << 8);
    //unsigned int paletteInfo = header[68] | (header[69] << 8);
    //unsigned int hScreenSize = header[70] | (header[71] << 8); // Only in some versionFive files
    //unsigned int vScreenSize = header[72] | (header[73] << 8); // Only in some versionFive files

    delete [] header;

    // Calculations
    *width = xMax - xMin + 1;
    *height = yMax - yMin + 1;
    unsigned long totalBytes = nPlanes * bytesPerLine; // total bytes per scan line
    unsigned long imageSize = totalBytes * *height;
    unsigned char *buffer = new unsigned char[imageSize];
    unsigned long b = 0;

    // Read encoded pixel data
    for (unsigned long i = 0; i < imageSize;) {
        unsigned int n = 1; // Run-length-encoding assumes 1
        int c = file.get();
        if (!file) {
            pcxPrint("Could not read data (%lu%s)", i, (file.eof() ? " EOF" : ""));
            delete [] buffer;
            return -7;
        }

        // Run-Length-Encoding
        if (compressed) {
            if ((c & 0xC0) == 0xC0) {
                n = c & 0x3F;
                c = file.get();
                if (!file) {
                    pcxPrint("Could not read data rle (%lu%s)", i, (file.eof() ? " EOF" : ""));
                    delete [] buffer;
                    return -8;
                }
            }
        }

        for (unsigned int j = 0; j < n; j++)
            buffer[b++] = (unsigned char)c;

        i += n;
    }

    // Read color palette
    unsigned char *palette = NULL;
    if (versionFive) {
        int c = file.get();
        if ((c == 12) && file) {
            palette = new unsigned char[768];
            for (unsigned int i = 0; i < 768; i++) {
                palette[i] = (unsigned char)file.get();
                if (!file) {
                    pcxPrint("Could not read 256 color palette (%d)", i);
                    delete [] buffer;
                    delete [] palette;
                    return -9;
                }
            }
        }
    }

    // Bring buffer into preferred format
    unsigned long size = *width * *height * 4;
    *image = new unsigned char[size];
    for (unsigned int y = 0; y < *height; y++) {
        for (unsigned int x = 0; x < *width; x++) {
            unsigned long baseIndex = (x + (y * *width)) * 4;
            unsigned char alpha = 255, red = 0, green = 0, blue = 0;

            if (palette != NULL) {
                if (nPlanes == 1) {
                    red = palette[buffer[(y * totalBytes) + x] * 3];
                    green = palette[(buffer[(y * totalBytes) + x] * 3) + 1];
                    blue = palette[(buffer[(y * totalBytes) + x] * 3) + 2];
                } else {
                    pcxPrint("Unsupported number of planes (%d)", nPlanes);
                    delete [] buffer;
                    delete [] palette;
                    delete [] *image;
                    *image = NULL;
                    return -10;
                }
            } else {
                if ((nPlanes == 3) || (nPlanes == 4)) {
                    red = buffer[(y * totalBytes) + x];
                    green = buffer[(y * totalBytes) + *width + x];
                    blue = buffer[(y * totalBytes) + (2 * *width) + x];
                    if (nPlanes == 4)
                        alpha = buffer[(y * totalBytes) + (3 * *width) + x];
                } else if (nPlanes == 1) {
                    red = green = blue = buffer[(y * totalBytes) + x];
                } else {
                    pcxPrint("Unsupported number of planes (%d)", nPlanes);
                    delete [] buffer;
                    delete [] palette;
                    delete [] *image;
                    *image = NULL;
                    return -11;
                }
            }

            (*image)[baseIndex + 0] = red;
            (*image)[baseIndex + 1] = green;
            (*image)[baseIndex + 2] = blue;
            (*image)[baseIndex + 3] = alpha;
        }
    }

    *mode = TextureManager::RGBA;
    *bpp = 32;

    delete [] buffer;
    delete [] palette;
    return 0;
}

