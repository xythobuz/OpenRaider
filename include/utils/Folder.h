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

    std::string& getName();
    std::string& getPath();

    unsigned long fileCount();
    File& getFile(unsigned long i);

    unsigned long folderCount();
    Folder& getFolder(unsigned long i);

    Folder getParent();

    void executeRemoveFiles(std::function<bool (File& f)> func);

    // Accessing a folder recursively
    // This treats all files in all subfolders as if they were in this folder
    unsigned long countRecursiveFiles();
    void executeRemoveRecursiveFiles(std::function<bool (File& f)> func);
    std::string getRecursiveFileName(unsigned long i);
    File& getRecursiveFile(unsigned long i);

  private:

    void createFolderItems();
    int readFolderItems(std::vector<std::string>& foundFiles, std::vector<std::string>& foundFolders);

    std::string name; //!< Only last part of path
    std::string path; //!< Full path, with name and '/' at end

    bool hasListed;
    bool listDot;

    std::vector<File> files;
    std::vector<Folder> folders;
};

#endif

