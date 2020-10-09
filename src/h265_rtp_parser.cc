/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */


#include "h265_rtp_parser.h"

#include <stdio.h>

#include <cstdint>
#include <vector>

#include "h265_common.h"
#include "h265_nal_unit_parser.h"
#include "absl/types/optional.h"
#include "rtc_base/bit_buffer.h"

namespace {
typedef absl::optional<h265nal::H265NalUnitHeaderParser::
    NalUnitHeaderState> OptionalNalUnitHeader;
typedef absl::optional<h265nal::H265NalUnitPayloadParser::
    NalUnitPayloadState> OptionalNalUnitPayload;
typedef absl::optional<h265nal::H265RtpParser::
    RtpState> OptionalRtp;
typedef absl::optional<h265nal::H265RtpSingleParser::
    RtpSingleState> OptionalRtpSingle;
}  // namespace

namespace h265nal {

// General note: this is based off rfc7798.
// You can find it on this page:
// https://tools.ietf.org/html/rfc7798#section-4.4.1

// Unpack RBSP and parse RTP NAL Unit state from the supplied buffer.
absl::optional<H265RtpParser::RtpState>
H265RtpParser::ParseRtp(
    const uint8_t* data, size_t length,
    struct H265BitstreamParserState* bitstream_parser_state) {

  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseRtp(&bit_buffer, bitstream_parser_state);
}

absl::optional<H265RtpParser::RtpState>
H265RtpParser::ParseRtp(
    rtc::BitBuffer* bit_buffer,
    struct H265BitstreamParserState* bitstream_parser_state) {
  // H265 RTP NAL Unit pseudo-NAL Unit.
  RtpState rtp;

  // peek a pseudo-nal_unit_header
  OptionalNalUnitHeader nal_unit_header =
      H265NalUnitHeaderParser::ParseNalUnitHeader(bit_buffer);
  if (nal_unit_header == absl::nullopt) {
    return absl::nullopt;
  }
  bit_buffer->Seek(0, 0);

  if (nal_unit_header->nal_unit_type <= 47) {
    // rtp_single()
    OptionalRtpSingle rtp_single =
        H265RtpSingleParser::ParseRtpSingle(
            bit_buffer,
            bitstream_parser_state);
    if (rtp_single != absl::nullopt) {
      rtp.rtp_single = *rtp_single;
    }
  }

  return OptionalRtp(rtp);
}

void H265RtpParser::RtpState::fdump(
    FILE* outfp, int indent_level, uint32_t nal_unit_type) const {
  fprintf(outfp, "rtp {");
  indent_level = indent_level_incr(indent_level);

  if (nal_unit_type <= 47) {
    // rtp_single()
    rtp_single.fdump(outfp, indent_level);
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

}  // namespace h265nal
