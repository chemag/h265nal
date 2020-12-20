/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h265_st_ref_pic_set_parser_unittest.cc.
// Do not edit directly.

#include "h265_st_ref_pic_set_parser.h"
#include "h265_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  auto st_ref_pic_set =
      h265nal::H265StRefPicSetParser::ParseStRefPicSet(data, size, 0, 1);
  return 0;
}
