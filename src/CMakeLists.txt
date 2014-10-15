# Options
option (ENABLE_AUDIO "Enable Sound Output" YES)
option (FORCE_GLUT "Use freeGLUT even if SDL2 was found" NO)

# Add OpenGL Library
find_package (OpenGL REQUIRED)
include_directories (SYSTEM ${OPENGL_INCLUDE_DIRS})
set (LIBS ${LIBS} ${OPENGL_LIBRARIES})

# Add Z Library
find_package (ZLIB REQUIRED)
include_directories (SYSTEM ${ZLIB_INCLUDE_DIRS})
set (LIBS ${LIBS} ${ZLIB_LIBRARIES})

# Add SDL2 Library
find_package (SDL2)
if (SDL2_FOUND AND NOT FORCE_GLUT)
    include_directories (SYSTEM ${SDL2_INCLUDE_DIR})
    set (LIBS ${LIBS} ${SDL2_LIBRARY})

    # Add SDL2_ttf Library
    find_package (SDL2TTF)
    if (SDL2TTF_FOUND)
        include_directories (SYSTEM ${SDL2TTF_INCLUDE_DIR})
        set (LIBS ${LIBS} ${SDL2TTF_LIBRARY})
    endif (SDL2TTF_FOUND)
else (SDL2_FOUND AND NOT FORCE_GLUT)
    # Add freeglut Library
    find_package (FREEGLUT)
    if (FREEGLUT_FOUND)
        include_directories (SYSTEM ${FREEGLUT_INCLUDE_DIRS})
        set (LIBS ${LIBS} ${FREEGLUT_LIBRARIES})
    endif (FREEGLUT_FOUND)
endif (SDL2_FOUND AND NOT FORCE_GLUT)

# Add OpenAL Library
find_package (OpenAL)
if (OPENAL_FOUND)
    include_directories (SYSTEM ${OPENAL_INCLUDE_DIRS})
    set (LIBS ${LIBS} ${OPENAL_LIBRARIES})

    # Add ALUT Library
    find_package (ALUT)
    if (ALUT_FOUND)
        include_directories (SYSTEM ${ALUT_INCLUDE_DIRS})
        set (LIBS ${LIBS} ${ALUT_LIBRARIES})
    endif (ALUT_FOUND)
endif (OPENAL_FOUND)

# Add PNG Library
find_package (PNG)
if (PNG_FOUND)
    include_directories (SYSTEM ${PNG_INCLUDE_DIRS})
    set (LIBS ${LIBS} ${PNG_LIBRARIES})
    add_definitions (${PNG_DEFINITIONS})
endif (PNG_FOUND)

#################################################################

# Set Source files
set (SRCS ${SRCS} "Camera.cpp")
set (SRCS ${SRCS} "Console.cpp")
set (SRCS ${SRCS} "Entity.cpp")
set (SRCS ${SRCS} "Exception.cpp")
set (SRCS ${SRCS} "Font.cpp")
set (SRCS ${SRCS} "FontImGui.cpp")
set (SRCS ${SRCS} "FontTRLE.cpp")
set (SRCS ${SRCS} "Game.cpp")
set (SRCS ${SRCS} "Log.cpp")
set (SRCS ${SRCS} "main.cpp")
set (SRCS ${SRCS} "Menu.cpp")
set (SRCS ${SRCS} "MenuFolder.cpp")
set (SRCS ${SRCS} "Mesh.cpp")
set (SRCS ${SRCS} "Render.cpp")
set (SRCS ${SRCS} "Room.cpp")
set (SRCS ${SRCS} "RoomData.cpp")
set (SRCS ${SRCS} "RunTime.cpp")
set (SRCS ${SRCS} "Script.cpp")
set (SRCS ${SRCS} "SkeletalModel.cpp")
set (SRCS ${SRCS} "Sound.cpp")
set (SRCS ${SRCS} "Sprite.cpp")
set (SRCS ${SRCS} "StaticMesh.cpp")
set (SRCS ${SRCS} "TextureManager.cpp")
set (SRCS ${SRCS} "TombRaider.cpp")
set (SRCS ${SRCS} "UI.cpp")
set (SRCS ${SRCS} "ViewVolume.cpp")
set (SRCS ${SRCS} "Window.cpp")
set (SRCS ${SRCS} "World.cpp")

# Select available Sound library
if (OPENAL_FOUND AND ALUT_FOUND AND ENABLE_AUDIO)
    set (SRCS ${SRCS} "SoundAL.cpp")
    set (USING_AL TRUE)
else (OPENAL_FOUND AND ALUT_FOUND AND ENABLE_AUDIO)
    set (SRCS ${SRCS} "SoundNull.cpp")
    set (USING_AL FALSE)
    message (STATUS "Disabled Sound support (no OpenAL and ALUT)!")
endif (OPENAL_FOUND AND ALUT_FOUND AND ENABLE_AUDIO)

