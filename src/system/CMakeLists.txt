# Source files
set (SYS_SRCS ${SYS_SRCS} "Font.cpp")
set (SYS_SRCS ${SYS_SRCS} "FontImGui.cpp")
set (SYS_SRCS ${SYS_SRCS} "FontTRLE.cpp")
set (SYS_SRCS ${SYS_SRCS} "Sound.cpp")
set (SYS_SRCS ${SYS_SRCS} "Window.cpp")

# Select available Sound library
if (OPENAL_FOUND AND ALUT_FOUND AND ENABLE_AUDIO)
    set (SYS_SRCS ${SYS_SRCS} "SoundAL.cpp")
else (OPENAL_FOUND AND ALUT_FOUND AND ENABLE_AUDIO)
    message (STATUS "Disabled Sound support (no OpenAL and ALUT)!")
endif (OPENAL_FOUND AND ALUT_FOUND AND ENABLE_AUDIO)

# Select available Windowing library
if (SDL2_FOUND AND NOT FORCE_GLUT)
    set (SYS_SRCS ${SYS_SRCS} "WindowSDL.cpp")
    if (SDL2TTF_FOUND)
        set (SYS_SRCS ${SYS_SRCS} "FontSDL.cpp")
    endif (SDL2TTF_FOUND)
else (SDL2_FOUND AND NOT FORCE_GLUT)
    if (FREEGLUT_FOUND)
        set (SYS_SRCS ${SYS_SRCS} "WindowGLUT.cpp")
    else (FREEGLUT_FOUND)
        message (FATAL_ERROR "SDL2 or freeGLUT are required!")
    endif (FREEGLUT_FOUND)
endif (SDL2_FOUND AND NOT FORCE_GLUT)

# Add library
add_library (OpenRaider_system OBJECT ${SYS_SRCS})

