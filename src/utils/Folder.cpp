/*!
 * \file src/utils/Folder.cpp
 * \brief Recursive file-system walking utilities
 *
 * \author xythobuz
 */

#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstring>

#include "global.h"
#include "utils/filesystem.h"
#include "utils/File.h"
#include "utils/Folder.h"

#if defined(HAVE_DIRENT_H) && defined(HAVE_OPENDIR) && defined(HAVE_READDIR_R) && defined(HAVE_CLOSEDIR) && defined(HAVE_DT_DIR)
#include <dirent.h>
#define USE_DIRENT
#elif defined (_WIN32)
#include <windows.h>
#define USE_FINDFILE
#else
#error No support for recursive folder traversal
#endif

Folder::Folder(std::string folder, bool listDotFiles) {
    if (((folder.length() == 0) || (folder.compare(".") == 0))
#ifdef _WIN32
            || ((folder.compare(1, 2, std::string(":\\")) != 0) && (folder.at(0) != '~'))
#else
            || ((folder.at(0) != '/') && (folder.at(0) != '~'))
#endif
            ) {
        // Prepend current working directory
        path = getCurrentWorkingDirectory();
        if (folder.length() > 1)
            path += folder.substr(1);
    } else if (folder.at(0) == '~') {
        // Prepend home directory
        path = getHomeDirectory();
        std::cout << "Home: " << path << std::endl;
        if (folder.length() > 1)
            path += folder.substr(1);
    } else {
        path = folder;
    }

    // Find all '\\' replace with '/'
    size_t pos = path.find('\\');
    while (pos != std::string::npos) {
        path.at(pos) = '/';
        pos = path.find('\\');
    }

    size_t last = path.rfind('/', path.length() - 2);
    name = path.substr(last + 1);
    if (name.back() == '/')
        name.erase(name.length() - 1);

    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    if (path.back() != '/')
        path.append(1, '/');

    hasListed = false;
    listDot = listDotFiles;
}

std::string &Folder::getName() {
    return name;
}

std::string &Folder::getPath() {
    return path;
}

unsigned long Folder::fileCount() {
    createFolderItems();
    return files.size();
}

File &Folder::getFile(unsigned long i) {
    createFolderItems();
    assert(i < files.size());
    return files.at(i);
}

unsigned long Folder::folderCount() {
    createFolderItems();
    return folders.size();
}

Folder &Folder::getFolder(unsigned long i) {
    createFolderItems();
    assert(i < folders.size());
    return folders.at(i);
}

unsigned long Folder::countRecursiveItems() {
    unsigned long count = fileCount();
    for (unsigned long i = 0; i < folderCount(); i++)
        count += getFolder(i).countRecursiveItems();
    return count;
}

void Folder::executeRemoveRecursiveItems(std::function<bool (File &f)> func) {
    for (unsigned long i = 0; i < fileCount(); i++) {
        if (func(getFile(i))) {
            files.erase(files.begin() + (long)i--);
        }
    }

    for (unsigned long i = 0; i < folderCount(); i++) {
        getFolder(i).executeRemoveRecursiveItems(func);
    }
}

std::string Folder::getRecursiveItemName(unsigned long i) {
    assert(i < countRecursiveItems());
    if (i < fileCount()) {
        return getFile(i).getName();
    } else {
        unsigned long count = fileCount();
        for (unsigned long n = 0; n < folderCount(); n++) {
            if ((i - count) < getFolder(n).countRecursiveItems()) {
                return getFolder(n).getName() + '/'
                    + getFolder(n).getRecursiveItemName(i - count);
            }
            count += getFolder(n).countRecursiveItems();
        }
    }

    assert(false);
    return "";
}

/*
File &Folder::getRecursiveItem(unsigned long i) {
    assert(i < countRecursiveItems());
    if (i < fileCount()) {
        return getFile(i);
    } else {
        unsigned long count = fileCount();
        for (unsigned long n = 0; n < folderCount(); n++) {
            if ((i - count) < getFolder(n).countRecursiveItems()) {
                return getFolder(n).getRecursiveItem(i - count);
            }
            count += getFolder(n).countRecursiveItems();
        }
    }

    assert(false);
    return files.at(0);
}
*/

void Folder::createFolderItems() {
    if (hasListed)
        return;

    std::vector<std::string> foundFiles, foundFolders;
    if (readFolderItems(foundFiles, foundFolders) != 0)
        throw Exception(std::string("Could not open folder ") + name);

    if (!listDot) {
        std::vector<std::string>::iterator it = foundFiles.begin();
        while (it != foundFiles.end()) {
            size_t pos = it->rfind('/', it->length() - 2);
            if (it->at(pos + 1) == '.')
                it = foundFiles.erase(it);
            else
                ++it;
        }

        it = foundFolders.begin();
        while (it != foundFolders.end()) {
            size_t pos = it->rfind('/', it->length() - 2);
            if (it->at(pos + 1) == '.')
                it = foundFolders.erase(it);
            else
                ++it;
        }
    }

    for (unsigned long i = 0; i < foundFiles.size(); i++)
        files.emplace_back(File(foundFiles.at(i)));

    for (unsigned long i = 0; i < foundFolders.size(); i++)
        folders.emplace_back(Folder(foundFolders.at(i)));

    hasListed = true;
}

#ifdef USE_DIRENT

int Folder::readFolderItems(std::vector<std::string> &foundFiles, std::vector<std::string> &foundFolders) {
    struct dirent entry;
    struct dirent *ep = nullptr;
    DIR *pakDir;

    pakDir = opendir(path.c_str());
    if (pakDir != nullptr) {
        readdir_r(pakDir, &entry, &ep);
        while (ep != nullptr) {
            if ((strcmp(".", ep->d_name) != 0)
                     && (strcmp("..", ep->d_name) != 0)) {
                std::string tmp(path);
                if (tmp.back() != '/')
                    tmp += '/';
                tmp += ep->d_name;
                if (ep->d_type == DT_DIR) {
                    if (tmp.back() != '/')
                        tmp += '/';
                    foundFolders.push_back(std::string(tmp));
                } else {
                    foundFiles.push_back(std::string(tmp));
                }
            }
            readdir_r(pakDir, &entry, &ep);
        }
        closedir(pakDir);
    } else {
        return 1;
    }

    return 0;
}

#elif defined(USE_FINDFILE)

int Folder::readFolderItems(std::vector<std::string> &foundFiles, std::vector<std::string> &foundFolders) {
    std::string tmp(path);
    tmp += "/*";

    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(tmp.c_str(), &fd);
    if (hFind == nullptr)
        return 1;

    do {
        std::string s(path);
        s = s + "/" + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            foundFolders.push_back(s);
        else
            foundFiles.push_back(s);
    } while (FindNextFile(hFind, &fd) != 0);

    FindClose(hFind);
    return 0;
}

#endif

