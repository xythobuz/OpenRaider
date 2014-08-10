# Source files
set (UTIL_SRCS ${UTIL_SRCS} "binary.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "File.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "filesystem.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "Folder.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "FolderRecursive.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "pcx.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "pixel.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "strings.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "tga.cpp")
set (UTIL_SRCS ${UTIL_SRCS} "time.cpp")

if (USING_PNG)
    set (UTIL_SRCS ${UTIL_SRCS} "png.cpp")
endif (USING_PNG)

# Add library
add_library (OpenRaider_utils OBJECT ${UTIL_SRCS})

