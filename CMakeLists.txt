cmake_minimum_required(VERSION 3.10)

# set the project name
project(slippy-map)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_CXX_STANDARD 23)

Include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    # This will be a tag once SDL3 is released
    GIT_TAG origin/main
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
    # This will be a tag once SDL3 is released
    GIT_TAG origin/main
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(SDL3_image)

# add the executable
add_executable(slippy-map src/main.cpp)

if(MSVC)
  target_compile_options(slippy-map PRIVATE /W4 /WX)
else()
  target_compile_options(slippy-map PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(slippy-map PRIVATE SDL3_image::SDL3_image SDL3::SDL3)
