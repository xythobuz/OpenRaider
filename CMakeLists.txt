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

# Preparing the bundle on install
# http://www.cmake.org/Wiki/BundleUtilitiesExample
# Why does it even work? O.o
if (APPLE)
    set (plugin_dest_dir OpenRaider.app/Contents/MacOS)
    set (APPS "${PROJECT_BINARY_DIR}/src/OpenRaider.app")
    set (DIRS ${QT_LIBRARY_DIRS})
    install (CODE "
        file(GLOB_RECURSE PLUGINS
            \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\${PLUGINS}\" \"${DIRS}\")
        " COMPONENT Runtime
    )
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

#docLocal:
#	sed -i '' 's/HAVE_DOT               = YES/HAVE_DOT               = NO/g' Doxyfile
#	doxygen
#	cp -R doc/html/* ../apache/
#	rm -rf doc
#	sed -i '' 's/HAVE_DOT               = NO/HAVE_DOT               = YES/g' Doxyfile
