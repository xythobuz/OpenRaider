# Set Source files
set (SRCS ${SRCS} Camera.cpp)
set (SRCS ${SRCS} Emitter.cpp)
set (SRCS ${SRCS} GLString.cpp)
set (SRCS ${SRCS} MatMath.cpp)
set (SRCS ${SRCS} Matrix.cpp)
set (SRCS ${SRCS} Mesh.cpp)
set (SRCS ${SRCS} Network.cpp)
set (SRCS ${SRCS} OpenRaider.cpp)
set (SRCS ${SRCS} Particle.cpp)
set (SRCS ${SRCS} Quaternion.cpp)
set (SRCS ${SRCS} Render.cpp)
set (SRCS ${SRCS} SDLSystem.cpp)
set (SRCS ${SRCS} SkeletalModel.cpp)
set (SRCS ${SRCS} Sound.cpp)
set (SRCS ${SRCS} System.cpp)
set (SRCS ${SRCS} Texture.cpp)
set (SRCS ${SRCS} TombRaider.cpp)
set (SRCS ${SRCS} Vector3d.cpp)
set (SRCS ${SRCS} ViewVolume.cpp)
set (SRCS ${SRCS} World.cpp)
set (SRCS ${SRCS} tga.cpp)

# Apple specific bundling
if (APPLE)
    # Mac Bundle Data
    set (MACOSX_BUNDLE_INFO_STRING "OpenSource TombRaider Game Engine")
    set (MACOSX_BUNDLE_ICON_FILE "AppIcon")
    set (MACOSX_BUNDLE_GUI_IDENTIFIER "de.xythobuz.OpenRaider")
    set (MACOSX_BUNDLE_LONG_VERSION_STRING "${OpenRaider_VERSION_MAJOR}.${OpenRaider_VERSION_MINOR}.${OpenRaider_VERSION_MICRO}")
    set (MACOSX_BUNDLE_BUNDLE_NAME "OpenRaider")
    set (MACOSX_BUNDLE_SHORT_VERSION_STRING "${OpenRaider_VERSION_MAJOR}.${OpenRaider_VERSION_MINOR}.${OpenRaider_VERSION_MICRO}")
    set (MACOSX_BUNDLE_BUNDLE_VERSION "${OpenRaider_VERSION_MAJOR}.${OpenRaider_VERSION_MINOR}.${OpenRaider_VERSION_MICRO}")
    set (MACOSX_BUNDLE_COPYRIGHT "2001 - 2014")

    # Copy Icon
    set (MAC_ICON "../data/AppIcon.icns")
    set_source_files_properties(${MAC_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set (SRCS ${SRCS} ${MAC_ICON})
endif (APPLE)

# Add Executable
add_executable (OpenRaider MACOSX_BUNDLE ${SRCS})

# Debug build warnings
set (WARNINGS "${WARNINGS} -Weverything -Wno-padded -Wno-packed")
set (WARNINGS "${WARNINGS} -Wno-global-constructors -Wno-exit-time-destructors")
set (WARNINGS "${WARNINGS} -Wno-documentation-unknown-command -Wno-c++98-compat-pedantic")
set (WARNINGS "${WARNINGS} -Wno-missing-prototypes -Wno-missing-variable-declarations")
set (WARNINGS "${WARNINGS} -Wno-disabled-macro-expansion")

set (WARNINGS "${WARNINGS} -Wno-shorten-64-to-32 -Wno-sign-conversion")

# Flags for all builds
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -std=c++11")
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -DUSING_EMITTER")

# Flags for Debug build
set (OpenRaider_CXX_FLAGS_DEBUG "${OpenRaider_CXX_FLAGS_DEBUG} -g -O0 ${WARNINGS}")
set (OpenRaider_CXX_FLAGS_DEBUG "${OpenRaider_CXX_FLAGS_DEBUG} -DDEBUG")
set (OpenRaider_CXX_FLAGS_DEBUG "${OpenRaider_CXX_FLAGS_DEBUG} -DEXPERIMENTAL")

# Flags for Releasae builds
set (OpenRaider_CXX_FLAGS_RELEASE "${OpenRaider_CXX_FLAGS_RELEASE} -ffast-math -funroll-loops")
set (OpenRaider_CXX_FLAGS_RELEASE "${OpenRaider_CXX_FLAGS_RELEASE} -fomit-frame-pointer -O2")

# Apply Flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenRaider_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_RELEASE}")

# Include directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Include External Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Add Math Library
set (LIBS ${LIBS} m)

# Add SDL2 Library
find_package (SDL2 REQUIRED)
include_directories (SYSTEM ${SDL2_INCLUDE_DIR})
set (LIBS ${LIBS} ${SDL2_LIBRARY})

# Add SDL2_ttf Library
find_package (SDL2TTF REQUIRED)
include_directories (SYSTEM ${SDL2TTF_INCLUDE_DIR})
set (LIBS ${LIBS} ${SDL2TTF_LIBRARY})

# Add OpenGL Library
find_package (OpenGL REQUIRED)
include_directories (SYSTEM ${OPENGL_INCLUDE_DIRS})
set (LIBS ${LIBS} ${OPENGL_LIBRARIES})

# Add OpenAL Library
find_package (OpenAL REQUIRED)
include_directories (SYSTEM ${OPENAL_INCLUDE_DIRS})
set (LIBS ${LIBS} ${OPENAL_LIBRARIES})

# Add ALUT Library
find_package (ALUT REQUIRED)
include_directories (SYSTEM ${ALUT_INCLUDE_DIRS})
set (LIBS ${LIBS} ${ALUT_LIBRARIES})

# Add Z Library
find_package (ZLIB REQUIRED)
include_directories (SYSTEM ${ZLIB_INCLUDE_DIRS})
set (LIBS ${LIBS} ${ZLIB_LIBRARIES})

# Add Thread Library
find_package (Threads REQUIRED)
set (LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT})

# Link to all found libs
target_link_libraries (OpenRaider ${LIBS})

# Add target to run executable
add_custom_target(run COMMAND OpenRaider DEPENDS OpenRaider WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
