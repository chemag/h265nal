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
std::unique_ptr<H265SeiMessageParser::SeiMessageState>
H265SeiMessageParser::ParseSei(const uint8_t* data, size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseSei(&bit_buffer);
}

std::unique_ptr<H265SeiMessageParser::SeiMessageState>
H265SeiMessageParser::ParseSei(rtc::BitBuffer* bit_buffer) noexcept {
  // H265 SEI NAL Unit (access_unit_delimiter_rbsp()) parser.
  // Section 7.3.5 ("Supplemental enhancement information message syntax") of
  // the H.265 standard for a complete description.

  uint32_t payload_type = 0;
  uint32_t payload_size = 0;
  uint32_t ff_byte = 0xff;
  auto sei_message_state = std::make_unique<SeiMessageState>();

  // ff_byte/last_payload_type_byte  f(8)
  ff_byte = 0xff;
  while (ff_byte == 0xff) {
    if (!bit_buffer->ReadBits(8, ff_byte)) {
      return nullptr;
    }
    payload_type += ff_byte;
  }
  sei_message_state->payload_type = static_cast<SeiType>(payload_type);

  // ff_byte/last_payload_size_byte  f(8)
  ff_byte = 0xff;
  while (ff_byte == 0xff) {
    if (!bit_buffer->ReadBits(8, ff_byte)) {
      return nullptr;
    }
    payload_size += ff_byte;
  }
  sei_message_state->payload_size = payload_size;

  // Section D.2.1: General SEI message syntax
  // TODO(chema): move dispatcher to a separate function
  // TODO(chema): enforce nal_unit_type check
  // sei_payload(payloadType, payloadSize)
  std::unique_ptr<H265SeiPayloadParser> payload_parser = nullptr;
  switch (static_cast<SeiType>(payload_type)) {
    case SeiType::user_data_registered_itu_t_t35:
      payload_parser =
          std::make_unique<H265SeiUserDataRegisteredItuTT35Parser>();
      break;
    default:
      payload_parser = std::make_unique<H265SeiUnknownParser>();
      break;
  }

  sei_message_state->payload_state =
      payload_parser->parse_payload(bit_buffer, payload_size);
  return sei_message_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiUserDataRegisteredItuTT35Parser::parse_payload(
    rtc::BitBuffer* bit_buffer, uint32_t payload_size) {
  // H265 SEI user data ITU T-35 (access_unit_delimiter_rbsp()) parser.
  // Section D.2.6 ("User data registered by Recommendation ITU-T T.35
  // SEI message syntax") of the H.265 standard for a complete description.
  uint32_t remaining_payload_size = payload_size;
  if (remaining_payload_size == 0) {
    return nullptr;
  }
  auto payload_state =
      std::make_unique<H265SeiUserDataRegisteredItuTT35State>();

  // itu_t_t35_country_code  b(8)
  if (!bit_buffer->ReadUInt8(payload_state->itu_t_t35_country_code)) {
    return nullptr;
  }
  remaining_payload_size--;

  if (payload_state->itu_t_t35_country_code == 0xff) {
    if (remaining_payload_size <= 0) {
      return nullptr;
    }

    // itu_t_t35_country_code_extension_byte  b(8)
    if (!bit_buffer->ReadUInt8(
            payload_state->itu_t_t35_country_code_extension_byte)) {
      return nullptr;
    }
    remaining_payload_size--;
  }

  payload_state->payload.resize(remaining_payload_size);
  for (size_t i = 0; i < payload_state->payload.size(); ++i) {
    // itu_t_t35_payload_byte  b(8)
    if (!bit_buffer->ReadUInt8(payload_state->payload[i])) {
      return nullptr;
    }
  }
  return payload_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiUnknownParser::parse_payload(rtc::BitBuffer* bit_buffer,
                                    uint32_t payload_size) {
  // We have no specific details for this sei, just keep all the bytes
  // in a payload buffer.
  uint32_t remaining_payload_size = payload_size;
  if (remaining_payload_size == 0) {
    return nullptr;
  }
  auto payload_state = std::make_unique<H265SeiUnknownState>();
  payload_state->payload.resize(remaining_payload_size);
  for (size_t i = 0; i < payload_state->payload.size(); ++i) {
    if (!bit_buffer->ReadUInt8(payload_state->payload[i])) {
      return nullptr;
    }
  }
  return payload_state;
}

#ifdef FDUMP_DEFINE

void H265SeiMessageParser::SeiMessageState::fdump(FILE* outfp,
                                                  int indent_level) const {
  fprintf(outfp, "sei message {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload_type: %i", payload_type);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload_size: %u", payload_size);

  payload_state->fdump(outfp, indent_level);

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiUserDataRegisteredItuTT35Parser::
    H265SeiUserDataRegisteredItuTT35State::fdump(FILE* outfp,
                                                 int indent_level) const {
  fprintf(outfp, "user_data_registered_itu_t_t35 {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "itu_t_t35_country_code: %i", itu_t_t35_country_code);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "itu_t_t35_country_code_extension_byte: %i",
          itu_t_t35_country_code_extension_byte);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload_size: %zu", payload.size());

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload {");
  for (const uint8_t& v : payload) {
    fprintf(outfp, " %u", v);
  }
  fprintf(outfp, " }");

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiUnknownParser::H265SeiUnknownState::fdump(FILE* outfp,
                                                      int indent_level) const {
  fprintf(outfp, "unimplemented {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload_size: %zu ", payload.size());

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload {");
  for (const uint8_t& v : payload) {
    fprintf(outfp, " %u", v);
  }
  fprintf(outfp, " }");

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
