/*!
 * \file include/utils/strings.h
 * \brief String handling utilities
 *
 * \author xythobuz
 */

#ifndef _UTILS_STRINGS_H_
#define _UTILS_STRINGS_H_

#include <string>

std::string findAndReplace(std::string s, std::string find, std::string replace);

std::string expandHomeDirectory(std::string s);

bool stringEndsWith(std::string s, std::string suffix, bool casesensitive = false);

std::string removeLastPathElement(std::string s);

std::string getLastPathElement(std::string s);

std::string convertPathDelimiter(std::string s);

#endif

