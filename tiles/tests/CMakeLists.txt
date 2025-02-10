cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)

get_filename_component(SOLUTION_DIR ../../ ABSOLUTE)

get_filename_component(PARENT_DIR ../ ABSOLUTE)

add_subdirectory(${SOLUTION_DIR}/external/Catch2 ${PARENT_DIR}/tests/build/external EXCLUDE_FROM_ALL )

# These tests can use the Catch2-provided main
add_executable(tests Utils_tests.cpp ${PARENT_DIR}/src/Utils.cpp)
include_directories(${PARENT_DIR}/inc)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain )
