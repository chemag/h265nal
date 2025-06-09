# CMakeLists files in this project can
# refer to the root source directory of the project as ${MAIN_SOURCE_DIR} and
# to the root binary directory of the project as ${MAIN_BINARY_DIR}.
cmake_minimum_required(VERSION 3.10)

# https://cmake.org/cmake/help/v3.0/command/project.html
project(h265nal VERSION 0.47)

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


# Recurse into source code subdirectories.
add_subdirectory(src)
add_subdirectory(tools)
option(BUILD_TESTS "Build h265 gtests" ON)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

# Ensure CMake honors normal variable values when calling option()
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()
option(BUILD_CLANG_FUZZER "Build clang fuzzer sanitizer targets" ON)

if(BUILD_CLANG_FUZZER)
  add_subdirectory(fuzz)
endif()
