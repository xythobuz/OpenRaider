/*!
 * \file include/main.h
 * \brief Where main() is
 *
 * \author xythobuz
 */
#ifndef _MAIN_H_
#define _MAIN_H_

/*!
 * \brief atexit() handler
 */
void cleanupHandler(void);

/*!
 * \brief Program entry point
 * \param argc number of arguments
 * \param argv array with argc strings
 * \returns 0 on success
 */
int main(int argc, char *argv[]);

#endif

