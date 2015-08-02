# Options
option (ENABLE_AUDIO "Enable Sound Output" YES)
option (FORCE_GLFW "Use GLFW even if SDL2 was found" NO)

# Add OpenGL Library
find_package (OpenGL REQUIRED)
include_directories (SYSTEM ${OPENGL_INCLUDE_DIRS})
set (LIBS ${LIBS} ${OPENGL_LIBRARIES})

# Add GLM Library
find_package (GLM REQUIRED)
include_directories (SYSTEM  ${GLM_INCLUDE_DIRS})

# Add glbinding Library
find_package (glbinding REQUIRED)
include_directories (SYSTEM ${GLBINDING_INCLUDES})
set (LIBS ${LIBS} ${GLBINDING_LIBRARIES})

# Add SDL2 Library
find_package (SDL2)
if (SDL2_FOUND AND NOT FORCE_GLFW)
    include_directories (SYSTEM ${SDL2_INCLUDE_DIR})
    set (LIBS ${LIBS} ${SDL2_LIBRARY})
else (SDL2_FOUND AND NOT FORCE_GLFW)
    find_package (GLFW)
    if (GLFW_FOUND)
        include_directories (SYSTEM ${GLFW_INCLUDE_DIRS})
        set (LIBS ${LIBS} ${GLFW_LIBRARIES})
    endif (GLFW_FOUND)
endif (SDL2_FOUND AND NOT FORCE_GLFW)

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

#################################################################

# Set Source files
set (SRCS ${SRCS} "BoundingBox.cpp" "../include/BoundingBox.h")
set (SRCS ${SRCS} "Camera.cpp" "../include/Camera.h")
set (SRCS ${SRCS} "Console.cpp" "../include/Console.h")
set (SRCS ${SRCS} "Entity.cpp" "../include/Entity.h")
set (SRCS ${SRCS} "Game.cpp" "../include/Game.h")
set (SRCS ${SRCS} "Log.cpp" "../include/Log.h")
set (SRCS ${SRCS} "main.cpp")
set (SRCS ${SRCS} "Menu.cpp" "../include/Menu.h")
set (SRCS ${SRCS} "Mesh.cpp" "../include/Mesh.h")
set (SRCS ${SRCS} "Render.cpp" "../include/Render.h")
set (SRCS ${SRCS} "Room.cpp" "../include/Room.h")
set (SRCS ${SRCS} "RoomData.cpp" "../include/RoomData.h")
set (SRCS ${SRCS} "RoomMesh.cpp" "../include/RoomMesh.h")
set (SRCS ${SRCS} "RunTime.cpp" "../include/RunTime.h")
set (SRCS ${SRCS} "Script.cpp" "../include/Script.h")
set (SRCS ${SRCS} "Selector.cpp" "../include/Selector.h")
set (SRCS ${SRCS} "SkeletalModel.cpp" "../include/SkeletalModel.h")
set (SRCS ${SRCS} "SoundManager.cpp" "../include/SoundManager.h")
set (SRCS ${SRCS} "Sprite.cpp" "../include/Sprite.h")
set (SRCS ${SRCS} "StaticMesh.cpp" "../include/StaticMesh.h")
set (SRCS ${SRCS} "TextureManager.cpp" "../include/TextureManager.h")
set (SRCS ${SRCS} "UI.cpp" "../include/UI.h")
set (SRCS ${SRCS} "World.cpp" "../include/World.h")

# Select available Sound library
if (OPENAL_FOUND AND ALUT_FOUND AND ENABLE_AUDIO)
    set (USING_AL TRUE)
else (OPENAL_FOUND AND ALUT_FOUND AND ENABLE_AUDIO)
    set (USING_AL FALSE)
    message (STATUS "Disabled Sound support (no OpenAL and ALUT)!")
endif (OPENAL_FOUND AND ALUT_FOUND AND ENABLE_AUDIO)

# Select available Windowing library
if (SDL2_FOUND AND NOT FORCE_GLFW)
    set (USING_GLFW FALSE)
    set (USING_SDL TRUE)
else (SDL2_FOUND AND NOT FORCE_GLFW)
    set (USING_SDL FALSE)
    if (GLFW_FOUND)
        set (USING_GLFW TRUE)
    else (GLFW_FOUND)
        set (USING_GLFW FALSE)
        message (FATAL_ERROR "SDL2 or GLFW are required!")
    endif (GLFW_FOUND)
endif (SDL2_FOUND AND NOT FORCE_GLFW)

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

# _getcwd() for current working directory in windows
check_include_files (direct.h HAVE_DIRECT_H)
check_function_exists (_getcwd HAVE__GETCWD)

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
    set (MAC_DATA "../data/OpenRaider.ini" "../data/splash.tga")
    set_source_files_properties (${MAC_DATA} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/defaults)
    set (RESRCS ${RESRCS} ${MAC_DATA})
endif (APPLE)

#################################################################

# Apply Flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenRaider_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_RELEASE}")

# Add subdirectories
add_subdirectory ("commands")
add_subdirectory ("deps")
add_subdirectory ("loader")
add_subdirectory ("system")
add_subdirectory ("utils")

# Add Executable
add_executable (OpenRaider MACOSX_BUNDLE ${RESRCS}
    ${SRCS} $<TARGET_OBJECTS:OpenRaider_commands>
    $<TARGET_OBJECTS:OpenRaider_deps> $<TARGET_OBJECTS:OpenRaider_loader>
    $<TARGET_OBJECTS:OpenRaider_utils> $<TARGET_OBJECTS:OpenRaider_system>
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
    install (FILES "${PROJECT_SOURCE_DIR}/data/splash.tga"
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

