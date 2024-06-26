cmake_minimum_required (VERSION 2.8.8)
project (OpenRaider)

# Version Number
set (OpenRaider_VERSION_MAJOR 0)
set (OpenRaider_VERSION_MINOR 1)
set (OpenRaider_VERSION_MICRO 4)
set (OpenRaider_VERSION_RELEASE -git)

# Build Host
set (OpenRaider_BUILD_HOST ${CMAKE_HOST_SYSTEM})
site_name (OpenRaider_HOSTNAME)

#################################################################

# Set a default build type if none was specified
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message (STATUS "Setting build type to 'Debug' as none was specified.")
    set (CMAKE_BUILD_TYPE Debug)
endif()

#################################################################

# Include directories
include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories (SYSTEM "${PROJECT_SOURCE_DIR}/src/deps")
include_directories ("${PROJECT_BINARY_DIR}")

# Include External Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

#################################################################

# Compiler specific flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (WARNINGS "${WARNINGS} -Weverything")
    set (WARNINGS "${WARNINGS} -Wno-padded -Wno-packed")
    set (WARNINGS "${WARNINGS} -Wno-global-constructors -Wno-exit-time-destructors")
    set (WARNINGS "${WARNINGS} -Wno-documentation-unknown-command -Wno-c++98-compat-pedantic")
    set (WARNINGS "${WARNINGS} -Wno-date-time -Wno-unused-variable -Wno-unused-parameter")
    set (WARNINGS "${WARNINGS} -Wno-shorten-64-to-32 -Wno-sign-compare -Wno-sign-conversion")
    set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -std=c++11")
    set (OpenRaider_CXX_FLAGS_DEBUG "${OpenRaider_CXX_FLAGS_DEBUG} -g -O0")
    set (OpenRaider_CXX_FLAGS_RELEASE "${OpenRaider_CXX_FLAGS_RELEASE} -O2")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set (WARNINGS "${WARNINGS} -Wall -Wextra -Wpedantic")
    set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -std=c++11")
    set (OpenRaider_CXX_FLAGS_DEBUG "${OpenRaider_CXX_FLAGS_DEBUG} -g -O0")
    set (OpenRaider_CXX_FLAGS_RELEASE "${OpenRaider_CXX_FLAGS_RELEASE} -O2")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # TODO Visual C++ compiler flags?
endif()

# Flags for all builds
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} ${WARNINGS} -DGLM_ENABLE_EXPERIMENTAL")

# Flags for Debug build
set (OpenRaider_CXX_FLAGS_DEBUG "${OpenRaider_CXX_FLAGS_DEBUG} -DDEBUG")

# Flags for Releasae builds
set (OpenRaider_CXX_FLAGS_RELEASE "${OpenRaider_CXX_FLAGS_RELEASE} -DNODEBUG")

#################################################################

include (GetGitRevisionDescription)
get_git_head_revision (GIT_REFSPEC GIT_SHA1)
if (NOT ${GIT_SHA1} STREQUAL "GITDIR-NOTFOUND")
    string (SUBSTRING "-${GIT_SHA1}" 0 8 OpenRaider_VERSION_RELEASE)
endif (NOT ${GIT_SHA1} STREQUAL "GITDIR-NOTFOUND")

#################################################################

enable_testing ()

#################################################################

# Add subdirectories
add_subdirectory (src)
add_subdirectory (test)

#################################################################

# CPack packaging
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/COPYING")
set (CPACK_MONOLITHIC_INSTALL YES)
include (CPack)

#################################################################

if (${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")

	# Target for running the setup
    add_custom_target (setup "${PROJECT_SOURCE_DIR}/cmake/setup_win.bat"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

endif (${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles") 

if (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")

    # Target for running the setup
    add_custom_target (setup "${PROJECT_SOURCE_DIR}/cmake/setup.sh"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

    #################################################################

    # Target for running cppcheck
    set (CHECK_STD "--std=c++11" "--std=posix")
    set (CHECK_FLAGS "--quiet" "--force" "-I${PROJECT_SOURCE_DIR}/include")
    set (CHECK_FLAGS ${CHECK_FLAGS} "-i${PROJECT_SOURCE_DIR}/src/deps")
    set (CHECK_NORMAL "--enable=information,warning,performance,portability")
    set (CHECK_FULL "--enable=all")
    set (CHECK_CONFIG "--check-config")
    add_custom_target (cppcheck echo "Running cppcheck..."
        COMMAND cppcheck ${CHECK_FLAGS} ${CHECK_STD} ${CHECK_NORMAL} ${PROJECT_SOURCE_DIR}
    )
    add_custom_target (cppcheckFull echo "Running full cppcheck..."
        COMMAND cppcheck ${CHECK_FLAGS} ${CHECK_STD} ${CHECK_FULL} ${PROJECT_SOURCE_DIR}
    )
    add_custom_target (cppcheckConfig echo "Checking cppcheck config..."
        COMMAND cppcheck ${CHECK_FLAGS} ${CHECK_CONFIG} ${PROJECT_SOURCE_DIR}
    )

    #################################################################

    # Generate Doxygen Documentation
    find_package (Doxygen)
    if (DOXYGEN_FOUND)
        # Configure the Template Doxyfile for our specific project
        configure_file (cmake/Doxyfile.in 
            ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)

        # Add a custom target to run Doxygen when ever the project is built
        add_custom_target (doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
            SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)

        # Add custom target to create local documentation without graphs
        add_custom_target (docLocal
            COMMAND sed -i '' "s/HAVE_DOT               = YES/HAVE_DOT               = NO/g" Doxyfile
            COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
            COMMAND cp -R doc/html/* ${PROJECT_SOURCE_DIR}/../apache/
            COMMAND sed -i '' "s/HAVE_DOT               = NO/HAVE_DOT               = YES/g" Doxyfile
            SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)

        # Add custom target to create local documentation with call and caller graphs
        add_custom_target (docLocalFull
            COMMAND sed -i '' "s/CALL_GRAPH             = NO/CALL_GRAPH             = YES/g" Doxyfile
            COMMAND sed -i '' "s/CALLER_GRAPH           = NO/CALLER_GRAPH           = YES/g" Doxyfile
            COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
            COMMAND cp -R doc/html/* ${PROJECT_SOURCE_DIR}/../apache/
            COMMAND sed -i '' "s/CALL_GRAPH             = YES/CALL_GRAPH             = NO/g" Doxyfile
            COMMAND sed -i '' "s/CALLER_GRAPH           = YES/CALLER_GRAPH           = NO/g" Doxyfile
            SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)

        # Clean doc files
        set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "doc")
    endif (DOXYGEN_FOUND)

    #################################################################

    # Auto-format code
    add_custom_target (format
        COMMAND astyle -A14s4pHUk1OocxC100z2SKNLm1Qxynr "include/*.h"
        COMMAND astyle -A14s4pHUk1OocxC100z2SKNLm1Qxynr "src/*.cpp" --exclude=src/deps
        COMMAND astyle -A14s4pHUk1OocxC100z2SKNLm1Qxynr "test/*.cpp" "test/*.h"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Auto-Formatting code..." VERBATIM
    )

    #################################################################

    # Count source code lines
    add_custom_target (count
        COMMAND cloc --exclude-dir=deps,cmake,data,build ${PROJECT_SOURCE_DIR}
        COMMENT "Counting lines of source code..." VERBATIM
    )

    add_custom_target (countFull
        COMMAND cloc --exclude-dir=build ${PROJECT_SOURCE_DIR}
        COMMENT "Counting lines of source code..." VERBATIM
    )
endif (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")