# Select available Windowing library
if (SDL2_FOUND AND NOT FORCE_GLUT)
    set (SRCS ${SRCS} "WindowSDL.cpp")
    set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -DUSING_SDL")
    if (SDL2TTF_FOUND)
        set (SRCS ${SRCS} "FontSDL.cpp")
        set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -DUSING_SDL_FONT")
    endif (SDL2TTF_FOUND)
else (SDL2_FOUND AND NOT FORCE_GLUT)
    if (FREEGLUT_FOUND)
        set (SRCS ${SRCS} "WindowGLUT.cpp")
        set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -DUSING_GLUT")
    else (FREEGLUT_FOUND)
        message (FATAL_ERROR "SDL2 or freeGLUT are required!")
    endif (FREEGLUT_FOUND)
endif (SDL2_FOUND AND NOT FORCE_GLUT)

if (PNG_FOUND)
    set (USING_PNG TRUE)
else (PNG_FOUND)
    set (USING_PNG FALSE)
endif (PNG_FOUND)

#################################################################

# Check for header/function presence
include (CheckIncludeFiles)
include (CheckFunctionExists)
include (CheckSymbolExists)

# backtrace() for assert with call stack output
check_include_files (execinfo.h HAVE_EXECINFO_H)
check_function_exists (backtrace HAVE_BACKTRACE)
check_function_exists (backtrace_symbols HAVE_BACKTRACE_SYMBOLS)

# readdir() for recursive folder traversal
check_include_files (dirent.h HAVE_DIRENT_H)
check_function_exists (opendir HAVE_OPENDIR)
check_function_exists (readdir_r HAVE_READDIR_R)
check_function_exists (closedir HAVE_CLOSEDIR)
check_symbol_exists (DT_DIR "dirent.h" HAVE_DT_DIR)

# getcwd() for the current working directory
check_include_files (unistd.h HAVE_UNISTD_H)
check_function_exists (getcwd HAVE_GETCWD)

# getenv() for reading environment variables
check_include_files (stdlib.h HAVE_STDLIB_H)
check_function_exists (getenv HAVE_GETENV)

#################################################################

# Configuration Header file
configure_file (
    "${PROJECT_SOURCE_DIR}/include/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

# Configuration Source file
configure_file (
    "${PROJECT_SOURCE_DIR}/src/config.cpp.in"   
    "${PROJECT_BINARY_DIR}/config.cpp"
)
set (SRCS ${SRCS} "${PROJECT_BINARY_DIR}/config.cpp")

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
    set (RESRCS ${RESRCS} ${MAC_ICON})

    # Copy Data
    set (MAC_DATA "../data/font.pc" "../data/OpenRaider.ini" "../data/test.ttf" "../data/splash.tga")
    set_source_files_properties (${MAC_DATA} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/defaults)
    set (RESRCS ${RESRCS} ${MAC_DATA})
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
add_subdirectory ("commands")
add_subdirectory ("deps")
add_subdirectory ("loader")
add_subdirectory ("math")
add_subdirectory ("utils")

# Add Executable
add_library (OpenRaider_all OBJECT ${SRCS})
add_executable (OpenRaider MACOSX_BUNDLE ${RESRCS}
    $<TARGET_OBJECTS:OpenRaider_all> $<TARGET_OBJECTS:OpenRaider_commands>
    $<TARGET_OBJECTS:OpenRaider_deps> $<TARGET_OBJECTS:OpenRaider_math>
    $<TARGET_OBJECTS:OpenRaider_utils> $<TARGET_OBJECTS:OpenRaider_loader>
)

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

# Install rules
if (APPLE)
    # We want to install to /Applications by default, not /usr
    set (OR_INSTALL_PREFIX "/" CACHE PATH "Prefix prepended to
install directories")
    set (CMAKE_INSTALL_PREFIX "${OR_INSTALL_PREFIX}" CACHE INTERNAL "Prefix
prepended to install directories" FORCE)

    # Only the App Bundle is required on OS X
    install (TARGETS OpenRaider BUNDLE DESTINATION Applications)
elseif (NOT WIN32)
    # We need a special config file for this install method
    configure_file (
        "${PROJECT_SOURCE_DIR}/data/Unix.in"   
        "${PROJECT_BINARY_DIR}/Unix.ini"
    )

    # Executable
    install (TARGETS OpenRaider RUNTIME DESTINATION bin)

    # Config file
    install (FILES "${PROJECT_BINARY_DIR}/Unix.ini"
        DESTINATION "share/OpenRaider"
        RENAME "OpenRaider.ini")

    # Data files
    install (FILES "${PROJECT_SOURCE_DIR}/data/font.pc"
                    "${PROJECT_SOURCE_DIR}/data/splash.tga"
                    "${PROJECT_SOURCE_DIR}/data/test.ttf"
        DESTINATION "share/OpenRaider")
endif (APPLE)

#################################################################

# Add Math Library
if (UNIX)
    set (LIBS ${LIBS} m)
endif (UNIX)

# Link to all found libs
target_link_libraries (OpenRaider ${LIBS})

#################################################################

# Add target to run executable
if (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" OR ${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
    add_custom_target(run COMMAND OpenRaider DEPENDS OpenRaider WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
endif (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" OR ${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")

