/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <vector>

#include "absl/types/optional.h"
#include "h265_rtp_ap_parser.h"
#include "h265_rtp_fu_parser.h"
#include "h265_rtp_single_parser.h"
#include "rtc_base/bit_buffer.h"

namespace h265nal {

// A class for parsing out an RTP NAL Unit data.
class H265RtpParser {
 public:
  // The parsed state of the RTP NAL Unit.
  struct RtpState {
    RtpState() = default;
    RtpState(const RtpState&) = default;
    ~RtpState() = default;
#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    struct H265NalUnitHeaderParser::NalUnitHeaderState nal_unit_header;
    struct H265RtpSingleParser::RtpSingleState rtp_single;
    struct H265RtpApParser::RtpApState rtp_ap;
    struct H265RtpFuParser::RtpFuState rtp_fu;
  };

  // Unpack RBSP and parse RTP NAL Unit state from the supplied buffer.
  static absl::optional<RtpState> ParseRtp(
      const uint8_t* data, size_t length,
      struct H265BitstreamParserState* bitstream_parser_state) noexcept;
  static absl::optional<RtpState> ParseRtp(
      rtc::BitBuffer* bit_buffer,
      struct H265BitstreamParserState* bitstream_parser_state) noexcept;
};

}  // namespace h265nal
