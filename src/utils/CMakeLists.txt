# Source files
set (UTIL_SRCS ${UTIL_SRCS} "pcx.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "pixel.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "strings.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "tga.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "time.cpp")

if (USING_PNG)
    set (UTIL_SRCS ${UTIL_SRCS} "png.cpp")
endif (USING_PNG)

# Include directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Add library
add_library (OpenRaider_utils ${UTIL_SRCS})
