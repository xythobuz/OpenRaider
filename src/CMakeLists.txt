# Add SDL2 Library
find_package (SDL2)
include_directories (SYSTEM ${SDL2_INCLUDE_DIR})
set (LIBS ${LIBS} ${SDL2_LIBRARY})

# Add SDL2_ttf Library
find_package (SDL2TTF)
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

#################################################################

# Set Source files
set (SRCS ${SRCS} "Console.cpp")
set (SRCS ${SRCS} "main.cpp")
set (SRCS ${SRCS} "Menu.cpp")
set (SRCS ${SRCS} "OpenRaider.cpp")
set (SRCS ${SRCS} "Sound.cpp")
set (SRCS ${SRCS} "Window.cpp")

# Select available Windowing library
if (SDL2_FOUND AND SDL2TTF_FOUND)
    set (SRCS ${SRCS} "WindowSDL.cpp")
else (SDL2_FOUND AND SDL2TTF_FOUND)
    # Currently only SDL2 support
    message (FATAL_ERROR "SDL2 and SDL2-TTF are required!")
endif (SDL2_FOUND AND SDL2TTF_FOUND)

#################################################################

# Configuration Header file
configure_file (
    "${PROJECT_SOURCE_DIR}/include/Config.h.in"
    "${PROJECT_BINARY_DIR}/Config.h"
)

#################################################################

# Apple specific bundling
if (APPLE)
    # Mac Bundle Data
    set (MACOSX_BUNDLE_INFO_STRING "OpenSource TombRaider Game Engine")
    set (MACOSX_BUNDLE_ICON_FILE "AppIcon")
    set (MACOSX_BUNDLE_GUI_IDENTIFIER "de.xythobuz.OpenRaider")
    set (MACOSX_BUNDLE_BUNDLE_NAME "OpenRaider")
    set (MACOSX_BUNDLE_SHORT_VERSION_STRING "${OpenRaider_VERSION_MAJOR}.${OpenRaider_VERSION_MINOR}.${OpenRaider_VERSION_MICRO}${OpenRaider_VERSION_RELEASE}")
    set (MACOSX_BUNDLE_LONG_VERSION_STRING "${OpenRaider_VERSION_MAJOR}.${OpenRaider_VERSION_MINOR}.${OpenRaider_VERSION_MICRO}${OpenRaider_VERSION_RELEASE} (${CMAKE_BUILD_TYPE})")
    set (MACOSX_BUNDLE_BUNDLE_VERSION "${OpenRaider_VERSION_MAJOR}.${OpenRaider_VERSION_MINOR}.${OpenRaider_VERSION_MICRO}${OpenRaider_VERSION_RELEASE} (${CMAKE_BUILD_TYPE})")
    set (MACOSX_BUNDLE_COPYRIGHT "2001 - 2014")

    # Copy Icon
    set (MAC_ICON "../cmake/AppIcon.icns")
    set_source_files_properties (${MAC_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set (SRCS ${SRCS} ${MAC_ICON})

    # Copy Data
    set (MAC_DATA "../data/OpenRaider.ini" "../data/test.ttf")
    set (MAC_DATA ${MAC_DATA} "../data/snow.tga" "../data/snow2.tga" "../data/splash.tga")
    set_source_files_properties (${MAC_DATA} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/defaults)
    set (SRCS ${SRCS} ${MAC_DATA})
endif (APPLE)

#################################################################

# Check for SSE, enable if available
include (../cmake/FindSSE.cmake)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (SSE_FOUND)
        set (SSE_FOUND_MSG "${SSE_FOUND_MSG} SSE")
        set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -msse")
    endif (SSE_FOUND)
    if (SSE2_FOUND)
        set (SSE_FOUND_MSG "${SSE_FOUND_MSG} SSE2")
        set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -msse2")
    endif (SSE2_FOUND)
    if (SSE3_FOUND)
        set (SSE_FOUND_MSG "${SSE_FOUND_MSG} SSE3")
        set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -msse3")
    endif (SSE3_FOUND)
    if (SSSE3_FOUND)
        set (SSE_FOUND_MSG "${SSE_FOUND_MSG} SSSE3")
        set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -mssse3")
    endif (SSSE3_FOUND)
    if (SSE4_1_FOUND)
        set (SSE_FOUND_MSG "${SSE_FOUND_MSG} SSE4.1")
        set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -msse4.1")
    endif (SSE4_1_FOUND)

    # Display message if something is enabled
    if (NOT "${SSE_FOUND_MSG}" STREQUAL "")
        message (STATUS "Enabled${SSE_FOUND_MSG}...")
    endif (NOT "${SSE_FOUND_MSG}" STREQUAL "")
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

# Apply Flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenRaider_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_RELEASE}")

# Add subdirectories
add_subdirectory ("math")
add_subdirectory ("utils")

# Add Executable
add_executable (OpenRaider MACOSX_BUNDLE ${SRCS})

#################################################################

if (APPLE)
    if (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
        # Copy setup script
        add_custom_command (TARGET OpenRaider POST_BUILD
            COMMAND echo "Injecting setup script..."
            COMMAND cp "${PROJECT_SOURCE_DIR}/cmake/setup_mac.sh" "${PROJECT_BINARY_DIR}/src/OpenRaider.app/Contents/MacOS/OpenRaider.sh"
            COMMAND chmod "a+x" "${PROJECT_BINARY_DIR}/src/OpenRaider.app/Contents/MacOS/OpenRaider.sh"
        )

        # Fix executable name, so setup script will be called
        add_custom_command (TARGET OpenRaider POST_BUILD
            COMMAND echo "Changing executable name..."
            COMMAND /usr/libexec/PlistBuddy -c "Set :CFBundleExecutable OpenRaider.sh" Info.plist
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/src/OpenRaider.app/Contents
        )

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
    endif (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
endif (APPLE)

#################################################################

if (NOT APPLE)
    # Install binary
    install (TARGET OpenRaider
        RUNTIME DESTINATION bin
    )
endif (NOT APPLE)

#################################################################

# Add Math Library
set (LIBS ${LIBS} m)

# Add utils Library
set (LIBS ${LIBS} OpenRaider_math)
set (LIBS ${LIBS} OpenRaider_utils)

# Link to all found libs
target_link_libraries (OpenRaider ${LIBS})

#################################################################

# Add target to run executable
add_custom_target(run COMMAND OpenRaider DEPENDS OpenRaider WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
