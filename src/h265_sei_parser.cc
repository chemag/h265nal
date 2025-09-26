/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_sei_parser.h"

#include <stdio.h>

#include <cinttypes>
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
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseSei(&bit_buffer);
}

std::unique_ptr<H265SeiMessageParser::SeiMessageState>
H265SeiMessageParser::ParseSei(BitBuffer* bit_buffer) noexcept {
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
    case SeiType::user_data_unregistered:
      payload_parser = std::make_unique<H265SeiUserDataUnregisteredParser>();
      break;
    case SeiType::mastering_display_colour_volume:
      payload_parser =
          std::make_unique<H265SeiMasteringDisplayColourVolumeParser>();
      break;
    case SeiType::content_light_level_info:
      payload_parser = std::make_unique<H265SeiContentLightLevelInfoParser>();
      break;
    case SeiType::alpha_channel_info:
      payload_parser = std::make_unique<H265SeiAlphaChannelInfoParser>();
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
H265SeiUserDataRegisteredItuTT35Parser::parse_payload(BitBuffer* bit_buffer,
                                                      uint32_t payload_size) {
  // H265 SEI user data ITU T-35 (user_data_registered_itu_t_t35()) parser.
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
H265SeiUserDataUnregisteredParser::parse_payload(BitBuffer* bit_buffer,
                                                 uint32_t payload_size) {
  uint32_t bits_tmp;

  // H265 SEI user data unregistered (user_data_unregistered()) parser.
  // Section D.2.7 ("User data unregisterd SEI message syntax") of
  // the H.265 standard for a complete description.
  uint32_t remaining_payload_size = payload_size;
  if (remaining_payload_size < 16) {
    return nullptr;
  }
  auto payload_state = std::make_unique<H265SeiUserDataUnregisteredState>();

  // uuid_iso_iec_11578  u(128)
  payload_state->uuid_iso_iec_11578_1 = 0;
  if (!bit_buffer->ReadBits(32, bits_tmp)) {
    return nullptr;
  }
  payload_state->uuid_iso_iec_11578_1 |= (uint64_t)bits_tmp << 32;
  if (!bit_buffer->ReadBits(32, bits_tmp)) {
    return nullptr;
  }
  payload_state->uuid_iso_iec_11578_1 |= (uint64_t)bits_tmp << 0;

  payload_state->uuid_iso_iec_11578_2 = 0;
  if (!bit_buffer->ReadBits(32, bits_tmp)) {
    return nullptr;
  }
  payload_state->uuid_iso_iec_11578_2 |= (uint64_t)bits_tmp << 32;
  if (!bit_buffer->ReadBits(32, bits_tmp)) {
    return nullptr;
  }
  payload_state->uuid_iso_iec_11578_2 |= (uint64_t)bits_tmp << 0;

  remaining_payload_size -= 16;

  payload_state->payload.resize(remaining_payload_size);
  for (size_t i = 0; i < payload_state->payload.size(); ++i) {
    // user_data_payload_byte  b(8)
    if (!bit_buffer->ReadUInt8(payload_state->payload[i])) {
      return nullptr;
    }
  }
  return payload_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiAlphaChannelInfoParser::parse_payload(BitBuffer* bit_buffer,
                                             uint32_t payload_size) {
  (void)payload_size;
  // H265 SEI alpha channel info (alpha_channel_info()) parser.
  // Section F.14.2.8 ("Alpha channel information SEI message syntax") and
  //  F.14.3.8 ("Alpha channel information SEI message semantics") of
  // the H.265 standard for a complete description.
  auto payload_state = std::make_unique<H265SeiAlphaChannelInfoState>();

  // alpha_channel_cancel_flag  u(1)
  if (!bit_buffer->ReadBits(1, payload_state->alpha_channel_cancel_flag)) {
    return nullptr;
  }

  if (!payload_state->alpha_channel_cancel_flag) {
    // alpha_channel_use_idc  u(3)
    if (!bit_buffer->ReadBits(3, payload_state->alpha_channel_use_idc)) {
      return nullptr;
    }

    // alpha_channel_bit_depth_minus8  u(3)
    if (!bit_buffer->ReadBits(3,
                              payload_state->alpha_channel_bit_depth_minus8)) {
      return nullptr;
    }

    // alpha_transparent_value  u(v)
    // The number of bits used for the representation of the
    // alpha_transparent_value syntax element is
    // alpha_channel_bit_depth_minus8 + 9
    if (!bit_buffer->ReadBits(payload_state->alpha_channel_bit_depth_minus8 + 9,
                              payload_state->alpha_transparent_value)) {
      return nullptr;
    }

    // alpha_opaque_value  u(v)
    // The number of bits used for the representation of the
    // alpha_opaque_value syntax element is alpha_channel_bit_depth_minus8 + 9
    if (!bit_buffer->ReadBits(payload_state->alpha_channel_bit_depth_minus8 + 9,
                              payload_state->alpha_opaque_value)) {
      return nullptr;
    }

    // alpha_channel_incr_flag  u(1)
    if (!bit_buffer->ReadBits(1, payload_state->alpha_channel_incr_flag)) {
      return nullptr;
    }

    // alpha_channel_clip_flag  u(1)
    if (!bit_buffer->ReadBits(1, payload_state->alpha_channel_clip_flag)) {
      return nullptr;
    }

    if (payload_state->alpha_channel_clip_flag) {
      // alpha_channel_clip_type_flag  u(1)
      if (!bit_buffer->ReadBits(1,
                                payload_state->alpha_channel_clip_type_flag)) {
        return nullptr;
      }
    }
  }

  return payload_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiMasteringDisplayColourVolumeParser::parse_payload(
    BitBuffer* bit_buffer, uint32_t payload_size) {
  (void)payload_size;
  // H265 SEI mastering display colour volume
  // (mastering_display_colour_volume()) parser. Section D.2.28 ("Mastering
  // display colour volume SEI message syntax") of the H.265 standard for a
  // complete description.
  auto payload_state =
      std::make_unique<H265SeiMasteringDisplayColourVolumeState>();

  // display_primaries_x[c] and display_primaries_y[c]  u(16) each
  for (int c = 0; c < 3; c++) {
    uint32_t primaries_x, primaries_y;
    if (!bit_buffer->ReadBits(16, primaries_x)) {
      return nullptr;
    }
    payload_state->display_primaries_x[c] = static_cast<uint16_t>(primaries_x);

    if (!bit_buffer->ReadBits(16, primaries_y)) {
      return nullptr;
    }
    payload_state->display_primaries_y[c] = static_cast<uint16_t>(primaries_y);
  }

  // white_point_x  u(16)
  uint32_t white_x, white_y;
  if (!bit_buffer->ReadBits(16, white_x)) {
    return nullptr;
  }
  payload_state->white_point_x = static_cast<uint16_t>(white_x);

  // white_point_y  u(16)
  if (!bit_buffer->ReadBits(16, white_y)) {
    return nullptr;
  }
  payload_state->white_point_y = static_cast<uint16_t>(white_y);

  // max_display_mastering_luminance  u(32)
  if (!bit_buffer->ReadBits(32,
                            payload_state->max_display_mastering_luminance)) {
    return nullptr;
  }

  // min_display_mastering_luminance  u(32)
  if (!bit_buffer->ReadBits(32,
                            payload_state->min_display_mastering_luminance)) {
    return nullptr;
  }

  return payload_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiContentLightLevelInfoParser::parse_payload(BitBuffer* bit_buffer,
                                                  uint32_t payload_size) {
  (void)payload_size;
  // H265 SEI content light level info (content_light_level_info()) parser.
  // Section D.2.35 ("Content light level information SEI message syntax")
  // of the H.265 standard for a complete description.
  auto payload_state = std::make_unique<H265SeiContentLightLevelInfoState>();

  // max_content_light_level  u(16)
  uint32_t max_content_light, max_pic_average;
  if (!bit_buffer->ReadBits(16, max_content_light)) {
    return nullptr;
  }
  payload_state->max_content_light_level =
      static_cast<uint16_t>(max_content_light);

  // max_pic_average_light_level  u(16)
  if (!bit_buffer->ReadBits(16, max_pic_average)) {
    return nullptr;
  }
  payload_state->max_pic_average_light_level =
      static_cast<uint16_t>(max_pic_average);

  return payload_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiUnknownParser::parse_payload(BitBuffer* bit_buffer,
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
  fprintf(outfp, "payload_type: %i", static_cast<int>(payload_type));

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "payload_size: %u", payload_size);

  if (payload_state != nullptr) {
    fdump_indent_level(outfp, indent_level);
    payload_state->fdump(outfp, indent_level);
  }

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

void H265SeiUserDataUnregisteredParser::H265SeiUserDataUnregisteredState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "user_data_unregistered {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  // this is an UUID: 4-2-2-2-6 structure
  fprintf(outfp,
          "uuid_iso_iec_11578: %08" PRIx64 "-%04" PRIx64 "-%04" PRIx64
          "-%04" PRIx64 "-%012" PRIx64,
          uuid_iso_iec_11578_1 >> 32, (uuid_iso_iec_11578_1 >> 16) & 0xffff,
          (uuid_iso_iec_11578_1 >> 0) & 0xffff, uuid_iso_iec_11578_2 >> 48,
          uuid_iso_iec_11578_2 & 0x0000ffffffffffff);

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

void H265SeiAlphaChannelInfoParser::H265SeiAlphaChannelInfoState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "alpha_channel_info {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "alpha_channel_cancel_flag: %i", alpha_channel_cancel_flag);

  if (!alpha_channel_cancel_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "alpha_channel_use_idc: %i", alpha_channel_use_idc);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "alpha_channel_bit_depth_minus8: %i",
            alpha_channel_bit_depth_minus8);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "alpha_transparent_value: %i", alpha_transparent_value);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "alpha_opaque_value: %i", alpha_opaque_value);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "alpha_channel_incr_flag: %i", alpha_channel_incr_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "alpha_channel_clip_flag: %i", alpha_channel_clip_flag);

    if (alpha_channel_clip_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "alpha_channel_clip_type_flag: %i",
              alpha_channel_clip_type_flag);
    }
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiMasteringDisplayColourVolumeParser::
    H265SeiMasteringDisplayColourVolumeState::fdump(FILE* outfp,
                                                    int indent_level) const {
  fprintf(outfp, "mastering_display_colour_volume {");
  indent_level = indent_level_incr(indent_level);

  for (int c = 0; c < 3; c++) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "display_primaries[%d]_x: %u (%.5f)", c,
            display_primaries_x[c], display_primaries_x[c] * 0.00002);
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "display_primaries[%d]_y: %u (%.5f)", c,
            display_primaries_y[c], display_primaries_y[c] * 0.00002);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "white_point_x: %u (%.5f)", white_point_x,
          white_point_x * 0.00002);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "white_point_y: %u (%.5f)", white_point_y,
          white_point_y * 0.00002);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_display_mastering_luminance: %u (%.4f cd/m^2)",
          max_display_mastering_luminance,
          max_display_mastering_luminance * 0.0001);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "min_display_mastering_luminance: %u (%.4f cd/m^2)",
          min_display_mastering_luminance,
          min_display_mastering_luminance * 0.0001);

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiContentLightLevelInfoParser::H265SeiContentLightLevelInfoState::
    fdump(FILE* outfp, int indent_level) const {
  fprintf(outfp, "content_light_level_info {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_content_light_level: %u cd/m^2", max_content_light_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_pic_average_light_level: %u cd/m^2",
          max_pic_average_light_level);

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
