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

# Preprocessor Flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Source Files, Executable
add_executable (OpenRaider
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
)

# Definitions for Debug build
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DEXPERIMENTAL")

# Definitions for all builds
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSING_EMITTER")

# Include Directory
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

# Add the install targets
install (TARGETS OpenRaider DESTINATION bin)
