/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h265_vui_parameters_parser_unittest.cc.
// Do not edit directly.

#include "h265_vui_parameters_parser.h"
#include "h265_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  auto vui_parameters =
      h265nal::H265VuiParametersParser::ParseVuiParameters(data, size);
  return 0;
}
