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

# Add test target
add_custom_target (tests)
add_dependencies (tests GLStringTest MathTest MatrixTest NetworkTest SoundTest TGATest TombRaiderTest)

# Preparing the bundle on install
# http://www.cmake.org/Wiki/BundleUtilitiesExample
# Why does it even work?
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
