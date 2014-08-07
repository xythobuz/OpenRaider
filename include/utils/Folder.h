/*!
 * \file include/utils/FileSystem.h
 * \brief Recursive file-system walking utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_FOLDER_H_
#define _UTILS_FOLDER_H_

#include <memory>
#include <string>
#include <vector>

class File;

class Folder {
public:
    Folder(std::string folder);
    ~Folder();

private:
    std::string name;
    std::string path;
    std::vector<std::shared_ptr<File>> files;
};

#endif

