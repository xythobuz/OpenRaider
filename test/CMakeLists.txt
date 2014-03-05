# Include directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Include External Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Warnings
set (WARNINGS "${WARNINGS} -Weverything -Wno-padded -Wno-packed")
set (WARNINGS "${WARNINGS} -Wno-global-constructors -Wno-exit-time-destructors")
set (WARNINGS "${WARNINGS} -Wno-documentation-unknown-command -Wno-c++98-compat-pedantic")
set (WARNINGS "${WARNINGS} -Wno-missing-prototypes -Wno-missing-variable-declarations")
set (WARNINGS "${WARNINGS} -Wno-disabled-macro-expansion")

set (WARNINGS "${WARNINGS} -Wno-shorten-64-to-32 -Wno-sign-conversion")

# Flags for all builds
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -std=c++11")
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -g -O0 ${WARNINGS}")
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -DDEBUG")
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -DUNIT_TEST")

#################################################################

# TGA Test
set (TGASRCS "${PROJECT_SOURCE_DIR}/test/tga.cpp" "${PROJECT_SOURCE_DIR}/src/tga.cpp")
add_executable (TGATest ${TGASRCS})
set_target_properties (TGATest PROPERTIES COMPILE_FLAGS ${OpenRaider_CXX_FLAGS})

#################################################################

# Add OpenAL Library
find_package (OpenAL REQUIRED)
include_directories (SYSTEM ${OPENAL_INCLUDE_DIRS})
set (SOUNDLIBS ${SOUNDLIBS} ${OPENAL_LIBRARIES})

# Add ALUT Library
find_package (ALUT REQUIRED)
include_directories (SYSTEM ${ALUT_INCLUDE_DIRS})
set (SOUNDLIBS ${SOUNDLIBS} ${ALUT_LIBRARIES})

# Sound Test
set (SOUNDSRCS "${PROJECT_SOURCE_DIR}/test/Sound.cpp" "${PROJECT_SOURCE_DIR}/src/Sound.cpp")
add_executable (SoundTest ${SOUNDSRCS})
set_target_properties (SoundTest PROPERTIES COMPILE_FLAGS ${OpenRaider_CXX_FLAGS})
target_link_libraries (SoundTest ${SOUNDLIBS})

#################################################################

# Add Thread Library
find_package (Threads REQUIRED)
set (THREADLIBS ${THREADLIBS} ${CMAKE_THREAD_LIBS_INIT})

# Network Test
set (NETWORKSRCS "${PROJECT_SOURCE_DIR}/test/Network.cpp" "${PROJECT_SOURCE_DIR}/src/Network.cpp")
set (NETWORKSRCS ${NETWORKSRCS} "${PROJECT_SOURCE_DIR}/src/MatMath.cpp" "${PROJECT_SOURCE_DIR}/src/Vector3d.cpp")
add_executable (NetworkTest ${NETWORKSRCS})
set_target_properties (NetworkTest PROPERTIES COMPILE_FLAGS ${OpenRaider_CXX_FLAGS})
target_link_libraries (NetworkTest m ${THREADLIBS})

#################################################################

# Math Test
set (MATHSRCS "${PROJECT_SOURCE_DIR}/test/MatMath.cpp" "${PROJECT_SOURCE_DIR}/src/MatMath.cpp" "${PROJECT_SOURCE_DIR}/src/Vector3d.cpp")
add_executable (MathTest ${MATHSRCS})
set_target_properties (MathTest PROPERTIES COMPILE_FLAGS ${OpenRaider_CXX_FLAGS})

#################################################################

# Matrix Test
set (MATRIXSRCS "${PROJECT_SOURCE_DIR}/test/Matrix.cpp" "${PROJECT_SOURCE_DIR}/src/Matrix.cpp")
set (MATRIXSRCS ${MATRIXSRCS} "${PROJECT_SOURCE_DIR}/src/Vector3d.cpp" "${PROJECT_SOURCE_DIR}/src/MatMath.cpp")
set (MATRIXSRCS ${MATRIXSRCS} "${PROJECT_SOURCE_DIR}/src/Quaternion.cpp")
add_executable (MatrixTest ${MATRIXSRCS})
set_target_properties (MatrixTest PROPERTIES COMPILE_FLAGS ${OpenRaider_CXX_FLAGS})
target_link_libraries (MatrixTest m)

#################################################################

# Add SDL2 Library
find_package (SDL2 REQUIRED)
include_directories (SYSTEM ${SDL2_INCLUDE_DIR})
set (GLLIBS ${GLLIBS} ${SDL2_LIBRARY})

# Add SDL2_ttf Library
find_package (SDL2TTF REQUIRED)
include_directories (SYSTEM ${SDL2TTF_INCLUDE_DIR})
set (GLLIBS ${GLLIBS} ${SDL2TTF_LIBRARY})

# Add OpenGL Library
find_package (OpenGL REQUIRED)
include_directories (SYSTEM ${OPENGL_INCLUDE_DIRS})
set (GLLIBS ${GLLIBS} ${OPENGL_LIBRARIES})

# GLString Test
set (GLSTRINGSRCS "${PROJECT_SOURCE_DIR}/test/GLString.cpp" "${PROJECT_SOURCE_DIR}/src/GLString.cpp")
set (GLSTRINGSRCS ${GLSTRINGSRCS} "${PROJECT_SOURCE_DIR}/src/tga.cpp" "${PROJECT_SOURCE_DIR}/src/Texture.cpp")
add_executable (GLStringTest ${GLSTRINGSRCS})
set_target_properties (GLStringTest PROPERTIES COMPILE_FLAGS ${OpenRaider_CXX_FLAGS})
target_link_libraries (GLStringTest m ${GLLIBS})

#################################################################

# Add Z Library
find_package (ZLIB REQUIRED)
include_directories (SYSTEM ${ZLIB_INCLUDE_DIRS})
set (ZLIBS ${ZLIBS} ${ZLIB_LIBRARIES})

# TombRaider Test
set (TOMB_FLAGS "-D__TEST_TR5_DUMP_TGA -D__TEST_32BIT_TEXTILES")
set (TOMBSRCS "${PROJECT_SOURCE_DIR}/test/TombRaider.cpp" "${PROJECT_SOURCE_DIR}/src/TombRaider.cpp")
set (TOMBSRCS ${TOMBSRCS} "${PROJECT_SOURCE_DIR}/src/tga.cpp")
add_executable (TombRaiderTest ${TOMBSRCS})
set_target_properties (TombRaiderTest PROPERTIES COMPILE_FLAGS "${OpenRaider_CXX_FLAGS} ${TOMB_FLAGS}")
target_link_libraries (TombRaiderTest ${ZLIBS})

#################################################################

#TEST_MAP_TR5=~/.OpenRaider/paks/tr5/demo.trc
#TEST_MAP_TR4=~/.OpenRaider/paks/tr4/angkor1.tr4
#TEST_MAP_TR3=~/.OpenRaider/paks/tr3/scotland.tr2
#TEST_MAP_TR2=~/.OpenRaider/paks/tr2/unwater.tr2
#TEST_MAP_TR1=~/.OpenRaider/paks/tr1/level1.phd

#TombRaider.reg_test:
#	$(MAKE) TombRaider.test
#	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR1) > /tmp/log.tr1
#	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR2) > /tmp/log.tr2
#	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR3) > /tmp/log.tr3
#	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR4) > /tmp/log.tr4
#	$(BUILD_TEST_DIR)/TombRaider.test load $(TEST_MAP_TR5) > /tmp/log.tr5
