/*!
 * \file include/utils/strings.h
 * \brief String handling utilities
 *
 * \author xythobuz
 * \author Mongoose
 */

#ifndef _UTILS_STRINGS_H_
#define _UTILS_STRINGS_H_

/*!
 * \brief Check if a string ends with another string.
 * \param str string to check
 * \param suffix suffix for which to check
 * \returns true if str ends with suffix
 */
bool stringEndsWith(const char *str, const char *suffix);

/*!
 * \brief Generates a buffered string for the printf call
 * \param string Format string like for printf
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

/*!
 * \brief Checks if Command matches Symbol.
 * Returns the rest of the argument list back in command buffer, if any
 * \param symbol command string
 * \param command with arguments
 * \returns true if command matches symbol
 */
bool rc_command(const char *symbol, char *command);

/*!
 * \brief Interpret a string as a bool
 * \param buffer "true" or "false"
 * \param val is set to boolean interpretation of buffer
 * \returns -1 for null string, -2 if string is not "true" or "false"
 */
int rc_get_bool(char *buffer, bool *val);

#endif
