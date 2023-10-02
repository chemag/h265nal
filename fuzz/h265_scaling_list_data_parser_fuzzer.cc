/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h265_scaling_list_data_parser_unittest.cc.
// Do not edit directly.

#include "h265_scaling_list_data_parser.h"
#include "h265_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  {
  auto scaling_list_data = h265nal::H265ScalingListDataParser::ParseScalingListData(
      data, size);
  }
  return 0;
}
