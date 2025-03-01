cmake_minimum_required(VERSION 3.13)

# set the project name
project(slippy-map)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_CXX_STANDARD 23)

Include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-3.2.4
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
    GIT_TAG release-3.2.0
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(SDL3_image)

set(CURL_MINIMAL 1 CACHE INTERNAL "Minimal curl build")

# **Build static library**
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build libcurl as static library")
set(BUILD_STATIC_LIBS ON CACHE INTERNAL "Build static lib")
#set(CURL_STATICLIB ON CACHE BOOL "Force static linking")

# **Minimal Feature-Set**
set(CURL_USE_OPENSSL OFF CACHE INTERNAL "Disable OpenSSL")
set(CURL_USE_LIBSSH2 OFF CACHE INTERNAL "Disable SSH2")
set(CURL_USE_LIBIDN2 OFF CACHE INTERNAL "Disable IDN2")
set(CURL_USE_GSSAPI OFF CACHE INTERNAL "Disable GSSAPI")
set(CURL_USE_LIBPSL OFF CACHE INTERNAL "Disable libpsl support")
set(CMAKE_USE_LIBIDN2 OFF CACHE INTERNAL "Disable IDN2")
set(CMAKE_USE_ARES OFF CACHE INTERNAL "Disable c-ares")
set(HTTP_ONLY ON CACHE INTERNAL "Only enable HTTP support")
set(CURL_DISABLE_LDAP ON CACHE INTERNAL "Disable LDAP")
set(CURL_DISABLE_TFTP ON CACHE INTERNAL "Disable TFTP")
set(CURL_DISABLE_FTP ON CACHE INTERNAL "Disable FTP")
set(CURL_DISABLE_FILE ON CACHE INTERNAL "Disable FILE protocol")
set(CURL_DISABLE_DICT ON CACHE INTERNAL "Disable DICT protocol")
set(CURL_DISABLE_GOPHER ON CACHE INTERNAL "Disable GOPHER protocol")
set(CURL_DISABLE_IMAP ON CACHE INTERNAL "Disable IMAP protocol")
set(CURL_DISABLE_POP3 ON CACHE INTERNAL "Disable POP3 protocol")
set(CURL_DISABLE_RTSP ON CACHE INTERNAL "Disable RTSP protocol")
set(CURL_DISABLE_SMB ON CACHE INTERNAL "Disable SMB protocol")
set(CURL_DISABLE_SMTP ON CACHE INTERNAL "Disable SMTP protocol")
set(CURL_DISABLE_TELNET ON CACHE INTERNAL "Disable TELNET protocol")
set(CURL_DISABLE_LDAP ON CACHE INTERNAL "Disable LDAP protocol")
set(CURL_DISABLE_HSTS ON CACHE INTERNAL "Disable HSTS support")

# **Skip the rest too**
set(BUILD_CURL_EXE OFF CACHE INTERNAL "Do not build curl CLI tool")
set(BUILD_TESTING OFF CACHE INTERNAL "Disable tests")
set(BUILD_EXAMPLES OFF CACHE INTERNAL "Disable examples")
set(BUILD_LIBCURL_DOCS OFF CACHE INTERNAL "Disable docs")
set(BUILD_MISC_DOCS OFF CACHE INTERNAL "Disable docs")

FetchContent_Declare(
    CURL
    GIT_REPOSITORY https://github.com/curl/curl
    # This will be a tag once SDL3 is released
    GIT_TAG curl-8_12_1
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(CURL)

# add the executable
add_executable(slippy-map)

add_subdirectory(src)

if(SLIPPY_MAP_BUILD_TESTS)
  add_subdirectory(tests)
  include(CTest)
endif()

if(MSVC)
  target_compile_options(slippy-map PRIVATE /W4 /WX)
else()
  target_compile_options(slippy-map PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(slippy-map PRIVATE CURL::libcurl SDL3_image::SDL3_image SDL3::SDL3)
