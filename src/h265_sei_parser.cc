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
  uint8_t ff_byte = 0xff;
  auto sei_message_state = std::make_unique<SeiMessageState>();

  while (ff_byte == 0xff) {
    bit_buffer->ReadUInt8(ff_byte);  // f(8)
    payload_type += ff_byte;
  }

  ff_byte = 0xff;
  while (ff_byte == 0xff) {
    bit_buffer->ReadUInt8(ff_byte);  // f(8)
    payload_size += ff_byte;
  }

  sei_message_state->payload_type = static_cast<SeiType>(payload_type);
  sei_message_state->payload_size = payload_size;

  std::unique_ptr<H265SeiPayloadParser> payload_parser = nullptr;
  switch (static_cast<SeiType>(payload_type)) {
    case SeiType::user_data_registered_itu_t_t35:
      payload_parser =
          std::make_unique<H265SeiUserDataRegisteredItuTT35Parser>();
      break;
    default:
      payload_parser = std::make_unique<H265SeiNotImplementedParser>();
      break;
  }

  sei_message_state->payload_state =
      payload_parser->parse_payload(bit_buffer, payload_size);
  return sei_message_state;
}

void H265SeiMessageParser::SeiMessageState::serialize(
    std::vector<uint8_t>& bytes) const {
  uint32_t remaining_type = static_cast<uint8_t>(payload_type);
  while (remaining_type > 0xff) {
    bytes.push_back(0xff);
    remaining_type -= 0xff;
  }
  bytes.push_back(static_cast<uint8_t>(remaining_type));

  uint32_t remaining_size = payload_size;
  while (remaining_size > 0xff) {
    bytes.push_back(0xff);
    remaining_size -= 0xff;
  }
  bytes.push_back(static_cast<uint8_t>(remaining_size));

  if (payload_state) {
    payload_state->serialize(bytes);
  }
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiUserDataRegisteredItuTT35Parser::parse_payload(
    rtc::BitBuffer* bit_buffer, uint32_t payload_size) {
  uint32_t remaining_payload_size = payload_size;
  if (remaining_payload_size == 0) {
    return nullptr;
  }
  auto payload_state =
      std::make_unique<H265SeiUserDataRegisteredItuTT35State>();
  bit_buffer->ReadUInt8(payload_state->itu_t_t35_country_code);
  remaining_payload_size--;
  if (payload_state->itu_t_t35_country_code == 0xff) {
    if (remaining_payload_size <= 0) {
      return nullptr;
    }
    bit_buffer->ReadUInt8(payload_state->itu_t_t35_country_code_extension_byte);
    remaining_payload_size--;
  }
  payload_state->payload.resize(remaining_payload_size);
  for (size_t i = 0; i < payload_state->payload.size(); ++i) {
    bit_buffer->ReadUInt8(payload_state->payload[i]);
  }
  return payload_state;
}

void H265SeiUserDataRegisteredItuTT35Parser::
    H265SeiUserDataRegisteredItuTT35State::serialize(
        std::vector<uint8_t>& bytes) const {
  if (itu_t_t35_country_code == 0xff) {
    bytes.push_back(itu_t_t35_country_code);
    bytes.push_back(itu_t_t35_country_code_extension_byte);
  } else {
    bytes.push_back(itu_t_t35_country_code);
  }
  bytes.insert(bytes.end(), payload.begin(), payload.end());
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiNotImplementedParser::parse_payload(rtc::BitBuffer* bit_buffer,
                                           uint32_t payload_size) {
  // We have no specific details for this sei, just keep all the bytes
  // in a payload buffer.
  uint32_t remaining_payload_size = payload_size;
  if (remaining_payload_size == 0) {
    return nullptr;
  }
  auto payload_state = std::make_unique<H265SeiNotImplementedState>();
  payload_state->payload.resize(remaining_payload_size);
  for (size_t i = 0; i < payload_state->payload.size(); ++i) {
    bit_buffer->ReadUInt8(payload_state->payload[i]);
  }
  return payload_state;
}

void H265SeiNotImplementedParser::H265SeiNotImplementedState::serialize(
    std::vector<uint8_t>& bytes) const {
  bytes.insert(bytes.end(), payload.begin(), payload.end());
}

#ifdef FDUMP_DEFINE

void H265SeiMessageParser::SeiMessageState::fdump(FILE* outfp,
                                                  int indent_level) const {
  fprintf(outfp, "sei message {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload_type: %i ", payload_type);
  fprintf(outfp, "payload_size: %u ", payload_size);
  payload_state->fdump(outfp, indent_level);
  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiUserDataRegisteredItuTT35Parser::
    H265SeiUserDataRegisteredItuTT35State::fdump(FILE* outfp,
                                                 int indent_level) const {
  fprintf(outfp, "user_data_registered_itu_t_t35 sei {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "country_code: %i ", itu_t_t35_country_code);
  fprintf(outfp, "country_code_extension: %i ",
          itu_t_t35_country_code_extension_byte);
  fprintf(outfp, "payload_size: %zu ", payload.size());

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiNotImplementedParser::H265SeiNotImplementedState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "unimplemented sei {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload_size: %zu ", payload.size());

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
