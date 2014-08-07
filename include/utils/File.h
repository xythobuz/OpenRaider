/*!
 * \file include/utils/File.h
 * \brief Recursive file-system walking utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_FILE_H_
#define _UTILS_FILE_H_

#include <memory>
#include <string>

class Folder;

class File {
public:
    File(std::string file);
    ~File();

    std::shared_ptr<Folder> getParent();

private:
    std::string name;
    std::string path;
    std::weak_ptr<Folder> parent;
};

#endif

