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

Feel free to test all the unittests:

```
$ make test
Running tests...
Test project h265nal/build
      Start  1: h265_profile_tier_level_parser_unittest
 1/10 Test  #1: h265_profile_tier_level_parser_unittest ...   Passed    0.00 sec
      Start  2: h265_vps_parser_unittest
 2/10 Test  #2: h265_vps_parser_unittest ..................   Passed    0.00 sec
      Start  3: h265_vui_parameters_parser_unittest
 3/10 Test  #3: h265_vui_parameters_parser_unittest .......   Passed    0.00 sec
      Start  4: h265_st_ref_pic_set_parser_unittest
 4/10 Test  #4: h265_st_ref_pic_set_parser_unittest .......   Passed    0.00 sec
      Start  5: h265_sps_parser_unittest
 5/10 Test  #5: h265_sps_parser_unittest ..................   Passed    0.00 sec
      Start  6: h265_pps_parser_unittest
 6/10 Test  #6: h265_pps_parser_unittest ..................   Passed    0.00 sec
      Start  7: h265_aud_parser_unittest
 7/10 Test  #7: h265_aud_parser_unittest ..................   Passed    0.00 sec
      Start  8: h265_slice_parser_unittest
 8/10 Test  #8: h265_slice_parser_unittest ................   Passed    0.00 sec
      Start  9: h265_bitstream_parser_unittest
 9/10 Test  #9: h265_bitstream_parser_unittest ............   Passed    0.00 sec
      Start 10: h265_nal_unit_parser_unittest
10/10 Test #10: h265_nal_unit_parser_unittest .............   Passed    0.00 sec

100% tests passed, 0 tests failed out of 10

Total Test time (real) =   0.04 sec
```

Or to test any of the unittests:

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


# CLI Binary Operation

Parse all the NAL units of an Annex B (`.265` extension) file.

```
$ ./src/h265nal file.265 --noas-one-line --add-length --add-offset
nal_unit {
  offset: 0x00000004
  length: 23
  nal_unit_header {
    forbidden_zero_bit: 0
    nal_unit_type: 32
    nuh_layer_id: 0
    nuh_temporal_id_plus1: 1
  }
  vps {
    vps_video_parameter_set_id: 0
    vps_base_layer_internal_flag: 1
    vps_base_layer_available_flag: 1
    vps_max_layers_minus1: 0
    ...
```


# Programmatic Integration Operation

In order to integrate the parser in your C++ parser, take a look at
`H265BitstreamParser::ParseBitstream()`. The function gets a generic
binary string (`data` and `length`), (1) splits it into a vector of
NAL units, (2) parses each of the NAL units, and (3) adds the parsed
NAL unit to a vector of parser NAL units. If the parsed NAL unit has
state that needs to be used to parse other NAL units (VPS, SPS, PPS),
then it (4) stores the state into a `BitstreamParserState` object.


# Requirements
Requires gtests, gmock, abseil.

The [webrtc](`webrtc`) directory contains an RBSP parser copied from webrtc.


# TODO

List of tasks:
* add RTP H265 support (rfc7798)
  * we should at least support single NAL unit, aggregation, and fragmentation
    unit packets (not sure about paci)
* add lacking parsers (e.g. SEI)
* remove TODO entries from the code


# License
h265nal is BSD licensed, as found in the [LICENSE](LICENSE) file.

