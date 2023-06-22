cmake_minimum_required(VERSION 3.16.3)
project(clog C)

set(CMAKE_C_STANDARD 99)
# set(CMAKE_C_COMPILER_NAMES clang)
set(CMAKE_VERBOSE_MAKEFILE on)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(isDebug TRUE)
else()
  set(isDebug FALSE)
endif()

message("isDebug: ${isDebug}")

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

# target_compile_definitions(clog PUBLIC _POSIX_C_SOURCE=200112L)
add_compile_options(
  clog
  PUBLIC
  -Wall
  -Wextra
  -Wshadow
  -Wstrict-aliasing
  -ansi
  -pedantic
  -Wno-declaration-after-statement
  -Wno-extra-semi-stmt
  -Wno-undef
  -Wno-unused-variable
  -Wno-unused-parameter
  -Wno-padded
  -Werror=implicit-function-declaration
  -Werror=incompatible-pointer-types
  -Werror=missing-prototypes
  -Werror=int-conversion
  -Werror=return-type
  -Werror=incompatible-function-pointer-types)

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
