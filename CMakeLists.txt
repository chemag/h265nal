# CMakeLists files in this project can
# refer to the root source directory of the project as ${MAIN_SOURCE_DIR} and
# to the root binary directory of the project as ${MAIN_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (h265nal)

# Abseil requires C++11
set(CMAKE_CXX_STANDARD 11)

enable_testing()

# Recurse into "src" and "webrtc" subdirectories.
add_subdirectory (src)
add_subdirectory (webrtc)
