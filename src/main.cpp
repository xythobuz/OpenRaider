/*!
 * \file src/main.cpp
 * \brief Where main() is
 *
 * \author xythobuz
 */

#include <stdlib.h>
#include <stdio.h>

#include "config.h"

void cleanupHandler() {

}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // Use default rc file path
    } else if (argc == 2) {
        // Use provided rc file
    } else {
        printf("Usage:\n%s [/path/to/config.file]\n", argv[0]);
        return 1;
    }

    atexit(cleanupHandler);

    return 0;
}
