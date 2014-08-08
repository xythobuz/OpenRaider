/*!
 * \file include/utils/FileSystem.h
 * \brief Recursive file-system walking utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_FOLDER_H_
#define _UTILS_FOLDER_H_

#include "Exception.h"
#include "utils/File.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class Folder {
public:
    Folder(std::string folder, bool listDotFiles = false);

    std::string &getName();
    std::string &getPath();

    unsigned long fileCount();
    File &getFile(unsigned long i);

    unsigned long folderCount();
    Folder &getFolder(unsigned long i);

    unsigned long countRecursiveItems();
    void executeRemoveRecursiveItems(std::function<bool (File &f)> func);
    std::string getRecursiveItemName(unsigned long i);
    //File &getRecursiveItem(unsigned long i);

private:

    void createFolderItems();
    int readFolderItems(std::vector<std::string> &foundFiles, std::vector<std::string> &foundFolders);

    std::string name; //!< Only last part of path
    std::string path; //!< Full path, with name and '/' at end

    bool hasListed;
    bool listDot;

    std::vector<File> files;
    std::vector<Folder> folders;
};

#endif

