/*!
 * \file src/deps/stb/stb.cpp
 * \brief stb implementation
 *
 * \author xythobuz
 */

#include "global.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) assert(x)
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

