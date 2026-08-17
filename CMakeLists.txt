# CMakeLists files in this project can
# refer to the root source directory of the project as ${MAIN_SOURCE_DIR} and
# to the root binary directory of the project as ${MAIN_BINARY_DIR}.
cmake_minimum_required(VERSION 3.10)

# https://cmake.org/cmake/help/v3.0/command/project.html
project(h265nal VERSION 0.55)

# std::make_ptr requires C++14
set(CMAKE_CXX_STANDARD 14)

# clang-tidy: run as "make tidy" (not part of the normal build)
#
# The checks come from .clang-tidy in the source root, not from a -checks
# argument here, so that an editor reading the same file reports the same
# findings. Without either, clang-tidy enables nothing and exits with
# "Error: no checks enabled", which is what this target used to do.
#
# Only the .cc files are listed. Headers are analysed as part of the
# translation units that include them, selected by HeaderFilterRegex;
# passing a header directly has no entry in the compilation database.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
  file(GLOB_RECURSE ALL_CXX_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cc
  )
  add_custom_target(tidy
    COMMAND ${CLANG_TIDY_EXE}
      -p ${CMAKE_BINARY_DIR}
      -quiet
      ${ALL_CXX_SOURCES}
    COMMENT "Running clang-tidy..."
    VERBATIM
  )
endif()
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

option(BUILD_CLANG_FUZZER "Build clang fuzzer sanitizer targets" OFF)

if(BUILD_CLANG_FUZZER)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "BUILD_CLANG_FUZZER requires Clang (found ${CMAKE_CXX_COMPILER_ID})")
  endif()
  # These have to be set here, before add_subdirectory(src):
  # add_compile_options() only reaches the targets of the directory it is
  # called from and of the subdirectories added after it, so setting them
  # in fuzz/ leaves the library (built in src/) with no sanitizer and no
  # coverage instrumentation. The fuzzers then link an uninstrumented
  # libh265nal: libFuzzer gets no feedback from the parsers and mutates
  # blind, and a memory error inside a parser goes unreported.
  #
  # What each piece buys, for a parser walking attacker-controlled bits:
  #
  # address
  #     AddressSanitizer: reads off the end of a NAL unit, use after free
  #     of a parser state.
  # undefined
  #     The UBSan default group: signed overflow, oversized shift counts,
  #     misaligned and null pointer use, out of range enum and bool loads,
  #     and conversions of a non-representable float.
  # unsigned-integer-overflow
  #     Not in that group, and not undefined behaviour: unsigned
  #     wraparound is well defined. It is still a bug when a length or an
  #     offset computed from the bitstream wraps instead of being
  #     rejected.
  # implicit-conversion
  #     Also well defined, also usually wrong here: a 32 bit field
  #     silently narrowed to 8 or 16 bits loses the value the bitstream
  #     asked for. A truncation that is deliberate is written as an
  #     explicit static_cast, which this check ignores by design.
  # unsigned-shift-base
  #     In neither group. Catches a left shift that overflows an unsigned
  #     type, which is this parser's shape exactly: the bit buffer masks
  #     with 0xFF << shift and 1 << bit_count, where the shift comes from
  #     a width the bitstream chose.
  # local-bounds
  #     Out of range indexing of a local array that array-bounds cannot
  #     rule out statically. This one cannot be made recoverable: it
  #     traps, so it dies on SIGILL with no diagnostic. A libFuzzer
  #     artifact with no message at all is likely to be this.
  # float-divide-by-zero
  #     Well defined too, it yields an infinity, but in a framerate or
  #     aspect ratio it means a zero denominator was read and not checked.
  # fuzzer-no-link
  #     Coverage instrumentation for the library without libFuzzer's
  #     main(); the fuzz targets link that themselves.
  #
  # Recoverable or fatal. UBSan defaults to recoverable: it prints to
  # stderr and lets the process continue, so libFuzzer never sees a crash
  # and writes no artifact. no-sanitize-recover puts the undefined group
  # at parity with ASan, which aborts.
  #
  # The three named in -fsanitize-recover stay recoverable, so that a
  # noisy one cannot end a campaign. float-divide-by-zero is recoverable
  # as well, without being named: it is outside the undefined group, so
  # the -fno- above never reaches it. local-bounds is the exception that
  # cannot be either, as noted above. Reports are clean today, so
  # promoting the recoverable ones to fatal is a reasonable next step and
  # a separate change. Order matters, the later -fsanitize-recover wins
  # over the earlier -fno-.
  set(H265NAL_SAN_FLAGS
      -fsanitize=address,undefined,unsigned-integer-overflow,implicit-conversion,unsigned-shift-base,local-bounds,float-divide-by-zero
      -fno-sanitize-recover=undefined
      -fsanitize-recover=unsigned-integer-overflow,implicit-conversion,unsigned-shift-base)
  add_compile_options(${H265NAL_SAN_FLAGS} -fsanitize=fuzzer-no-link)
  add_link_options(${H265NAL_SAN_FLAGS})
endif()

# Recurse into source code subdirectories.
add_subdirectory(src)
add_subdirectory(tools)
option(BUILD_H265_TESTS "Build h265 gtests" ON)
if(BUILD_H265_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

if(BUILD_CLANG_FUZZER)
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
