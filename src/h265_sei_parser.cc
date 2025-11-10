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
    case SeiType::knee_function_info:
      payload_parser = std::make_unique<H265SeiKneeFunctionInfoParser>();
      break;
    case SeiType::colour_remapping_info:
      payload_parser = std::make_unique<H265SeiColourRemappingInfoParser>();
      break;
    case SeiType::content_colour_volume:
      payload_parser = std::make_unique<H265SeiContentColourVolumeParser>();
      break;
    case SeiType::alternative_transfer_characteristics:
      payload_parser =
          std::make_unique<H265SeiAlternativeTransferCharacteristicsParser>();
      break;
    case SeiType::ambient_viewing_environment:
      payload_parser =
          std::make_unique<H265SeiAmbientViewingEnvironmentParser>();
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
H265SeiKneeFunctionInfoParser::parse_payload(BitBuffer* bit_buffer,
                                             uint32_t payload_size) {
  (void)payload_size;
  // H265 SEI knee function info (knee_function_info()) parser.
  // Section D.2.30 ("Knee function information SEI message syntax")
  // of the H.265 standard for a complete description.
  auto payload_state = std::make_unique<H265SeiKneeFunctionInfoState>();

  // knee_function_id  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(payload_state->knee_function_id)) {
    return nullptr;
  }

  // knee_function_cancel_flag  u(1)
  if (!bit_buffer->ReadBits(1, payload_state->knee_function_cancel_flag)) {
    return nullptr;
  }

  if (!payload_state->knee_function_cancel_flag) {
    // knee_function_persistence_flag  u(1)
    if (!bit_buffer->ReadBits(1,
                              payload_state->knee_function_persistence_flag)) {
      return nullptr;
    }

    // input_d_range  u(32)
    if (!bit_buffer->ReadBits(32, payload_state->input_d_range)) {
      return nullptr;
    }

    // input_disp_luminance  u(32)
    if (!bit_buffer->ReadBits(32, payload_state->input_disp_luminance)) {
      return nullptr;
    }

    // output_d_range  u(32)
    if (!bit_buffer->ReadBits(32, payload_state->output_d_range)) {
      return nullptr;
    }

    // output_disp_luminance  u(32)
    if (!bit_buffer->ReadBits(32, payload_state->output_disp_luminance)) {
      return nullptr;
    }

    // num_knee_points_minus1  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            payload_state->num_knee_points_minus1)) {
      return nullptr;
    }

    // Read knee points
    for (uint32_t i = 0; i <= payload_state->num_knee_points_minus1; i++) {
      // input_knee_point[i]  u(10)
      uint32_t input_knee;
      if (!bit_buffer->ReadBits(10, input_knee)) {
        return nullptr;
      }
      payload_state->input_knee_point.push_back(
          static_cast<uint16_t>(input_knee));

      // output_knee_point[i]  u(10)
      uint32_t output_knee;
      if (!bit_buffer->ReadBits(10, output_knee)) {
        return nullptr;
      }
      payload_state->output_knee_point.push_back(
          static_cast<uint16_t>(output_knee));
    }
  }

  return payload_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiColourRemappingInfoParser::parse_payload(BitBuffer* bit_buffer,
                                                uint32_t payload_size) {
  (void)payload_size;
  // H265 SEI colour remapping info (colour_remapping_info()) parser.
  // Section D.2.31 ("Colour remapping information SEI message syntax")
  // of the H.265 standard for a complete description.
  auto payload_state = std::make_unique<H265SeiColourRemappingInfoState>();

  // colour_remap_id  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(payload_state->colour_remap_id)) {
    return nullptr;
  }

  // colour_remap_cancel_flag  u(1)
  if (!bit_buffer->ReadBits(1, payload_state->colour_remap_cancel_flag)) {
    return nullptr;
  }

  if (!payload_state->colour_remap_cancel_flag) {
    // colour_remap_persistence_flag  u(1)
    if (!bit_buffer->ReadBits(1,
                              payload_state->colour_remap_persistence_flag)) {
      return nullptr;
    }

    // colour_remap_video_signal_info_present_flag  u(1)
    if (!bit_buffer->ReadBits(
            1, payload_state->colour_remap_video_signal_info_present_flag)) {
      return nullptr;
    }

    if (payload_state->colour_remap_video_signal_info_present_flag) {
      // colour_remap_full_range_flag  u(1)
      if (!bit_buffer->ReadBits(1,
                                payload_state->colour_remap_full_range_flag)) {
        return nullptr;
      }

      // colour_remap_primaries  u(8)
      uint32_t primaries, transfer, matrix;
      if (!bit_buffer->ReadBits(8, primaries)) {
        return nullptr;
      }
      payload_state->colour_remap_primaries = static_cast<uint8_t>(primaries);

      // colour_remap_transfer_function  u(8)
      if (!bit_buffer->ReadBits(8, transfer)) {
        return nullptr;
      }
      payload_state->colour_remap_transfer_function =
          static_cast<uint8_t>(transfer);

      // colour_remap_matrix_coefficients  u(8)
      if (!bit_buffer->ReadBits(8, matrix)) {
        return nullptr;
      }
      payload_state->colour_remap_matrix_coefficients =
          static_cast<uint8_t>(matrix);
    }

    // colour_remap_input_bit_depth  u(8)
    uint32_t input_bit_depth, bit_depth;
    if (!bit_buffer->ReadBits(8, input_bit_depth)) {
      return nullptr;
    }
    payload_state->colour_remap_input_bit_depth =
        static_cast<uint8_t>(input_bit_depth);

    // colour_remap_output_bit_depth  u(8)
    if (!bit_buffer->ReadBits(8, bit_depth)) {
      return nullptr;
    }
    payload_state->colour_remap_output_bit_depth =
        static_cast<uint8_t>(bit_depth);

    // Validate bit depths to avoid division by zero in bit_depth_used formula.
    // The formula ((bit_depth + 7) >> 3) << 3 evaluates to 0 when bit_depth is
    // 0, which would cause ReadBits(0, ...) to fail with an assertion.
    if (payload_state->colour_remap_input_bit_depth == 0 ||
        payload_state->colour_remap_output_bit_depth == 0) {
      return nullptr;
    }

    // Initialize vectors for 3 color components
    payload_state->pre_lut_coded_value.resize(3);
    payload_state->pre_lut_target_value.resize(3);

    // Pre-LUT
    for (size_t c = 0; c < 3; c++) {
      // pre_lut_num_val_minus1[c]  u(8)
      uint32_t num_val_minus1;
      if (!bit_buffer->ReadBits(8, num_val_minus1)) {
        return nullptr;
      }
      payload_state->pre_lut_num_val_minus1[c] =
          static_cast<uint8_t>(num_val_minus1);

      if (payload_state->pre_lut_num_val_minus1[c] > 0) {
        uint32_t bit_depth_used = static_cast<uint32_t>(
            ((payload_state->colour_remap_input_bit_depth + 7) >> 3) << 3);
        for (uint32_t i = 0; i <= payload_state->pre_lut_num_val_minus1[c];
             i++) {
          // pre_lut_coded_value[c][i]  u(v)
          // The number of bits used to represent pre_lut_coded_value[c][i]
          // is ((colour_remap_input_bit_depth + 7) >> 3) << 3
          uint32_t coded_value;
          if (!bit_buffer->ReadBits(bit_depth_used, coded_value)) {
            return nullptr;
          }
          payload_state->pre_lut_coded_value[c].push_back(coded_value);

          // pre_lut_target_value[c][i]  u(v)
          // The number of bits used to represent pre_lut_target_value[c][i]
          // is ((colour_remap_output_bit_depth + 7) >> 3) << 3
          uint32_t target_value;
          if (!bit_buffer->ReadBits(bit_depth_used, target_value)) {
            return nullptr;
          }
          payload_state->pre_lut_target_value[c].push_back(target_value);
        }
      }
    }

    // colour_remap_matrix_present_flag  u(1)
    if (!bit_buffer->ReadBits(
            1, payload_state->colour_remap_matrix_present_flag)) {
      return nullptr;
    }

    if (payload_state->colour_remap_matrix_present_flag) {
      // log2_matrix_denom  u(4)
      if (!bit_buffer->ReadBits(4, payload_state->log2_matrix_denom)) {
        return nullptr;
      }

      // Initialize matrix (3x3)
      payload_state->colour_remap_coeffs.resize(3);
      for (size_t c = 0; c < 3; c++) {
        payload_state->colour_remap_coeffs[c].resize(3);
        for (size_t i = 0; i < 3; i++) {
          // colour_remap_coeffs[c][i]  se(v)
          int32_t coeff;
          if (!bit_buffer->ReadSignedExponentialGolomb(coeff)) {
            return nullptr;
          }
          payload_state->colour_remap_coeffs[c][i] = coeff;
        }
      }
    }

    // Post-LUT
    payload_state->post_lut_coded_value.resize(3);
    payload_state->post_lut_target_value.resize(3);

    for (size_t c = 0; c < 3; c++) {
      // post_lut_num_val_minus1[c]  u(8)
      uint32_t num_val_minus1;
      if (!bit_buffer->ReadBits(8, num_val_minus1)) {
        return nullptr;
      }
      payload_state->post_lut_num_val_minus1[c] =
          static_cast<uint8_t>(num_val_minus1);

      if (payload_state->post_lut_num_val_minus1[c] > 0) {
        uint32_t bit_depth_used = static_cast<uint32_t>(
            ((payload_state->colour_remap_output_bit_depth + 7) >> 3) << 3);
        for (uint32_t i = 0; i <= payload_state->post_lut_num_val_minus1[c];
             i++) {
          // post_lut_coded_value[c][i]  u(v)
          // [...] the number of bits used to represent
          // post_lut_coded_value[c][i] is
          // ((colour_remap_output_bit_depth + 7) >> 3) << 3
          uint32_t coded_value;
          if (!bit_buffer->ReadBits(bit_depth_used, coded_value)) {
            return nullptr;
          }
          payload_state->post_lut_coded_value[c].push_back(coded_value);

          // [...] colour_remap_input_bit_depth is replaced by
          // colour_remap_output_bit_depth in the semantics
          // post_lut_target_value[c][i]  u(v)
          uint32_t target_value;
          if (!bit_buffer->ReadBits(bit_depth_used, target_value)) {
            return nullptr;
          }
          payload_state->post_lut_target_value[c].push_back(target_value);
        }
      }
    }
  }

  return payload_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiContentColourVolumeParser::parse_payload(BitBuffer* bit_buffer,
                                                uint32_t payload_size) {
  (void)payload_size;
  // H265 SEI content colour volume (content_colour_volume()) parser.
  // Section D.2.40 ("Content colour volume SEI message syntax")
  // of the H.265 standard for a complete description.
  auto payload_state = std::make_unique<H265SeiContentColourVolumeState>();

  // ccv_cancel_flag  u(1)
  if (!bit_buffer->ReadBits(1, payload_state->ccv_cancel_flag)) {
    return nullptr;
  }

  if (!payload_state->ccv_cancel_flag) {
    // ccv_persistence_flag  u(1)
    if (!bit_buffer->ReadBits(1, payload_state->ccv_persistence_flag)) {
      return nullptr;
    }

    // ccv_primaries_present_flag  u(1)
    if (!bit_buffer->ReadBits(1, payload_state->ccv_primaries_present_flag)) {
      return nullptr;
    }

    // ccv_min_luminance_value_present_flag  u(1)
    if (!bit_buffer->ReadBits(
            1, payload_state->ccv_min_luminance_value_present_flag)) {
      return nullptr;
    }

    // ccv_max_luminance_value_present_flag  u(1)
    if (!bit_buffer->ReadBits(
            1, payload_state->ccv_max_luminance_value_present_flag)) {
      return nullptr;
    }

    // ccv_avg_luminance_value_present_flag  u(1)
    if (!bit_buffer->ReadBits(
            1, payload_state->ccv_avg_luminance_value_present_flag)) {
      return nullptr;
    }

    // ccv_reserved_zero_2bits  u(2)
    if (!bit_buffer->ReadBits(2, payload_state->ccv_reserved_zero_2bits)) {
      return nullptr;
    }

    if (payload_state->ccv_primaries_present_flag) {
      for (int c = 0; c < 3; c++) {
        // ccv_primaries_x[c]  i(32)
        uint32_t primaries_x;
        if (!bit_buffer->ReadBits(32, primaries_x)) {
          return nullptr;
        }
        payload_state->ccv_primaries_x[c] = static_cast<int32_t>(primaries_x);

        // ccv_primaries_y[c]  i(32)
        uint32_t primaries_y;
        if (!bit_buffer->ReadBits(32, primaries_y)) {
          return nullptr;
        }
        payload_state->ccv_primaries_y[c] = static_cast<int32_t>(primaries_y);
      }
    }

    if (payload_state->ccv_min_luminance_value_present_flag) {
      // ccv_min_luminance_value  u(32)
      if (!bit_buffer->ReadBits(32, payload_state->ccv_min_luminance_value)) {
        return nullptr;
      }
    }

    if (payload_state->ccv_max_luminance_value_present_flag) {
      // ccv_max_luminance_value  u(32)
      if (!bit_buffer->ReadBits(32, payload_state->ccv_max_luminance_value)) {
        return nullptr;
      }
    }

    if (payload_state->ccv_avg_luminance_value_present_flag) {
      // ccv_avg_luminance_value  u(32)
      if (!bit_buffer->ReadBits(32, payload_state->ccv_avg_luminance_value)) {
        return nullptr;
      }
    }
  }

  return payload_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiAlternativeTransferCharacteristicsParser::parse_payload(
    BitBuffer* bit_buffer, uint32_t payload_size) {
  (void)payload_size;
  // H265 SEI alternative transfer characteristics
  // (alternative_transfer_characteristics()) parser.
  // Section D.2.38 ("Alternative transfer characteristics SEI message syntax")
  // of the H.265 standard for a complete description.
  auto payload_state =
      std::make_unique<H265SeiAlternativeTransferCharacteristicsState>();

  // preferred_transfer_characteristics  u(8)
  uint32_t preferred_transfer_char;
  if (!bit_buffer->ReadBits(8, preferred_transfer_char)) {
    return nullptr;
  }
  payload_state->preferred_transfer_characteristics =
      static_cast<uint8_t>(preferred_transfer_char);

  return payload_state;
}

std::unique_ptr<H265SeiPayloadParser::H265SeiPayloadState>
H265SeiAmbientViewingEnvironmentParser::parse_payload(BitBuffer* bit_buffer,
                                                      uint32_t payload_size) {
  (void)payload_size;
  // H265 SEI ambient viewing environment
  // (ambient_viewing_environment()) parser.
  // Section D.2.39 ("Ambient viewing environment SEI message syntax")
  // of the H.265 standard for a complete description.
  auto payload_state =
      std::make_unique<H265SeiAmbientViewingEnvironmentState>();

  // ambient_illuminance  u(32)
  uint32_t ambient_illuminance;
  if (!bit_buffer->ReadBits(32, ambient_illuminance)) {
    return nullptr;
  }
  payload_state->ambient_illuminance = ambient_illuminance;

  // ambient_light_x  u(16)
  uint32_t ambient_light_x;
  if (!bit_buffer->ReadBits(16, ambient_light_x)) {
    return nullptr;
  }
  payload_state->ambient_light_x = static_cast<uint16_t>(ambient_light_x);

  // ambient_light_y  u(16)
  uint32_t ambient_light_y;
  if (!bit_buffer->ReadBits(16, ambient_light_y)) {
    return nullptr;
  }
  payload_state->ambient_light_y = static_cast<uint16_t>(ambient_light_y);

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

void H265SeiKneeFunctionInfoParser::H265SeiKneeFunctionInfoState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "knee_function_info {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "knee_function_id: %u", knee_function_id);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "knee_function_cancel_flag: %u", knee_function_cancel_flag);

  if (!knee_function_cancel_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "knee_function_persistence_flag: %u",
            knee_function_persistence_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "input_d_range: %u", input_d_range);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "input_disp_luminance: %u", input_disp_luminance);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "output_d_range: %u", output_d_range);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "output_disp_luminance: %u", output_disp_luminance);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "num_knee_points_minus1: %u", num_knee_points_minus1);

    if (!input_knee_point.empty()) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "input_knee_point: {");
      for (const auto& val : input_knee_point) {
        fprintf(outfp, " %u", val);
      }
      fprintf(outfp, " }");

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "output_knee_point: {");
      for (const auto& val : output_knee_point) {
        fprintf(outfp, " %u", val);
      }
      fprintf(outfp, " }");
    }
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiColourRemappingInfoParser::H265SeiColourRemappingInfoState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "colour_remapping_info {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "colour_remap_id: %u", colour_remap_id);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "colour_remap_cancel_flag: %u", colour_remap_cancel_flag);

  if (!colour_remap_cancel_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "colour_remap_persistence_flag: %u",
            colour_remap_persistence_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "colour_remap_video_signal_info_present_flag: %u",
            colour_remap_video_signal_info_present_flag);

    if (colour_remap_video_signal_info_present_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "colour_remap_full_range_flag: %u",
              colour_remap_full_range_flag);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "colour_remap_primaries: %u", colour_remap_primaries);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "colour_remap_transfer_function: %u",
              colour_remap_transfer_function);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "colour_remap_matrix_coefficients: %u",
              colour_remap_matrix_coefficients);
    }

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "colour_remap_input_bit_depth: %u",
            colour_remap_input_bit_depth);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "colour_remap_output_bit_depth: %u",
            colour_remap_output_bit_depth);

    // Pre-LUT
    for (size_t c = 0; c < 3; c++) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "pre_lut_num_val_minus1[%zu]: %u", c,
              pre_lut_num_val_minus1[c]);

      if (pre_lut_num_val_minus1[c] > 0 && c < pre_lut_coded_value.size()) {
        fdump_indent_level(outfp, indent_level);
        fprintf(outfp, "pre_lut_coded_value[%zu]: {", c);
        for (const auto& val : pre_lut_coded_value[c]) {
          fprintf(outfp, " %u", val);
        }
        fprintf(outfp, " }");

        fdump_indent_level(outfp, indent_level);
        fprintf(outfp, "pre_lut_target_value[%zu]: {", c);
        for (const auto& val : pre_lut_target_value[c]) {
          fprintf(outfp, " %u", val);
        }
        fprintf(outfp, " }");
      }
    }

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "colour_remap_matrix_present_flag: %u",
            colour_remap_matrix_present_flag);

    if (colour_remap_matrix_present_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "log2_matrix_denom: %u", log2_matrix_denom);

      if (!colour_remap_coeffs.empty()) {
        for (size_t c = 0; c < 3; c++) {
          if (c < colour_remap_coeffs.size()) {
            fdump_indent_level(outfp, indent_level);
            fprintf(outfp, "colour_remap_coeffs[%zu]: {", c);
            for (const auto& val : colour_remap_coeffs[c]) {
              fprintf(outfp, " %d", val);
            }
            fprintf(outfp, " }");
          }
        }
      }
    }

    // Post-LUT
    for (size_t c = 0; c < 3; c++) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "post_lut_num_val_minus1[%zu]: %u", c,
              post_lut_num_val_minus1[c]);

      if (post_lut_num_val_minus1[c] > 0 && c < post_lut_coded_value.size()) {
        fdump_indent_level(outfp, indent_level);
        fprintf(outfp, "post_lut_coded_value[%zu]: {", c);
        for (const auto& val : post_lut_coded_value[c]) {
          fprintf(outfp, " %u", val);
        }
        fprintf(outfp, " }");

        fdump_indent_level(outfp, indent_level);
        fprintf(outfp, "post_lut_target_value[%zu]: {", c);
        for (const auto& val : post_lut_target_value[c]) {
          fprintf(outfp, " %u", val);
        }
        fprintf(outfp, " }");
      }
    }
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiContentColourVolumeParser::H265SeiContentColourVolumeState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "content_colour_volume {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "ccv_cancel_flag: %u", ccv_cancel_flag);

  if (!ccv_cancel_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ccv_persistence_flag: %u", ccv_persistence_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ccv_primaries_present_flag: %u",
            ccv_primaries_present_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ccv_min_luminance_value_present_flag: %u",
            ccv_min_luminance_value_present_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ccv_max_luminance_value_present_flag: %u",
            ccv_max_luminance_value_present_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ccv_avg_luminance_value_present_flag: %u",
            ccv_avg_luminance_value_present_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ccv_reserved_zero_2bits: %u", ccv_reserved_zero_2bits);

    if (ccv_primaries_present_flag) {
      for (int c = 0; c < 3; c++) {
        fdump_indent_level(outfp, indent_level);
        fprintf(outfp, "ccv_primaries_x[%d]: %d", c, ccv_primaries_x[c]);

        fdump_indent_level(outfp, indent_level);
        fprintf(outfp, "ccv_primaries_y[%d]: %d", c, ccv_primaries_y[c]);
      }
    }

    if (ccv_min_luminance_value_present_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "ccv_min_luminance_value: %u", ccv_min_luminance_value);
    }

    if (ccv_max_luminance_value_present_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "ccv_max_luminance_value: %u", ccv_max_luminance_value);
    }

    if (ccv_avg_luminance_value_present_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "ccv_avg_luminance_value: %u", ccv_avg_luminance_value);
    }
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiAlternativeTransferCharacteristicsParser::
    H265SeiAlternativeTransferCharacteristicsState::fdump(
        FILE* outfp, int indent_level) const {
  fprintf(outfp, "alternative_transfer_characteristics {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "preferred_transfer_characteristics: %u",
          preferred_transfer_characteristics);

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H265SeiAmbientViewingEnvironmentParser::
    H265SeiAmbientViewingEnvironmentState::fdump(FILE* outfp,
                                                 int indent_level) const {
  fprintf(outfp, "ambient_viewing_environment {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "ambient_illuminance: %u", ambient_illuminance);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "ambient_light_x: %u", ambient_light_x);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "ambient_light_y: %u", ambient_light_y);

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
