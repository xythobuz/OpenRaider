# Source files
set (UTIL_SRCS ${UTIL_SRCS} "math.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "Matrix.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "Quaternion.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "strings.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "tga.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "time.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "Vector3d.cpp")

# Include directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Add library
add_library (utilities ${UTIL_SRCS})
