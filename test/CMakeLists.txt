# Include directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Include External Modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Flags for all builds
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -std=c++11")
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -g -O0 ${WARNINGS}")
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -DDEBUG")
set (OpenRaider_CXX_FLAGS "${OpenRaider_CXX_FLAGS} -DUNIT_TEST")

#################################################################

# TGA Test
set (TGASRCS "${PROJECT_SOURCE_DIR}/test/tga.cpp" "${PROJECT_SOURCE_DIR}/src/utils/tga.cpp")
add_executable (TGATest EXCLUDE_FROM_ALL ${TGASRCS})
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
add_executable (SoundTest EXCLUDE_FROM_ALL ${SOUNDSRCS})
set_target_properties (SoundTest PROPERTIES COMPILE_FLAGS ${OpenRaider_CXX_FLAGS})
target_link_libraries (SoundTest ${SOUNDLIBS})

#################################################################

# Math Test
set (MATHSRCS "${PROJECT_SOURCE_DIR}/test/math.cpp" "${PROJECT_SOURCE_DIR}/src/utils/math.cpp" "${PROJECT_SOURCE_DIR}/src/Vector3d.cpp")
add_executable (MathTest EXCLUDE_FROM_ALL ${MATHSRCS})
set_target_properties (MathTest PROPERTIES COMPILE_FLAGS ${OpenRaider_CXX_FLAGS})

#################################################################

# Matrix Test
set (MATRIXSRCS "${PROJECT_SOURCE_DIR}/test/Matrix.cpp" "${PROJECT_SOURCE_DIR}/src/Matrix.cpp")
set (MATRIXSRCS ${MATRIXSRCS} "${PROJECT_SOURCE_DIR}/src/Vector3d.cpp" "${PROJECT_SOURCE_DIR}/src/utils/math.cpp")
set (MATRIXSRCS ${MATRIXSRCS} "${PROJECT_SOURCE_DIR}/src/Quaternion.cpp")
add_executable (MatrixTest EXCLUDE_FROM_ALL ${MATRIXSRCS})
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
set (GLSTRINGSRCS ${GLSTRINGSRCS} "${PROJECT_SOURCE_DIR}/src/utils/tga.cpp" "${PROJECT_SOURCE_DIR}/src/Texture.cpp")
add_executable (GLStringTest EXCLUDE_FROM_ALL ${GLSTRINGSRCS})
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
set (TOMBSRCS ${TOMBSRCS} "${PROJECT_SOURCE_DIR}/src/utils/tga.cpp")
add_executable (TombRaiderTest EXCLUDE_FROM_ALL ${TOMBSRCS})
set_target_properties (TombRaiderTest PROPERTIES COMPILE_FLAGS "${OpenRaider_CXX_FLAGS} ${TOMB_FLAGS}")
target_link_libraries (TombRaiderTest ${ZLIBS})

#################################################################

# Add test build target
add_custom_target (tests)
add_dependencies (tests GLStringTest MathTest MatrixTest SoundTest TGATest TombRaiderTest)

# Add test run target
add_custom_target (testRun echo "Running Unit Tests..."
    COMMAND TGATest
    COMMAND SoundTest
    COMMAND MatrixTest
    COMMAND MathTest
    COMMAND GLStringTest
)
add_dependencies (testRun tests)

# Add TombRaider regression test target
set (REGTEST_TR1 "~/.OpenRaider/paks/tr1/level1.phd")
set (REGTEST_TR2 "~/.OpenRaider/paks/tr2/unwater.tr2")
#set (REGTEST_TR3 "~/.OpenRaider/paks/tr3/scotland.tr2")
set (REGTEST_TR3 "~/.OpenRaider/paks/tr3/HOUSE.TR2")
set (REGTEST_TR4 "~/.OpenRaider/paks/tr4/angkor1.tr4")
#set (REGTEST_TR5 "~/.OpenRaider/paks/tr5/demo.trc")
set (REGTEST_TR5 "~/.OpenRaider/paks/tr5/andy1.trc")
add_custom_target (testReg echo "Running TombRaider 1 Regression Test..."
    COMMAND TombRaiderTest load ${REGTEST_TR1} > ../log.tr1
    COMMAND echo "Running TombRaider 2 Regression Test..."
    COMMAND TombRaiderTest load ${REGTEST_TR2} > ../log.tr2
    COMMAND echo "Running TombRaider 3 Regression Test..."
    COMMAND TombRaiderTest load ${REGTEST_TR3} > ../log.tr3
    COMMAND echo "Running TombRaider 4 Regression Test..."
    COMMAND TombRaiderTest load ${REGTEST_TR4} > ../log.tr4
    COMMAND echo "Running TombRaider 5 Regression Test..."
    COMMAND TombRaiderTest load ${REGTEST_TR5} > ../log.tr5
)
add_dependencies (testReg TombRaiderTest)

# Clean regression test logs
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
    "../log.tr1;../log.tr2;../log.tr3;../log.tr4;../log.tr5")
