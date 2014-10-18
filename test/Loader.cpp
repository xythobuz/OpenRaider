/*!
 * \file test/Loader.cpp
 * \brief Level Loader Unit Test
 *
 * \author xythobuz
 */

#include "global.h"
#include "loader/Loader.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\t" << argv[0] << " /path/to/level.tr2" << std::endl;
        return 0; // Don't fail when just running this test...
    }

    // Print file engine version
    std::cout << "Loading \"" << argv[1] << "\"" << std::endl;
    std::cout << "Trying to detect engine version... ";
    Loader::LoaderVersion v = Loader::checkFile(argv[1]);
    switch (v) {
        case Loader::TR_1:
            std::cout << "TR 1";
            break;

        case Loader::TR_2:
            std::cout << "TR 2";
            break;

        case Loader::TR_3:
            std::cout << "TR 3";
            break;

        case Loader::TR_4:
            std::cout << "TR 4";
            break;

        case Loader::TR_5:
            std::cout << "TR 5";
            break;

        case Loader::TR_UNKNOWN:
            std::cout << "Unknown version!" << std::endl;
            return 2;
    }
    std::cout << " detected!" << std::endl;

    // Create matching loader
    std::cout << std::endl << "Creating level loader... ";
    std::unique_ptr<Loader> loader = Loader::createLoader(argv[1]);
    if (!loader) {
        std::cout << "Failed!" << std::endl;
        return 3;
    } else {
        std::cout << "Success!" << std::endl;
    }

    // Try to load the level
    std::cout << "Trying to load level... ";
    int error = loader->load(argv[1]);
    if (error != 0) {
        std::cout << "Failure!" << std::endl;
        std::cout << "Error code: " << error << std::endl;
    } else {
        std::cout << "Success!" << std::endl;
    }

    return error;
}

