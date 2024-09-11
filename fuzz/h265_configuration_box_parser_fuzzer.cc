/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h265_configuration_box_parser_unittest.cc.
// Do not edit directly.

#include "h265_configuration_box_parser.h"
#include "h265_common.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  {
  h265nal::H265BitstreamParserState bitstream_parser_state;
  h265nal::ParsingOptions parsing_options;
  auto configuration_box = h265nal::H265ConfigurationBoxParser::ParseConfigurationBox(
      data, size, &bitstream_parser_state, parsing_options);
  }
  return 0;
}
