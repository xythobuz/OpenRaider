# Source files
set (DEPS_SRCS ${DEPS_SRCS} "commander/commander.c")
set (DEPS_SRCS ${DEPS_SRCS} "imgui/imgui.cpp")

# Add library
add_library (OpenRaider_deps OBJECT ${DEPS_SRCS})

