/*!
 * \file src/utils/File.cpp
 * \brief Recursive file-system walking utilities
 *
 * \author xythobuz
 */

#include <algorithm>

#include "global.h"
#include "utils/File.h"
#include "utils/Folder.h"

File::File(std::string file) : path(file) {
    size_t pos = file.rfind('/', file.length() - 2);
    name = file.substr(pos + 1);
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
}

std::string &File::getName() {
    return name;
}

std::string &File::getPath() {
    return path;
}

