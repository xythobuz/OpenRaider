cmake_minimum_required (VERSION 2.8)
project (OpenRaider)

# Version Number
set (OpenRaider_VERSION_MAJOR 0)
set (OpenRaider_VERSION_MINOR 1)
set (OpenRaider_VERSION_MICRO 2)

# Build Host
set (OpenRaider_BUILD_HOST ${CMAKE_HOST_SYSTEM})

# Configuration Header file
configure_file (
    "${PROJECT_SOURCE_DIR}/include/Config.h.in"
    "${PROJECT_BINARY_DIR}/Config.h"
)
include_directories ("${PROJECT_BINARY_DIR}")

# Default to Debug build
set (CMAKE_BUILD_TYPE "Debug")

# Add subdirectories
add_subdirectory (src)
add_subdirectory (test)

#################################################################

if (APPLE)
    # Preparing the bundle on install
    # http://www.cmake.org/Wiki/BundleUtilitiesExample
    set (plugin_dest_dir OpenRaider.app/Contents/MacOS)
    set (APPS "${PROJECT_BINARY_DIR}/src/OpenRaider.app")
    install (CODE "
        file(GLOB_RECURSE PLUGINS
            \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\${PLUGINS}\" \"\")
        " COMPONENT Runtime
    )

    # Copy setup script
    install (PROGRAMS ${PROJECT_SOURCE_DIR}/cmake/setup_mac.sh
        DESTINATION ${PROJECT_BINARY_DIR}/src/OpenRaider.app/Contents/MacOS
        RENAME OpenRaider.sh
    )

    # Fix executable name, so setup script will be called
    install (CODE "
        execute_process (
            COMMAND /usr/libexec/PlistBuddy -c \"Set :CFBundleExecutable OpenRaider.sh\" Info.plist
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/src/OpenRaider.app/Contents
        )
    " COMPONENT Runtime)
endif (APPLE)

#################################################################

# Target for running cppcheck
set (CHECK_STD "--std=c++11" "--std=posix")
set (CHECK_FLAGS "--quiet" "--force")
set (CHECK_NORMAL "--enable=information,warning,performance,portability")
set (CHECK_FULL "--enable=all")
set (CHECK_CONFIG "--check-config")
add_custom_target (check echo "Running cppcheck..."
    COMMAND cppcheck ${CHECK_FLAGS} "-I${PROJECT_SOURCE_DIR}/include" ${CHECK_STD} ${CHECK_NORMAL} ${PROJECT_SOURCE_DIR}
)
add_custom_target (checkFull echo "Running full cppcheck..."
    COMMAND cppcheck ${CHECK_FLAGS} "-I${PROJECT_SOURCE_DIR}/include" ${CHECK_STD} ${CHECK_FULL} ${PROJECT_SOURCE_DIR}
)
add_custom_target (checkConfig echo "Checking cppcheck config..."
    COMMAND cppcheck ${CHECK_FLAGS} "-I${PROJECT_SOURCE_DIR}/include" ${CHECK_CONFIG} ${PROJECT_SOURCE_DIR}
)

#################################################################

# Generate Doxygen Documentation
find_package (Doxygen)
if (DOXYGEN_FOUND)
    # Configure the Template Doxyfile for our specific project
    configure_file(Doxyfile.in 
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
endif(DOXYGEN_FOUND)

# Clean doc files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "doc")
