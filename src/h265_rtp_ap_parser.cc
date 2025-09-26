/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_rtp_ap_parser.h"

#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h265_bitstream_parser_state.h"
#include "h265_common.h"
#include "h265_nal_unit_parser.h"
#include "rtc_common.h"

namespace h265nal {

// General note: this is based off rfc7798/
// You can find it on this page:
// https://tools.ietf.org/html/rfc7798#section-4.4.2

// Unpack RBSP and parse RTP AP state from the supplied buffer.
std::unique_ptr<H265RtpApParser::RtpApState> H265RtpApParser::ParseRtpAp(
    const uint8_t* data, size_t length,
    struct H265BitstreamParserState* bitstream_parser_state) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseRtpAp(&bit_buffer, bitstream_parser_state);
}

std::unique_ptr<H265RtpApParser::RtpApState> H265RtpApParser::ParseRtpAp(
    BitBuffer* bit_buffer,
    struct H265BitstreamParserState* bitstream_parser_state) noexcept {
  // H265 RTP AP pseudo-NAL Unit.
  auto rtp_ap = std::make_unique<RtpApState>();

  // first read the common header
  rtp_ap->header = H265NalUnitHeaderParser::ParseNalUnitHeader(bit_buffer);
  if (rtp_ap->header == nullptr) {
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: cannot ParseNalUnitHeader in rtp ap\n");
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  while (bit_buffer->RemainingBitCount() > 0) {
    // NALU size
    uint32_t nalu_size;
    if (!bit_buffer->ReadBits(16, nalu_size)) {
      return nullptr;
    }
    rtp_ap->nal_unit_sizes.push_back(nalu_size);

    // NALU header
    rtp_ap->nal_unit_headers.push_back(
        H265NalUnitHeaderParser::ParseNalUnitHeader(bit_buffer));
    if (rtp_ap->nal_unit_headers.back() == nullptr) {
#ifdef FPRINT_ERRORS
      fprintf(stderr, "error: cannot ParseNalUnitHeader in rtp ap\n");
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // NALU payload
    rtp_ap->nal_unit_payloads.push_back(
        H265NalUnitPayloadParser::ParseNalUnitPayload(
            bit_buffer, rtp_ap->nal_unit_headers.back()->nal_unit_type,
            bitstream_parser_state));
    if (rtp_ap->nal_unit_payloads.back() == nullptr) {
      return nullptr;
    }
  }
  return rtp_ap;
}

#ifdef FDUMP_DEFINE
void H265RtpApParser::RtpApState::fdump(FILE* outfp, int indent_level,
                                        ParsingOptions parsing_options) const {
  fprintf(outfp, "rtp_ap {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  header->fdump(outfp, indent_level);

  for (unsigned int i = 0; i < nal_unit_sizes.size(); ++i) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "nal_unit_size: %zu", nal_unit_sizes[i]);

    fdump_indent_level(outfp, indent_level);
    nal_unit_headers[i]->fdump(outfp, indent_level);

    fdump_indent_level(outfp, indent_level);
    nal_unit_payloads[i]->fdump(outfp, indent_level,
                                nal_unit_headers[i]->nal_unit_type, parsing_options);
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
