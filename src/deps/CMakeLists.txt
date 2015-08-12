# Source files
set (DEPS_SRCS ${DEPS_SRCS} "ezoptionparser/ezOptionParser.hpp")
set (DEPS_SRCS ${DEPS_SRCS} "imgui/imgui.cpp" "imgui/imgui.h" "imgui/imconfig.h" "imgui/stb_textedit.h")
set (DEPS_SRCS ${DEPS_SRCS} "stb/stb.cpp" "stb/stb_image.h" "stb/stb_image_write.h")

# Add library
add_library (OpenRaider_deps OBJECT ${DEPS_SRCS})

