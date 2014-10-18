/*!
 * \file src/utils/Folder.cpp
 * \brief Recursive file-system walking utilities
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/File.h"
#include "utils/Folder.h"

unsigned long Folder::countRecursiveFiles() {
    createFolderItems();
    unsigned long count = fileCount();
    for (unsigned long i = 0; i < folderCount(); i++)
        count += getFolder(i).countRecursiveFiles();
    return count;
}

void Folder::executeRemoveRecursiveFiles(std::function<bool (File& f)> func) {
    executeRemoveFiles(func);
    for (unsigned long i = 0; i < folderCount(); i++) {
        getFolder(i).executeRemoveRecursiveFiles(func);
    }
}

std::string Folder::getRecursiveFileName(unsigned long i) {
    createFolderItems();
    assert(i < countRecursiveFiles());
    if (i < fileCount()) {
        return getFile(i).getName();
    } else {
        unsigned long count = fileCount();
        for (unsigned long n = 0; n < folderCount(); n++) {
            if ((i - count) < getFolder(n).countRecursiveFiles()) {
                return getFolder(n).getName() + '/'
                       + getFolder(n).getRecursiveFileName(i - count);
            }
            count += getFolder(n).countRecursiveFiles();
        }
    }

    assert(false);
    return "";
}

File& Folder::getRecursiveFile(unsigned long i) {
    createFolderItems();
    assert(i < countRecursiveFiles());
    if (i < fileCount()) {
        return getFile(i);
    } else {
        unsigned long count = fileCount();
        for (unsigned long n = 0; n < folderCount(); n++) {
            if ((i - count) < getFolder(n).countRecursiveFiles()) {
                return getFolder(n).getRecursiveFile(i - count);
            }
            count += getFolder(n).countRecursiveFiles();
        }
    }

    assert(false);
    return files.at(0);
}

