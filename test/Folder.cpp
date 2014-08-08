/*!
 * \file test/Folder.cpp
 * \brief File system utils unit test
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
#include "utils/File.h"
#include "utils/Folder.h"

int main() {
    Folder f(".");

    std::cout << f.folderCount() << " folders:" << std::endl;
    for (unsigned long i = 0; i < f.folderCount(); i++)
        std::cout << "\t" << f.getFolder(i).getName() << std::endl;

    std::cout << f.fileCount() << " files:" << std::endl;
    for (unsigned long i = 0; i < f.fileCount(); i++)
        std::cout << "\t" << f.getFile(i).getName() << std::endl;

    return 0;
}

