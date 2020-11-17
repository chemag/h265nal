/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */


#include "h265_utils.h"

#include <stdio.h>

#include <cstdint>
#include <vector>

#include "h265_bitstream_parser_state.h"
#include "h265_common.h"
#include "h265_rtp_parser.h"
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
    const H265RtpParser::RtpState rtp,
    const H265BitstreamParserState* bitstream_parser_state) {
  const struct H265NalUnitHeaderParser::NalUnitHeaderState* header;
  const struct H265NalUnitPayloadParser::NalUnitPayloadState* payload;

  // get the actual NAL header (not the RTP one)
  uint32_t nal_unit_type;
  if (rtp.nal_unit_header.nal_unit_type <= 47) {
    header = &(rtp.rtp_single.nal_unit_header);
    nal_unit_type = header->nal_unit_type;
    payload = &(rtp.rtp_single.nal_unit_payload);
  } else if (rtp.nal_unit_header.nal_unit_type == AP) {
    // use the latest NAL unit in the AP
    size_t index = rtp.rtp_ap.nal_unit_headers.size() - 1;
    header = &(rtp.rtp_ap.nal_unit_headers[index]);
    nal_unit_type = header->nal_unit_type;
    payload = &(rtp.rtp_ap.nal_unit_payloads[index]);
  } else if (rtp.nal_unit_header.nal_unit_type == FU) {
    // check this is the first NAL of the frame
    if (rtp.rtp_fu.s_bit == 0) {
      return absl::nullopt;
    }
    nal_unit_type = rtp.rtp_fu.fu_type;
    payload = &(rtp.rtp_fu.nal_unit_payload);
  }

  // make sure the RTP packet contains a slice header with an IDR frame
  if ((nal_unit_type != NalUnitType::IDR_W_RADL) &&
      (nal_unit_type != NalUnitType::IDR_N_LP)) {
    return absl::nullopt;
  }

  // check some values
  auto &slice_header = payload->slice_segment_layer.slice_segment_header;
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
