/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <vector>

#include "h265_bitstream_parser_state.h"
#include "h265_rtp_single_parser.h"
#include "absl/types/optional.h"

namespace h265nal {

// A class for unclassified utilities.
class H265Utils {
 public:
  // Get the slice QP for the Y component (Equation 7-54)
  static absl::optional<int32_t> GetSliceQpY(
      const H265RtpSingleParser::RtpSingleState rtp_single,
      const H265BitstreamParserState* bitstream_parser_state);
};

}  // namespace h265nal
