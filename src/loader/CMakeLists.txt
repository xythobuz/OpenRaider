# Source files
set (LOADER_SRCS ${LOADER_SRCS} "Loader.cpp")
set (LOADER_SRCS ${LOADER_SRCS} "LoaderTR1.cpp")
set (LOADER_SRCS ${LOADER_SRCS} "LoaderTR2.cpp")
set (LOADER_SRCS ${LOADER_SRCS} "LoaderTR3.cpp")

# Add library
add_library (OpenRaider_loader OBJECT ${LOADER_SRCS})

