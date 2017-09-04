cmake_minimum_required(VERSION 3.5.1)

PROJECT("Pixel Shader")

#Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)
add_subdirectory(googletest)

enable_testing()
add_subdirectory(test)


SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -p -O0 -Wall -pedantic -Werror ")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG")

OPTION(ASAN "Use adresss sanitizer" OFF)
IF (ASAN)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
ENDIF()
