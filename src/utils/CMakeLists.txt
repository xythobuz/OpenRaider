# Source files
set (UTIL_SRCS ${UTIL_SRCS} "binary.cpp" "../../include/utils/binary.h")
set (UTIL_SRCS ${UTIL_SRCS} "filesystem.cpp" "../../include/utils/filesystem.h")
set (UTIL_SRCS ${UTIL_SRCS} "Folder.cpp" "../../include/utils/Folder.h")
set (UTIL_SRCS ${UTIL_SRCS} "FolderRecursive.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "pcx.cpp" "../../include/utils/pcx.h")
set (UTIL_SRCS ${UTIL_SRCS} "pixel.cpp" "../../include/utils/pixel.h")
set (UTIL_SRCS ${UTIL_SRCS} "random.cpp" "../../include/utils/random.h")
set (UTIL_SRCS ${UTIL_SRCS} "strings.cpp" "../../include/utils/strings.h")
set (UTIL_SRCS ${UTIL_SRCS} "time.cpp" "../../include/utils/time.h")

# Add library
add_library (OpenRaider_utils OBJECT ${UTIL_SRCS})

