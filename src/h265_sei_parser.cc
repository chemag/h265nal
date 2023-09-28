/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_sei_parser.h"

#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h265_common.h"

namespace h265nal {

// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Unpack RBSP and parse SEI state from the supplied buffer.
std::unique_ptr<H265SeiParser::SeiState> H265SeiParser::ParseSei(
    const uint8_t* data, size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseSei(&bit_buffer);
}

std::unique_ptr<H265SeiParser::SeiState> H265SeiParser::ParseSei(
    rtc::BitBuffer* bit_buffer) noexcept {
  // H265 SEI NAL Unit (access_unit_delimiter_rbsp()) parser.
  // Section 7.3.5 ("Supplemental enhancement information message syntax") of the H.265
  // standard for a complete description.

  uint32_t payload_type = 0;
  uint32_t ff_byte = 0xff;
  
  while(ff_byte == 0xff) {
    bit_buffer->ReadBits(8, ff_byte); // f(8)
    payload_type += ff_byte;
  }

  std::unique_ptr<SeiState> sei = nullptr;
  if(static_cast<SeiType>(payload_type) == SeiType::user_data_registered_itu_t_t35) {
    sei = std::make_unique<SeiStateUserData>();
  } else {
    sei = std::make_unique<SeiStateNotImplemented>();
  }
  sei->payload_type = static_cast<SeiType>(payload_type);

  ff_byte = 0xff;
  while(ff_byte == 0xff) {
    bit_buffer->ReadBits(8, ff_byte); // f(8)
    sei->payload_size += ff_byte;
  }

  // now read the payload:
  sei->payload.resize(sei->payload_size);
  for (size_t i = 0; i < sei->payload_size; ++i) {
    bit_buffer->ReadUInt8(sei->payload[i]);
  }

  // call parse payload to allow the SeiState to perform parsing specific to the Sei type
  sei->parse_payload();
  
  return sei;
}

void H265SeiParser::SeiStateUserData::parse_payload() {
  if (payload.size() < 2) {
    return;
  }
  userdata_payload_start_index = 1;
  itu_t_t35_country_code  = payload[0];
  if (itu_t_t35_country_code == 0xff) {
    userdata_payload_start_index = 2;
    itu_t_t35_country_code_extension_byte = payload[1];
  }
}

const uint8_t* H265SeiParser::SeiStateUserData::get_userdata_payload(size_t& payload_size) const {
  payload_size = payload.size() - userdata_payload_start_index;
  return &(payload[userdata_payload_start_index]);
}

void H265SeiParser::SeiStateNotImplemented::parse_payload() {
  // No-op
  return;  
}

#ifdef FDUMP_DEFINE

void H265SeiParser::SeiStateUserData::fdump(FILE* outfp, int indent_level) const {
  fprintf(outfp, "user_data_registered_itu_t_t35 sei {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "country_code: %i ", itu_t_t35_country_code);
  fprintf(outfp, "country_code_extension: %i ", itu_t_t35_country_code_extension_byte);
  size_t ud_payload_size;
  const uint8_t* ud_payload = get_userdata_payload(ud_payload_size);
  fprintf(outfp, "payload_size: %zu ", ud_payload_size);

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiParser::SeiStateNotImplemented::fdump(FILE* outfp, int indent_level) const {
  fprintf(outfp, "unimplemented sei {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload_type: %i ", payload_type);
  fprintf(outfp, "payload_size: %u ", payload_size);

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
