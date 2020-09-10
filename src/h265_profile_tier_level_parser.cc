/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_profile_tier_level_parser.h"

#include <cstdint>
#include <vector>

#include "h265_common.h"
#include "rtc_base/bit_buffer.h"

namespace {
typedef absl::optional<h265nal::H265ProfileInfoParser::ProfileInfoState>
    OptionalProfileInfo;
typedef absl::optional<h265nal::H265ProfileTierLevelParser::
    ProfileTierLevelState> OptionalProfileTierLevel;

#define RETURN_EMPTY_ON_FAIL_PI(x) \
  if (!(x)) {                   \
    return OptionalProfileInfo();       \
  }

#define RETURN_EMPTY_ON_FAIL_PTL(x) \
  if (!(x)) {                   \
    return OptionalProfileTierLevel();       \
  }
}  // namespace

namespace h265nal {

// General note: this is based off the 02/2016 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Unpack RBSP and parse PROFILE_TIER_LEVEL state from the supplied buffer.
absl::optional<H265ProfileTierLevelParser::ProfileTierLevelState>
H265ProfileTierLevelParser::ParseProfileTierLevel(
    const uint8_t* data, size_t length, const bool profilePresentFlag,
    const unsigned int maxNumSubLayersMinus1) {

  std::vector<uint8_t> unpacked_buffer = ParseRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());

  // Now, we need to use a bit buffer to parse through the actual H265
  // profile_tier_level format. See Section 7.3.2.1 ("Video parameter set
  // data syntax") of the H.265 standard for a complete description.
  // Since we only care about resolution, we ignore the majority of fields, but
  // we still have to actively parse through a lot of the data, since many of
  // the fields have variable size.
  ProfileTierLevelState profile_tier_level;

  if (profilePresentFlag) {
    OptionalProfileInfo profile_info = H265ProfileInfoParser::ParseProfileInfo(
        &bit_buffer, true);
    if (profile_info != absl::nullopt) {
      profile_tier_level.general = *profile_info;
    }
  }
  for (int i = 0; i < maxNumSubLayersMinus1; i++) {
    // sub_layer_profile_present_flag[i]  u(1)
    RETURN_EMPTY_ON_FAIL_PTL(
          bit_buffer.ReadBits(
              &(profile_tier_level.sub_layer_profile_present_flag[i]), 1));
    // sub_layer_level_present_flag[i]  u(1)
    RETURN_EMPTY_ON_FAIL_PTL(
          bit_buffer.ReadBits(
              &(profile_tier_level.sub_layer_level_present_flag[i]), 1));
  }

  if (maxNumSubLayersMinus1 > 0) {
    for (int i = maxNumSubLayersMinus1; i < 8; i++) {
      // reserved_zero_2bits[i]  u(2)
      RETURN_EMPTY_ON_FAIL_PTL(
            bit_buffer.ReadBits(
                &(profile_tier_level.reserved_zero_2bits[i]), 2));
    }
  }

  for (int i = 0; i < maxNumSubLayersMinus1; i++) {
    OptionalProfileInfo profile_info =
        H265ProfileInfoParser::ParseProfileInfo(&bit_buffer, true);
    if (profile_info != absl::nullopt) {
      profile_tier_level.sub_layer[i] = *profile_info;
    }
  }

  return OptionalProfileTierLevel(profile_tier_level);
}


absl::optional<H265ProfileInfoParser::ProfileInfoState>
H265ProfileInfoParser::ParseProfileInfo(
      const uint8_t* data, size_t length, bool level_idc_flag) {
  std::vector<uint8_t> unpacked_buffer = ParseRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseProfileInfo(&bit_buffer, level_idc_flag);
}


