/*!
 * \file include/utils/strings.h
 * \brief String handling utilities
 *
 * \author xythobuz
 * \author Mongoose
 */

#ifndef _UTILS_STRINGS_H_
#define _UTILS_STRINGS_H_

#include <cstdarg>

char *stringRemoveQuotes(const char *s);

char *stringReplace(const char *s, const char *search, const char *replace);

int readBool(const char *value, bool *var);

/*!
 * \brief Check if a string ends with another string.
 * \param str string to check
 * \param suffix suffix for which to check
 * \returns true if str ends with suffix
 */
bool stringEndsWith(const char *str, const char *suffix);

/*!
 * \brief Generates a buffered string for the printf call
 * \param string format string like for printf
 * \param args arguments matching format string
 * \returns string in a buffer
 */
char *bufferString(const char *string, va_list args) __attribute__((format(printf, 1, 0)));

/*!
 * \brief Generates a buffered string for the printf call
 * \param string format string like for printf
 * \returns string in a buffer
 */
char *bufferString(const char *string, ...) __attribute__((format(printf, 1, 2)));

/*!
 * \brief Expansion of unix home enviroment char.
 * Also makes sure string ends in "end" char.
 * \param path path string
 * \param end end character. 0 appends no additional char
 * \returns allocated string of path with expansions
 */
char *fullPath(const char *path, char end);

#endif

