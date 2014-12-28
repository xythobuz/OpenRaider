# Source files
set (SYS_SRCS ${SYS_SRCS} "Font.cpp" "../../include/system/Font.h")
set (SYS_SRCS ${SYS_SRCS} "FontImGui.cpp" "../../include/system/FontImGui.h")
set (SYS_SRCS ${SYS_SRCS} "FontTRLE.cpp" "../../include/system/FontTRLE.h")
set (SYS_SRCS ${SYS_SRCS} "Shader.cpp" "../../include/system/Shader.h")
set (SYS_SRCS ${SYS_SRCS} "Sound.cpp" "../../include/system/Sound.h")
set (SYS_SRCS ${SYS_SRCS} "Window.cpp" "../../include/system/Window.h")

# Select available Sound library
if (USING_AL AND ENABLE_AUDIO)
    set (SYS_SRCS ${SYS_SRCS} "SoundAL.cpp" "../../include/system/SoundAL.h")
else (USING_AL AND ENABLE_AUDIO)
    message (STATUS "Disabled Sound support (no OpenAL and ALUT)!")
endif (USING_AL AND ENABLE_AUDIO)

if (USING_SDL)
    set (SYS_SRCS ${SYS_SRCS} "WindowSDL.cpp" "../../include/system/WindowSDL.h")
endif (USING_SDL)

if (USING_SDL_FONT)
    set (SYS_SRCS ${SYS_SRCS} "FontSDL.cpp" "../../include/system/FontSDL.h")
endif (USING_SDL_FONT)

if (USING_GLFW)
    set (SYS_SRCS ${SYS_SRCS} "WindowGLFW.cpp" "../../include/system/WindowGLFW.h")
endif (USING_GLFW)

# Add library
add_library (OpenRaider_system OBJECT ${SYS_SRCS})

