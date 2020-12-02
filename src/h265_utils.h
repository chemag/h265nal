/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <vector>

#include "absl/types/optional.h"
#include "h265_bitstream_parser_state.h"
#include "h265_rtp_parser.h"

namespace h265nal {

// A class for unclassified utilities.
class H265Utils {
 public:
  // Get the slice QP for the Y component (Equation 7-54)
  static absl::optional<int32_t> GetSliceQpY(
      const H265RtpParser::RtpState rtp,
      const H265BitstreamParserState* bitstream_parser_state);
  static std::vector<int32_t> GetSliceQpY(
      const uint8_t* data, size_t length,
      H265BitstreamParserState* bitstream_parser_state);
};

}  // namespace h265nal
