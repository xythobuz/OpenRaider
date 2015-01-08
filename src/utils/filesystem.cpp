/*!
 * \file src/utils/filesystem.cpp
 * \brief file-system utilities
 *
 * \author xythobuz
 */

#include "global.h"
#include "utils/filesystem.h"

#if defined(HAVE_UNISTD_H) && defined(HAVE_GETCWD)
#include <unistd.h>
#endif

#if defined(HAVE_STDLIB_H) && defined(HAVE_GETENV)
#include <stdlib.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

std::string getCurrentWorkingDirectory() {
#if defined(HAVE_UNISTD_H) && defined(HAVE_GETCWD)

    char path[1024];
    assertEqual(getcwd(path, 1024), path);
    return std::string(path);

#else

    assert(false);
    return "";

#endif
}

std::string getHomeDirectory() {
#if defined(HAVE_STDLIB_H) && defined(HAVE_GETENV)

    char* path = getenv("HOME");
    assert(path != nullptr);
    return path;

#elif defined(_WIN32)

    char path[MAX_PATH];
    assertEqual(SHGetFolderPath(nullptr, CSIDL_PROFILE, nullptr, 0, path), S_OK);
    size_t lenPath = strlen(path);
    for (unsigned int i = 0; i < lenPath; i++)
        if (dir[i] == '\\')
            dir[i] = '/';
    return std::string(path);

#else

    assert(false);
    return "";

#endif
}

