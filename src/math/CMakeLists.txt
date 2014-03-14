# Source files
set (MATH_SRCS ${MATH_SRCS} "math.cpp")
set (MATH_SRCS ${MATH_SRCS} "Matrix.cpp")
set (MATH_SRCS ${MATH_SRCS} "Quaternion.cpp")
set (MATH_SRCS ${MATH_SRCS} "Vector3d.cpp")

# Include directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Add library
add_library (OpenRaider_math ${MATH_SRCS})
