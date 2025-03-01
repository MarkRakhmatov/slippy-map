cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

get_filename_component(ROOT_DIR ../ ABSOLUTE)

get_filename_component(PARENT_DIR ../ ABSOLUTE)

# file(GLOB test-sources CONFIGURE_DEPENDS ${ROOT_DIR}/src/*.cpp)

# These tests can use the Catch2-provided main
add_executable(tests Utils_tests.cpp MapModel_tests.cpp
${ROOT_DIR}/src/tiles/Utils.cpp
${ROOT_DIR}/src/tiles/MapModel.cpp
)

include_directories(${ROOT_DIR}/src)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)
