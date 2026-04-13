# CMakeLists files in this project can
# refer to the root source directory of the project as ${MAIN_SOURCE_DIR} and
# to the root binary directory of the project as ${MAIN_BINARY_DIR}.
cmake_minimum_required(VERSION 3.10)

# https://cmake.org/cmake/help/v3.0/command/project.html
project(h265nal VERSION 0.54)

# std::make_ptr requires C++14
set(CMAKE_CXX_STANDARD 14)

# set up clang-tidy
set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-format-style='google'")
if(WIN32)
  # Designated initializers throughout project requires C++20 on windows
  set(CMAKE_CXX_STANDARD 20)
  if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
  endif()
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

# Set debug flags for both C and C++
set(COMMON_DEBUG_FLAGS "-g -O0 -Wall -Wextra -Wunused-parameter -Wshadow -Wformat -Wextra-semi -Wsign-conversion -Werror")
# Only Clang (incl. AppleClang) has -Wextra-semi-stmt
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(COMMON_DEBUG_FLAGS "${COMMON_DEBUG_FLAGS} -Wextra-semi-stmt")
endif()
set(CMAKE_C_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")

# Ensure CMake honors normal variable values when calling option()
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

# Recurse into source code subdirectories.
add_subdirectory(src)
add_subdirectory(tools)
option(BUILD_H265_TESTS "Build h265 gtests" ON)
if(BUILD_H265_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

option(BUILD_CLANG_FUZZER "Build clang fuzzer sanitizer targets" OFF)
if(NOT DEFINED BUILD_CLANG_FUZZER_SET_BY_USER)
  # Auto-enable fuzzer when using Clang
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(BUILD_CLANG_FUZZER ON)
  endif()
endif()

if(BUILD_CLANG_FUZZER)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "BUILD_CLANG_FUZZER requires Clang (found ${CMAKE_CXX_COMPILER_ID})")
  endif()
  add_subdirectory(fuzz)
endif()

# install targets
include(GNUInstallDirs)
install(TARGETS h265nal
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(TARGETS h265nal-bin
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/h265nal
    FILES_MATCHING PATTERN "*.h"
)
