/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */


#include "h265_vps_parser.h"

#include <cstdint>
#include <vector>

#include "h265_common.h"
#include "absl/types/optional.h"
#include "rtc_base/bit_buffer.h"

namespace {
typedef absl::optional<h265nal::H265ProfileTierLevelParser::
    ProfileTierLevelState> OptionalProfileTierLevel;
typedef absl::optional<h265nal::H265VpsParser::
    VpsState> OptionalVps;
}  // namespace

namespace h265nal {

// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Unpack RBSP and parse VPS state from the supplied buffer.
absl::optional<H265VpsParser::VpsState> H265VpsParser::ParseVps(
    const uint8_t* data, size_t length) {

  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseVps(&bit_buffer);
}


absl::optional<H265VpsParser::VpsState> H265VpsParser::ParseVps(
    rtc::BitBuffer* bit_buffer) {
  uint32_t golomb_tmp;

  // H265 VPS (video_parameter_set_rbsp()) NAL Unit.
  // Section 7.3.2.1 ("Video parameter set data syntax") of the H.265
  // standard for a complete description.
  VpsState vps;

  // vps_video_parameter_set_id  u(4)
  if (!bit_buffer->ReadBits(&(vps.vps_video_parameter_set_id), 4)) {
    return absl::nullopt;
  }

  // vps_base_layer_internal_flag  u(1)
  if (!bit_buffer->ReadBits(&(vps.vps_base_layer_internal_flag), 1)) {
    return absl::nullopt;
  }

  // vps_base_layer_available_flag  u(1)
  if (!bit_buffer->ReadBits(&(vps.vps_base_layer_available_flag), 1)) {
    return absl::nullopt;
  }

  // vps_max_layers_minus1  u(6)
  if (!bit_buffer->ReadBits(&(vps.vps_max_layers_minus1), 6)) {
    return absl::nullopt;
  }

  // vps_max_sub_layers_minus1  u(3)
  if (!bit_buffer->ReadBits(&(vps.vps_max_sub_layers_minus1), 3)) {
    return absl::nullopt;
  }

  // vps_temporal_id_nesting_flag  u(1)
  if (!bit_buffer->ReadBits(&(vps.vps_temporal_id_nesting_flag), 1)) {
    return absl::nullopt;
  }

  // vps_reserved_0xffff_16bits  u(16)
  if (!bit_buffer->ReadBits(&(vps.vps_reserved_0xffff_16bits), 16)) {
    return absl::nullopt;
  }

  // profile_tier_level(1, vps_max_sub_layers_minus1)
  OptionalProfileTierLevel profile_tier_level =
      H265ProfileTierLevelParser::ParseProfileTierLevel(
          bit_buffer, true, vps.vps_max_sub_layers_minus1);
  if (profile_tier_level != absl::nullopt) {
    vps.profile_tier_level = *profile_tier_level;
  }

  // vps_sub_layer_ordering_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(&(vps.vps_sub_layer_ordering_info_present_flag),
                            1)) {
    return absl::nullopt;
  }

  for (int i = (vps.vps_sub_layer_ordering_info_present_flag ?
       0 : vps.vps_max_sub_layers_minus1);
       i <= vps.vps_max_sub_layers_minus1; i++) {
    // vps_max_dec_pic_buffering_minus1[i]  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(&golomb_tmp)) {
      return absl::nullopt;
    }
    vps.vps_max_dec_pic_buffering_minus1.push_back(golomb_tmp);
    // vps_max_num_reorder_pics[i]  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(&golomb_tmp)) {
      return absl::nullopt;
    }
    vps.vps_max_num_reorder_pics.push_back(golomb_tmp);
    // vps_max_latency_increase_plus1[i]  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(&golomb_tmp)) {
      return absl::nullopt;
    }
    vps.vps_max_latency_increase_plus1.push_back(golomb_tmp);
  }

  // vps_max_layer_id  u(6)
  if (!bit_buffer->ReadBits(&(vps.vps_max_layer_id), 6)) {
    return absl::nullopt;
  }

  // vps_num_layer_sets_minus1  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(&(vps.vps_num_layer_sets_minus1))) {
    return absl::nullopt;
  }

  for (int i = 1; i <= vps.vps_num_layer_sets_minus1; i++) {
    vps.layer_id_included_flag[i-1].emplace_back();
    for (int j = 0; j <= vps.vps_max_layer_id; j++) {
      // layer_id_included_flag[i][j]  u(1)
      vps.layer_id_included_flag[i-1].push_back(golomb_tmp);
    }
  }

  // vps_timing_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(&(vps.vps_timing_info_present_flag), 1)) {
    return absl::nullopt;
  }

  if (vps.vps_timing_info_present_flag) {
    // vps_num_units_in_tick  u(32)
    if (!bit_buffer->ReadBits(&(vps.vps_num_units_in_tick), 32)) {
      return absl::nullopt;
    }

    // vps_time_scale  u(32)
    if (!bit_buffer->ReadBits(&(vps.vps_time_scale), 32)) {
      return absl::nullopt;
    }

    // vps_poc_proportional_to_timing_flag  u(1)
    if (!bit_buffer->ReadBits(&(vps.vps_poc_proportional_to_timing_flag), 1)) {
      return absl::nullopt;
    }

    if (vps.vps_poc_proportional_to_timing_flag) {
      // vps_num_ticks_poc_diff_one_minus1  ue(v)
      if (!bit_buffer->ReadExponentialGolomb(
          &(vps.vps_num_ticks_poc_diff_one_minus1))) {
        return absl::nullopt;
      }
    }
    // vps_num_hrd_parameters  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(&(vps.vps_num_hrd_parameters))) {
      return absl::nullopt;
    }

    for (int i = 0; i < vps.vps_num_hrd_parameters; i++) {
      // hrd_layer_set_idx[i]  ue(v)
      if (!bit_buffer->ReadExponentialGolomb(&golomb_tmp)) {
        return absl::nullopt;
      }
      vps.hrd_layer_set_idx.push_back(golomb_tmp);

      if (i > 0) {
        // cprms_present_flag[i]  u(1)
        if (!bit_buffer->ReadExponentialGolomb(&golomb_tmp)) {
          return absl::nullopt;
        }
        vps.cprms_present_flag.push_back(golomb_tmp);
      }
      // hrd_parameters(cprms_present_flag[i], vps_max_sub_layers_minus1)
    }
  }

  // vps_extension_flag  u(1)
  if (!bit_buffer->ReadBits(&(vps.vps_extension_flag), 1)) {
    return absl::nullopt;
  }

  if (vps.vps_extension_flag) {
    while (more_rbsp_data(bit_buffer)) {
      // vps_extension_data_flag  u(1)
      if (!bit_buffer->ReadBits(&(vps.vps_extension_data_flag), 1)) {
        return absl::nullopt;
      }
    }
  }
  rbsp_trailing_bits(bit_buffer);

  return OptionalVps(vps);
}

}  // namespace h265nal
