# Source files
set (UTIL_SRCS ${UTIL_SRCS} "strings.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "tga.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "time.cpp")

# Include directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Add library
add_library (OpenRaider_utils ${UTIL_SRCS})
