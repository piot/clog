cmake_minimum_required(VERSION 3.16.3)
project(clog C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_VERBOSE_MAKEFILE on)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(isDebug TRUE)
else()
  set(isDebug FALSE)
endif()

message("isDebug: ${isDebug}")

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(COMPILER_NAME "clang")
  set(COMPILER_CLANG 1)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set(COMPILER_NAME "gcc")
  set(COMPILER_GCC 1)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  set(COMPILER_NAME "msvc")
  set(COMPILER_MSVC 1)
endif()

set(useSanitizers ${isDebug})
message("useSanitizers: ${useSanitizers}")

if(useSanitizers)
  message("using sanitizers")
  set(sanitizers "-fsanitize=address")
endif()

if(APPLE)
  add_compile_definitions(TORNADO_OS_MACOS)
  set(OS_MACOS 1)
elseif(UNIX)
  add_compile_definitions(TORNADO_OS_LINUX)
  set(OS_LINUX 1)
elseif(WIN32)
  add_compile_definitions(TORNADO_OS_WINDOWS)
  set(OS_WINDOWS 1)
endif()

if(isDebug)
  message("Debug build detected")
  set(CONFIGURATION_DEBUG 1)
endif()

file(GLOB_RECURSE lib_src FOLLOW_SYMLINKS "lib/*.c")

add_library(clog ${lib_src})

if(COMPILER_CLANG)
  target_compile_options(
    clog
    PRIVATE -Weverything -Werror -Wno-padded
            -Wno-declaration-after-statement # must be bug in clang? shows up
                                             # even if std=c99
  )
elseif(COMPILER_MSVC)
  target_compile_options(
    clog
    PRIVATE /F
            5000000
            /Wall
            /wd4668
            /wd4820
            /wd4100
            /wd4710
            /wd4711
            /wd5945
            /wd5105
            /wd5045
            /we4047
            /we4255
            /we4013
            /we4020
            /we4474
            /we4477
            /we4057
            /we4101
            /we4189) # /WX
else()
  target_compile_options(clog PRIVATE -Wall)
endif()

set(deps ../deps/)

target_include_directories(clog PUBLIC include)
target_include_directories(clog PUBLIC ${deps}/piot/tiny-libc/src/include)

function(unixlike)
  message("unixlike")
  target_link_libraries(clog m)
endfunction()

if(OS_LINUX)
  message("Linux Detected!")
  unixlike()

elseif(OS_MACOS)
  message("MacOS detected!")
  unixlike()
endif()

set_target_properties(clog PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                      "${CMAKE_BINARY_DIR}/bin")

if(useSanitizers)
  target_link_libraries(clog ${sanitizers})
endif()

target_compile_options(clog PRIVATE ${sanitizers})

if(NOT isDebug)
  message("optimize!")
  target_compile_options(clog PRIVATE -O3) # -flto file format no recognized
endif()
