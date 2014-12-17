# Source files
set (DEPS_SRCS ${DEPS_SRCS} "commander/commander.c" "commander/commander.h")
set (DEPS_SRCS ${DEPS_SRCS} "imgui/imgui.cpp" "imgui/imgui.h" "imgui/imconfig.h")

# Add library
add_library (OpenRaider_deps OBJECT ${DEPS_SRCS})

