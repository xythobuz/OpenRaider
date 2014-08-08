set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenRaider_CXX_FLAGS} -DUNIT_TEST")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenRaider_CXX_FLAGS_DEBUG} -DUNIT_TEST")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenRaider_CXX_FLAGS_RELEASE} -DUNIT_TEST")

add_custom_target (check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

#################################################################

add_executable (tester_binary EXCLUDE_FROM_ALL
    "binary.cpp" "../src/utils/binary.cpp" "../src/main.cpp"
)
add_dependencies (check tester_binary)
add_test (NAME test_binary COMMAND tester_binary)

#################################################################

add_executable (tester_folder EXCLUDE_FROM_ALL
    "Folder.cpp" "../src/utils/filesystem.cpp"
    "../src/utils/Folder.cpp" "../src/utils/File.cpp"
    "../src/Exception.cpp" "../src/main.cpp"
)

#add_dependencies (check tester_folder)
#add_test (NAME test_folder COMMAND tester_folder)

#################################################################

add_executable (tester_script EXCLUDE_FROM_ALL
    "Script.cpp" "../src/Script.cpp" "../src/main.cpp"
    "../src/utils/binary.cpp" "../src/utils/strings.cpp"
)

#add_dependencies (check tester_script)
#add_test (NAME test_script COMMAND tester_script)

#################################################################

