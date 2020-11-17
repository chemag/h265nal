/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */


#include "h265_utils.h"

#include <stdio.h>

#include <cstdint>
#include <vector>

#include "h265_bitstream_parser_state.h"
#include "h265_common.h"
#include "h265_rtp_single_parser.h"
#include "absl/types/optional.h"

namespace h265nal {

namespace {
typedef absl::optional<int32_t> OptionalInt32_t;
}  // namespace


// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Calculate Luminance Slice QP values from slice header and PPS.
absl::optional<int32_t>
H265Utils::GetSliceQpY(
    const H265RtpSingleParser::RtpSingleState rtp_single,
    const H265BitstreamParserState* bitstream_parser_state) {

  // make sure the RTP packet contains a slice header with an IDR frame
  auto &header = rtp_single.nal_unit_header;
  if ((header.nal_unit_type != NalUnitType::IDR_W_RADL) &&
      (header.nal_unit_type != NalUnitType::IDR_N_LP)) {
    return absl::nullopt;
  }

  // check some values
  auto &payload = rtp_single.nal_unit_payload;
  auto &slice_header = payload.slice_segment_layer.slice_segment_header;
  auto pps_id = slice_header.slice_pic_parameter_set_id;
  auto slice_qp_delta = slice_header.slice_qp_delta;

  // check the PPS exists in the bitstream parser state
  auto pps = bitstream_parser_state->GetPps(pps_id);
  if (pps == absl::nullopt) {
    return absl::nullopt;
  }
  const auto init_qp_minus26 = pps->init_qp_minus26;

  // Equation 7-54, Section 7.4.7.1
  int32_t slice_qpy = 26 + init_qp_minus26 + slice_qp_delta;

  return OptionalInt32_t(slice_qpy);
}

}  // namespace h265nal
