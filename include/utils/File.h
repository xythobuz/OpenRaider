/*!
 * \file include/utils/File.h
 * \brief Recursive file-system walking utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_FILE_H_
#define _UTILS_FILE_H_

#include <string>

class Folder;

class File {
public:
    File(std::string file);

    std::string &getName();
    std::string &getPath();

private:
    std::string name;
    std::string path;
};

#endif

