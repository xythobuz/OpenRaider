/*!
 * \file src/utils/strings.cpp
 * \brief String handling utilities
 *
 * \author xythobuz
 */

#include <algorithm>

#include "global.h"
#include "utils/filesystem.h"
#include "utils/strings.h"

std::string findAndReplace(std::string s, std::string find, std::string replace) {
    size_t p = s.find(find);
    while (p != std::string::npos) {
        s.erase(p, find.length());
        s.insert(p, replace);
        p = s.find(find);
    }
    return s;
}

std::string expandHomeDirectory(std::string s) {
    if ((s.length() > 0) && (s[0] == '~')) {
        s.erase(0, 1);
        s.insert(0, getHomeDirectory());
    }
    return s;
}

bool stringEndsWith(std::string s, std::string suffix, bool casesensitive) {
    if (!casesensitive) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
    }

    if (s.length() >= suffix.length()) {
        std::string end = s.substr(s.length() - suffix.length());
        return (end == suffix);
    }

    return false;
}

