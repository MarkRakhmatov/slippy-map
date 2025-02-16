cmake_minimum_required(VERSION 3.10)

# set the project name
project(slippy-map)

set(CMAKE_CXX_STANDARD 23)

# add the executable
add_executable(slippy-map src/main.cpp)

if(MSVC)
  target_compile_options(slippy-map PRIVATE /W4 /WX)
else()
  target_compile_options(slippy-map PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
