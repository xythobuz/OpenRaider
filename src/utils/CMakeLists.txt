# Source files
set (UTIL_SRCS ${UTIL_SRCS} "strings.cpp")

# Include directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Add library
add_library (utilities ${UTIL_SRCS})
