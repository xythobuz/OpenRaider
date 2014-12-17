# Source files
set (LOADER_SRCS ${LOADER_SRCS} "Loader.cpp" "../../include/loader/Loader.h")
set (LOADER_SRCS ${LOADER_SRCS} "LoaderTR1.cpp" "../../include/loader/LoaderTR1.h")
set (LOADER_SRCS ${LOADER_SRCS} "LoaderTR2.cpp" "../../include/loader/LoaderTR2.h")
set (LOADER_SRCS ${LOADER_SRCS} "LoaderTR3.cpp" "../../include/loader/LoaderTR3.h")

# Add library
add_library (OpenRaider_loader OBJECT ${LOADER_SRCS})

