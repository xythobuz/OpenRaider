set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenRaider_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenRaider_CXX_FLAGS} ${OpenRaider_CXX_FLAGS_RELEASE}")

add_custom_target (check COMMAND ${CMAKE_CTEST_COMMAND} -V)

#################################################################

add_executable (tester_binary EXCLUDE_FROM_ALL
    "binary.cpp" "../utils/binary.cpp"
)
add_dependencies (check tester_binary)
add_test (NAME test_binary COMMAND tester_binary)

#################################################################