absl::optional<H265ProfileInfoParser::ProfileInfoState>
H265ProfileInfoParser::ParseProfileInfo(
      rtc::BitBuffer* bit_buffer, bool level_idc_flag) {
  ProfileInfoState profile_info;
  uint32_t bits_tmp, bits_tmp_hi;

  // profile_space  u(2)
  RETURN_EMPTY_ON_FAIL_PI(
      bit_buffer->ReadBits(&profile_info.profile_space, 2));
  // tier_flag  u(1)
  RETURN_EMPTY_ON_FAIL_PI(
        bit_buffer->ReadBits(&profile_info.tier_flag, 1));
  // profile_idc  u(5)
  RETURN_EMPTY_ON_FAIL_PI(
        bit_buffer->ReadBits(&profile_info.profile_idc, 5));
  // for (j = 0; j < 32; j++)
  for (int j = 0; j < 32; j++) {
    // profile_compatibility_flag[j]  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.profile_compatibility_flag[j],
                               1));
  }
  // progressive_source_flag  u(1)
  RETURN_EMPTY_ON_FAIL_PI(
        bit_buffer->ReadBits(&profile_info.progressive_source_flag, 1));
  // interlaced_source_flag  u(1)
  RETURN_EMPTY_ON_FAIL_PI(
        bit_buffer->ReadBits(&profile_info.interlaced_source_flag, 1));
  // non_packed_constraint_flag  u(1)
  RETURN_EMPTY_ON_FAIL_PI(
        bit_buffer->ReadBits(&profile_info.non_packed_constraint_flag, 1));
  // frame_only_constraint_flag  u(1)
  RETURN_EMPTY_ON_FAIL_PI(
        bit_buffer->ReadBits(&profile_info.frame_only_constraint_flag, 1));
  if (profile_info.profile_idc == 4 ||
      profile_info.profile_compatibility_flag[4] == 1 ||
      profile_info.profile_idc == 5 ||
      profile_info.profile_compatibility_flag[5] == 1 ||
      profile_info.profile_idc == 6 ||
      profile_info.profile_compatibility_flag[6] == 1 ||
      profile_info.profile_idc == 7 ||
      profile_info.profile_compatibility_flag[7] == 1 ||
      profile_info.profile_idc == 8 ||
      profile_info.profile_compatibility_flag[8] == 1 ||
      profile_info.profile_idc == 9 ||
      profile_info.profile_compatibility_flag[9] == 1 ||
      profile_info.profile_idc == 10 ||
      profile_info.profile_compatibility_flag[10] == 1) {
    // The number of bits in this syntax structure is not affected by
    // this condition
    // max_12bit_constraint_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.max_12bit_constraint_flag, 1));
    // max_10bit_constraint_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.max_10bit_constraint_flag, 1));
    // max_8bit_constraint_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.max_8bit_constraint_flag, 1));
    // max_422chroma_constraint_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.max_422chroma_constraint_flag, 1));
    // max_420chroma_constraint_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.max_420chroma_constraint_flag, 1));
    // max_monochrome_constraint_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.max_monochrome_constraint_flag,
                               1));
    // intra_constraint_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.intra_constraint_flag, 1));
    // one_picture_only_constraint_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.one_picture_only_constraint_flag,
                               1));
    // lower_bit_rate_constraint_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.lower_bit_rate_constraint_flag,
                               1));
    if (profile_info.profile_idc == 5 ||
        profile_info.profile_compatibility_flag[5] == 1 ||
        profile_info.profile_idc == 9 ||
        profile_info.profile_compatibility_flag[9] == 1 ||
        profile_info.profile_idc == 10 ||
        profile_info.profile_compatibility_flag[10] == 1) {
      // max_14bit_constraint_flag  u(1)
      RETURN_EMPTY_ON_FAIL_PI(
            bit_buffer->ReadBits(&profile_info.max_14bit_constraint_flag, 1));
      // reserved_zero_33bits  u(33)
      RETURN_EMPTY_ON_FAIL_PI(bit_buffer->ReadBits(&bits_tmp_hi, 1));
      RETURN_EMPTY_ON_FAIL_PI(bit_buffer->ReadBits(&bits_tmp, 32));
      profile_info.reserved_zero_33bits =
          ((uint64_t)bits_tmp_hi << 32) | bits_tmp;
    } else {
      // reserved_zero_34bits  u(34)
      RETURN_EMPTY_ON_FAIL_PI(bit_buffer->ReadBits(&bits_tmp_hi, 2));
      RETURN_EMPTY_ON_FAIL_PI(bit_buffer->ReadBits(&bits_tmp, 32));
      profile_info.reserved_zero_34bits =
          ((uint64_t)bits_tmp_hi << 32) | bits_tmp;
    }
  } else {
    // reserved_zero_43bits  u(43)
    RETURN_EMPTY_ON_FAIL_PI(bit_buffer->ReadBits(&bits_tmp_hi, 11));
    RETURN_EMPTY_ON_FAIL_PI(bit_buffer->ReadBits(&bits_tmp, 32));
    profile_info.reserved_zero_43bits =
        ((uint64_t)bits_tmp_hi << 32) | bits_tmp;
  }
  // The number of bits in this syntax structure is not affected by
  // this condition
  if ((profile_info.profile_idc >= 1 &&
       profile_info.profile_idc <= 5) ||
      profile_info.profile_idc == 9 ||
      profile_info.profile_compatibility_flag[1] == 1 ||
      profile_info.profile_compatibility_flag[2] == 1 ||
      profile_info.profile_compatibility_flag[3] == 1 ||
      profile_info.profile_compatibility_flag[4] == 1 ||
      profile_info.profile_compatibility_flag[5] == 1 ||
      profile_info.profile_compatibility_flag[9] == 1) {
    // inbld_flag  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.inbld_flag, 1));
  } else {
    // reserved_zero_bit  u(1)
    RETURN_EMPTY_ON_FAIL_PI(
          bit_buffer->ReadBits(&profile_info.reserved_zero_bit, 1));
  }

  if (level_idc_flag) {
    // level_idc  u(8)
    RETURN_EMPTY_ON_FAIL_PI(
        bit_buffer->ReadBits(&profile_info.level_idc, 8));
  }

  return OptionalProfileInfo(profile_info);
}

}  // namespace h265nal
