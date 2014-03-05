cmake_minimum_required (VERSION 2.8)
project (OpenRaider)

# Version Number
set (OpenRaider_VERSION_MAJOR 0)
set (OpenRaider_VERSION_MINOR 1)
set (OpenRaider_VERSION_MICRO 2)

# Build Host
set (OpenRaider_BUILD_HOST ${CMAKE_HOST_SYSTEM})

# Apple specific bundling
if (APPLE)

    # Mac Bundle Data
    set (MACOSX_BUNDLE_INFO_STRING "Open Tomb Raider Engine")
    set (MACOSX_BUNDLE_ICON_FILE "AppIcon")
    set (MACOSX_BUNDLE_GUI_IDENTIFIER "de.xythobuz.OpenRaider")
    set (MACOSX_BUNDLE_LONG_VERSION_STRING "${OpenRaider_VERSION_MAJOR}.${OpenRaider_VERSION_MINOR}.${OpenRaider_VERSION_MICRO}")
    set (MACOSX_BUNDLE_BUNDLE_NAME "OpenRaider")
    set (MACOSX_BUNDLE_SHORT_VERSION_STRING "${OpenRaider_VERSION_MAJOR}.${OpenRaider_VERSION_MINOR}.${OpenRaider_VERSION_MICRO}")
    set (MACOSX_BUNDLE_BUNDLE_VERSION "${OpenRaider_VERSION_MAJOR}.${OpenRaider_VERSION_MINOR}.${OpenRaider_VERSION_MICRO}")
    set (MACOSX_BUNDLE_COPYRIGHT "2001 - 2014")

    # Copy Icon
    set (MAC_ICON "${PROJECT_SOURCE_DIR}/data/AppIcon.icns")
    set_source_files_properties(${MAC_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set (SRCS ${SRCS} ${MAC_ICON})

endif (APPLE)

# Configuration Header file
configure_file (
    "${PROJECT_SOURCE_DIR}/include/Config.h.in"
    "${PROJECT_BINARY_DIR}/Config.h"
)
include_directories ("${PROJECT_BINARY_DIR}")

# Executable, Source files
add_executable (OpenRaider MACOSX_BUNDLE
    src/Camera.cpp
    src/Emitter.cpp
    src/GLString.cpp
    src/MatMath.cpp
    src/Matrix.cpp
    src/Mesh.cpp
    src/Network.cpp
    src/OpenRaider.cpp
    src/Particle.cpp
    src/Quaternion.cpp
    src/Render.cpp
    src/SDLSystem.cpp
    src/SkeletalModel.cpp
    src/Sound.cpp
    src/System.cpp
    src/Texture.cpp
    src/TombRaider.cpp
    src/Vector3d.cpp
    src/ViewVolume.cpp
    src/World.cpp
    src/tga.cpp
    ${SRCS}
)

# Default to Debug build
set (CMAKE_BUILD_TYPE "Debug")

# Flags for all builds
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -std=c++11")
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -DUSING_EMITTER")

# Flags for Debug build
set (OpenRaider_CXX_FLAGS_DEBUG "${OpenRaider_CXX_FLAGS_DEBUG} -DDEBUG")
set (OpenRaider_CXX_FLAGS_DEBUG "${OpenRaider_CXX_FLAGS_DEBUG} -DEXPERIMENTAL")

# Flags for Releasae builds
set (OpenRaider_CXX_FLAGS_RELEASE "${OpenRaider_CXX_FLAGS_RELEASE}")

# Apply Flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenRaider_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_RELEASE}")

# Include directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Include External Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Add SDL2 Library
find_package (SDL2 REQUIRED)
include_directories (${SDL2_INCLUDE_DIR})
set (LIBS ${LIBS} ${SDL2_LIBRARY})

# Add SDL2_ttf Library
find_package (SDL2TTF REQUIRED)
include_directories (${SDL2TTF_INCLUDE_DIR})
set (LIBS ${LIBS} ${SDL2TTF_LIBRARY})

# Add OpenGL Library
find_package (OpenGL REQUIRED)
include_directories (${OPENGL_INCLUDE_DIRS})
set (LIBS ${LIBS} ${OPENGL_LIBRARIES})

# Add OpenAL Library
find_package (OpenAL REQUIRED)
include_directories (${OPENAL_INCLUDE_DIRS})
set (LIBS ${LIBS} ${OPENAL_LIBRARIES})

# Add ALUT Library
find_package (ALUT REQUIRED)
include_directories (${ALUT_INCLUDE_DIRS})
set (LIBS ${LIBS} ${ALUT_LIBRARIES})

# Add Z Library
find_package (ZLIB REQUIRED)
include_directories (${ZLIB_INCLUDE_DIRS})
set (LIBS ${LIBS} ${ZLIB_LIBRARIES})

# Link to all found libs
target_link_libraries (OpenRaider ${LIBS})

# Preparing the bundle on install
# http://www.cmake.org/Wiki/BundleUtilitiesExample
# Why does it even work?
if (APPLE)
    set (plugin_dest_dir OpenRaider.app/Contents/MacOS)
    set (APPS "${PROJECT_BINARY_DIR}/OpenRaider.app")
    set (DIRS ${QT_LIBRARY_DIRS})
    install (CODE "
        file(GLOB_RECURSE PLUGINS
            \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\${PLUGINS}\" \"${DIRS}\")
        " COMPONENT Runtime
    )
endif (APPLE)
