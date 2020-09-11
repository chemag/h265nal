# h265nal: A Library and Tool to parse H265 NAL units

By [Chema Gonzalez](https://github.com/chemag), 2020-09-11


# Rationale
This document describes h265nal, a simpler H265 NAL unit parser.

Final goal it to create a binary that accepts a file in h265 Annex B format
(.265) and dumps the contents of the parsed NALs.


# Install Instructions

Get the git repo, and then build using cmake.

```
$ git clone https://github.com/chemag/h265nal
$ cd h265nal
$ mkdir build
$ cd build
$ cmake ..  # also cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
$ make
```

Feel free to test any of the unittests:

```
$ ./src/h265_profile_tier_level_parser_unittest
Running main() from /builddir/build/BUILD/googletest-release-1.8.1/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test case.
[----------] Global test environment set-up.
[----------] 1 test from H265ProfileTierLevelParserTest
[ RUN      ] H265ProfileTierLevelParserTest.TestSampleValue
[       OK ] H265ProfileTierLevelParserTest.TestSampleValue (0 ms)
[----------] 1 test from H265ProfileTierLevelParserTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test case ran. (0 ms total)
[  PASSED  ] 1 test.
```


# Examples

[todo]


# Requirements
Requires gtests, gmock, abseil.

The [webrtc](`webrtc`) directory contains an RBSP parser copied from webrtc.


# License
h265nal is BSD licensed, as found in the [LICENSE](LICENSE) file.

