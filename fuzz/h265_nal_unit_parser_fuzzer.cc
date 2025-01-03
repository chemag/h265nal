/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h265_nal_unit_parser_unittest.cc.
// Do not edit directly.

#include "h265_nal_unit_parser.h"
#include "h265_bitstream_parser_state.h"
#include "h265_common.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  {
  h265nal::H265BitstreamParserState bitstream_parser_state;
  h265nal::ParsingOptions parsing_options;
  parsing_options.add_checksum = true;
  auto nal_unit = h265nal::H265NalUnitParser::ParseNalUnit(data, size,
                                                  &bitstream_parser_state,
                                                  parsing_options);
  }
  return 0;
}
