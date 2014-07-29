# Source files
set (DEPS_SRCS ${DEPS_SRCS} "commander/commander.c")

# Add library
add_library (OpenRaider_deps OBJECT ${DEPS_SRCS})

