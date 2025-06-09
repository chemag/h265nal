/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_profile_tier_level_parser.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h265_common.h"

namespace h265nal {

// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Unpack RBSP and parse PROFILE_TIER_LEVEL state from the supplied buffer.
std::unique_ptr<H265ProfileTierLevelParser::ProfileTierLevelState>
H265ProfileTierLevelParser::ParseProfileTierLevel(
    const uint8_t* data, size_t length, const bool profilePresentFlag,
    const unsigned int maxNumSubLayersMinus1) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());

  return ParseProfileTierLevel(&bit_buffer, profilePresentFlag,
                               maxNumSubLayersMinus1);
}

std::unique_ptr<H265ProfileTierLevelParser::ProfileTierLevelState>
H265ProfileTierLevelParser::ParseProfileTierLevel(
    BitBuffer* bit_buffer, const bool profilePresentFlag,
    const unsigned int maxNumSubLayersMinus1) noexcept {
  uint32_t bits_tmp;

  // profile_tier_level() parser.
  // Section 7.3.3 ("Profile, tier and level syntax") of the H.265
  // standard for a complete description.
  auto profile_tier_level = std::make_unique<ProfileTierLevelState>();

  // input parameters
  profile_tier_level->profilePresentFlag = profilePresentFlag;
  profile_tier_level->maxNumSubLayersMinus1 = maxNumSubLayersMinus1;

  if (profilePresentFlag) {
    profile_tier_level->general =
        H265ProfileInfoParser::ParseProfileInfo(bit_buffer);
    if (profile_tier_level->general == nullptr) {
      return nullptr;
    }
  }

  // general_level_idc  u(8)
  if (!bit_buffer->ReadBits(8, profile_tier_level->general_level_idc)) {
    return nullptr;
  }

  for (uint32_t i = 0; i < maxNumSubLayersMinus1; i++) {
    // sub_layer_profile_present_flag[i]  u(1)
    if (!bit_buffer->ReadBits(1, bits_tmp)) {
      return nullptr;
    }
    profile_tier_level->sub_layer_profile_present_flag.push_back(bits_tmp);

    // sub_layer_level_present_flag[i]  u(1)
    if (!bit_buffer->ReadBits(1, bits_tmp)) {
      return nullptr;
    }
    profile_tier_level->sub_layer_level_present_flag.push_back(bits_tmp);
  }

  if (maxNumSubLayersMinus1 > 0) {
    for (uint32_t i = maxNumSubLayersMinus1; i < 8; i++) {
      // reserved_zero_2bits[i]  u(2)
      if (!bit_buffer->ReadBits(2, bits_tmp)) {
        return nullptr;
      }
      profile_tier_level->reserved_zero_2bits.push_back(bits_tmp);
    }
  }

  for (uint32_t i = 0; i < maxNumSubLayersMinus1; i++) {
    if (profile_tier_level->sub_layer_profile_present_flag[i]) {
      profile_tier_level->sub_layer.push_back(
          H265ProfileInfoParser::ParseProfileInfo(bit_buffer));
      if (profile_tier_level->sub_layer.back() == nullptr) {
        return nullptr;
      }
    }

    if (profile_tier_level->sub_layer_profile_present_flag[i]) {
      // sub_layer_level_idc  u(8)
      if (!bit_buffer->ReadBits(8, bits_tmp)) {
        return nullptr;
      }
      profile_tier_level->sub_layer_level_idc.push_back(bits_tmp);
    }
  }

  return profile_tier_level;
}

std::unique_ptr<H265ProfileInfoParser::ProfileInfoState>
H265ProfileInfoParser::ParseProfileInfo(const uint8_t* data,
                                        size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseProfileInfo(&bit_buffer);
}

std::unique_ptr<H265ProfileInfoParser::ProfileInfoState>
H265ProfileInfoParser::ParseProfileInfo(BitBuffer* bit_buffer) noexcept {
  auto profile_info = std::make_unique<ProfileInfoState>();
  uint32_t bits_tmp, bits_tmp_hi;

  // profile_space  u(2)
  if (!bit_buffer->ReadBits(2, profile_info->profile_space)) {
    return nullptr;
  }
  // tier_flag  u(1)
  if (!bit_buffer->ReadBits(1, profile_info->tier_flag)) {
    return nullptr;
  }
  // profile_idc  u(5)
  if (!bit_buffer->ReadBits(5, profile_info->profile_idc)) {
    return nullptr;
  }
  // for (j = 0; j < 32; j++)
  for (uint32_t j = 0; j < 32; j++) {
    // profile_compatibility_flag[j]  u(1)
    if (!bit_buffer->ReadBits(1, profile_info->profile_compatibility_flag[j])) {
      return nullptr;
    }
  }

  // progressive_source_flag  u(1)
  if (!bit_buffer->ReadBits(1, profile_info->progressive_source_flag)) {
    return nullptr;
  }
  // interlaced_source_flag  u(1)
  if (!bit_buffer->ReadBits(1, profile_info->interlaced_source_flag)) {
    return nullptr;
  }
  // non_packed_constraint_flag  u(1)
  if (!bit_buffer->ReadBits(1, profile_info->non_packed_constraint_flag)) {
    return nullptr;
  }
  // frame_only_constraint_flag  u(1)
  if (!bit_buffer->ReadBits(1, profile_info->frame_only_constraint_flag)) {
    return nullptr;
  }
  if (profile_info->profile_idc == 4 ||
      profile_info->profile_compatibility_flag[4] == 1 ||
      profile_info->profile_idc == 5 ||
      profile_info->profile_compatibility_flag[5] == 1 ||
      profile_info->profile_idc == 6 ||
      profile_info->profile_compatibility_flag[6] == 1 ||
      profile_info->profile_idc == 7 ||
      profile_info->profile_compatibility_flag[7] == 1 ||
      profile_info->profile_idc == 8 ||
      profile_info->profile_compatibility_flag[8] == 1 ||
      profile_info->profile_idc == 9 ||
      profile_info->profile_compatibility_flag[9] == 1 ||
      profile_info->profile_idc == 10 ||
      profile_info->profile_compatibility_flag[10] == 1) {
    // The number of bits in this syntax structure is not affected by
    // this condition
    // max_12bit_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1, profile_info->max_12bit_constraint_flag)) {
      return nullptr;
    }
    // max_10bit_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1, profile_info->max_10bit_constraint_flag)) {
      return nullptr;
    }
    // max_8bit_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1, profile_info->max_8bit_constraint_flag)) {
      return nullptr;
    }
    // max_422chroma_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1, profile_info->max_422chroma_constraint_flag)) {
      return nullptr;
    }
    // max_420chroma_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1, profile_info->max_420chroma_constraint_flag)) {
      return nullptr;
    }
    // max_monochrome_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1,
                              profile_info->max_monochrome_constraint_flag)) {
      return nullptr;
    }
    // intra_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1, profile_info->intra_constraint_flag)) {
      return nullptr;
    }
    // one_picture_only_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1,
                              profile_info->one_picture_only_constraint_flag)) {
      return nullptr;
    }
    // lower_bit_rate_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1,
                              profile_info->lower_bit_rate_constraint_flag)) {
      return nullptr;
    }
    if (profile_info->profile_idc == 5 ||
        profile_info->profile_compatibility_flag[5] == 1 ||
        profile_info->profile_idc == 9 ||
        profile_info->profile_compatibility_flag[9] == 1 ||
        profile_info->profile_idc == 10 ||
        profile_info->profile_compatibility_flag[10] == 1) {
      // max_14bit_constraint_flag  u(1)
      if (!bit_buffer->ReadBits(1, profile_info->max_14bit_constraint_flag)) {
        return nullptr;
      }
      // reserved_zero_33bits  u(33)
      if (!bit_buffer->ReadBits(1, bits_tmp_hi)) {
        return nullptr;
      }
      if (!bit_buffer->ReadBits(32, bits_tmp)) {
        return nullptr;
      }
      profile_info->reserved_zero_33bits =
          ((uint64_t)bits_tmp_hi << 32) | bits_tmp;
    } else {
      // reserved_zero_34bits  u(34)
      if (!bit_buffer->ReadBits(2, bits_tmp_hi)) {
        return nullptr;
      }
      if (!bit_buffer->ReadBits(32, bits_tmp)) {
        return nullptr;
      }
      profile_info->reserved_zero_34bits =
          ((uint64_t)bits_tmp_hi << 32) | bits_tmp;
    }
  } else if (profile_info->profile_idc == 2 ||
             profile_info->profile_compatibility_flag[2] == 1) {
    // reserved_zero_7bits  u(7)
    if (!bit_buffer->ReadBits(7, profile_info->reserved_zero_7bits)) {
      return nullptr;
    }
    // one_picture_only_constraint_flag  u(1)
    if (!bit_buffer->ReadBits(1,
                              profile_info->one_picture_only_constraint_flag)) {
      return nullptr;
    }
    // reserved_zero_35bits  u(35)
    if (!bit_buffer->ReadBits(3, bits_tmp_hi)) {
      return nullptr;
    }
    if (!bit_buffer->ReadBits(32, bits_tmp)) {
      return nullptr;
    }
    profile_info->reserved_zero_35bits =
        ((uint64_t)bits_tmp_hi << 32) | bits_tmp;
  } else {
    // reserved_zero_43bits  u(43)
    if (!bit_buffer->ReadBits(11, bits_tmp_hi)) {
      return nullptr;
    }
    if (!bit_buffer->ReadBits(32, bits_tmp)) {
      return nullptr;
    }
    profile_info->reserved_zero_43bits =
        ((uint64_t)bits_tmp_hi << 32) | bits_tmp;
  }
  // get profile type
  profile_info->profile_type = profile_info->GetProfileType();
  // The number of bits in this syntax structure is not affected by
  // this condition
  if ((profile_info->profile_idc >= 1 && profile_info->profile_idc <= 5) ||
      profile_info->profile_idc == 9 ||
      profile_info->profile_compatibility_flag[1] == 1 ||
      profile_info->profile_compatibility_flag[2] == 1 ||
      profile_info->profile_compatibility_flag[3] == 1 ||
      profile_info->profile_compatibility_flag[4] == 1 ||
      profile_info->profile_compatibility_flag[5] == 1 ||
      profile_info->profile_compatibility_flag[9] == 1) {
    // inbld_flag  u(1)
    if (!bit_buffer->ReadBits(1, profile_info->inbld_flag)) {
      return nullptr;
    }
  } else {
    // reserved_zero_bit  u(1)
    if (!bit_buffer->ReadBits(1, profile_info->reserved_zero_bit)) {
      return nullptr;
    }
  }

  return profile_info;
}

ProfileType H265ProfileInfoParser::ProfileInfoState::GetProfileType()
    const noexcept {
  // All profile checks include "general_profile_idc equal to j or
  // general_profile_compatibility_flag[j] equal to 1". Moreover, it also
  // states that "general_profile_compatibility_flag[ j ] equal to 1, when
  // general_profile_space is equal to 0, indicates that the CVS conforms
  // to the profile indicated by general_profile_idc equal to j as specified
  // in Annex A. When general_profile_space is equal to 0,
  // general_profile_compatibility_flag[ general_profile_idc ] shall be
  // equal to 1.".
  // As a safe approach, we will check for general_profile_space to be 0,
  // and then ignore the profile_compatibility_flag[j] flags.
  if (profile_space != 0) {
    return ProfileType::UNSPECIFIED;
  }

  switch (profile_idc) {
    case 1: {
      return ProfileType::MAIN;
      break;
    }
    case 2: {
      if (one_picture_only_constraint_flag == 1) {
        return MAIN_10_STILL_PICTURE;
      }
      return ProfileType::MAIN_10;
      break;
    }
    case 3: {
      return ProfileType::MAIN_STILL_PICTURE;
      break;
    }
    case 4: {
      if ((max_12bit_constraint_flag == 1) &&
          (max_10bit_constraint_flag == 1) && (max_8bit_constraint_flag == 1) &&
          (max_422chroma_constraint_flag == 1) &&
          (max_420chroma_constraint_flag == 1) &&
          (max_monochrome_constraint_flag == 1) &&
          (intra_constraint_flag == 0) &&
          (one_picture_only_constraint_flag == 0) &&
          (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MONOCHROME;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 1) &&
                 (max_monochrome_constraint_flag == 1) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MONOCHROME_10;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 1) &&
                 (max_monochrome_constraint_flag == 1) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MONOCHROME_12;
      } else if ((max_12bit_constraint_flag == 0) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 1) &&
                 (max_monochrome_constraint_flag == 1) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MONOCHROME_16;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 1) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MAIN_12;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MAIN_422_10;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MAIN_422_12;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 1) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MAIN_444;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MAIN_444_10;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::MAIN_444_12;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 1) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 1) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::MAIN_INTRA;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 1) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::MAIN_10_INTRA;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 1) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::MAIN_12_INTRA;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::MAIN_422_10_INTRA;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::MAIN_422_12_INTRA;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 1) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::MAIN_444_INTRA;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::MAIN_444_10_INTRA;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::MAIN_444_12_INTRA;
      } else if ((max_12bit_constraint_flag == 0) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::MAIN_444_16_INTRA;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 1) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 1)) {
        return ProfileType::MAIN_444_STILL_PICTURE;
      } else if ((max_12bit_constraint_flag == 0) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 1)) {
        return ProfileType::MAIN_444_16_STILL_PICTURE;
      }
      return ProfileType::FREXT;
      break;
    }
    case 5: {
      // Table A.3
      if ((max_14bit_constraint_flag == 1) &&
          (max_12bit_constraint_flag == 1) &&
          (max_10bit_constraint_flag == 1) && (max_8bit_constraint_flag == 1) &&
          (max_422chroma_constraint_flag == 0) &&
          (max_420chroma_constraint_flag == 0) &&
          (max_monochrome_constraint_flag == 0) &&
          (intra_constraint_flag == 0) &&
          (one_picture_only_constraint_flag == 0) &&
          (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::HIGH_THROUGHPUT_444;
      } else if ((max_14bit_constraint_flag == 1) &&
                 (max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::HIGH_THROUGHPUT_444_10;
      } else if ((max_14bit_constraint_flag == 1) &&
                 (max_12bit_constraint_flag == 0) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::HIGH_THROUGHPUT_444_14;
      } else if ((max_14bit_constraint_flag == 0) &&
                 (max_12bit_constraint_flag == 0) &&
                 (max_10bit_constraint_flag == 0) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 1) &&
                 (one_picture_only_constraint_flag == 0)) {
        return ProfileType::HIGH_THROUGHPUT_444_16_INTRA;
      }
      return ProfileType::HIGH_THROUGHPUT;
      break;
    }
    case 6: {
      return ProfileType::MULTIVIEW_MAIN;
      break;
    }
    case 7: {
      // Appendix H.11.1.1
      if ((max_12bit_constraint_flag == 1) &&
          (max_10bit_constraint_flag == 1) && (max_8bit_constraint_flag == 1) &&
          (max_422chroma_constraint_flag == 1) &&
          (max_420chroma_constraint_flag == 1) &&
          (max_monochrome_constraint_flag == 0) &&
          (intra_constraint_flag == 0) &&
          (one_picture_only_constraint_flag == 0) &&
          (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::SCALABLE_MAIN;
      } else if ((max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 1) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::SCALABLE_MAIN_10;
      }
      return ProfileType::SCALABLE;
      break;
    }
    case 8: {
      return ProfileType::_3D_MAIN;
      break;
    }
    case 9: {
      // Table A.5
      if ((max_14bit_constraint_flag == 1) &&
          (max_12bit_constraint_flag == 1) &&
          (max_10bit_constraint_flag == 1) && (max_8bit_constraint_flag == 1) &&
          (max_422chroma_constraint_flag == 1) &&
          (max_420chroma_constraint_flag == 1) &&
          (max_monochrome_constraint_flag == 0) &&
          (intra_constraint_flag == 0) &&
          (one_picture_only_constraint_flag == 0) &&
          (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::SCREEN_EXTENDED_MAIN;
      } else if ((max_14bit_constraint_flag == 1) &&
                 (max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 1) &&
                 (max_420chroma_constraint_flag == 1) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::SCREEN_EXTENDED_MAIN_10;
      } else if ((max_14bit_constraint_flag == 1) &&
                 (max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 1) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::SCREEN_EXTENDED_MAIN_444;
      } else if ((max_14bit_constraint_flag == 1) &&
                 (max_12bit_constraint_flag == 1) &&
                 (max_10bit_constraint_flag == 1) &&
                 (max_8bit_constraint_flag == 0) &&
                 (max_422chroma_constraint_flag == 0) &&
                 (max_420chroma_constraint_flag == 0) &&
                 (max_monochrome_constraint_flag == 0) &&
                 (intra_constraint_flag == 0) &&
                 (one_picture_only_constraint_flag == 0) &&
                 (lower_bit_rate_constraint_flag == 1)) {
        return ProfileType::SCREEN_EXTENDED_MAIN_444_10;
      }
      return ProfileType::SCREEN_EXTENDED;
      break;
    }
    case 11: {
      return ProfileType::SCREEN_EXTENDED_HIGH_THROUGHPUT;
      break;
    }
    case 12: {
      return ProfileType::MULTIVIEW_EXTENDED;
      break;
    }
    case 13: {
      return ProfileType::MULTIVIEW_EXTENDED_10;
      break;
    }
    default:
      break;
  }

  return ProfileType::UNSPECIFIED;
}

#ifdef FDUMP_DEFINE
void H265ProfileInfoParser::ProfileInfoState::fdump(FILE* outfp,
                                                    int indent_level) const {
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "profile_space: %i", profile_space);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "tier_flag: %i", tier_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "profile_idc: %i", profile_idc);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "profile_compatibility_flag {");
  for (const uint32_t& v : profile_compatibility_flag) {
    fprintf(outfp, " %i", v);
  }
  fprintf(outfp, " }");

  fdump_indent_level(outfp, indent_level);
  std::string profile_type_str;
  profileTypeToString(profile_type, profile_type_str);
  fprintf(outfp, "profile: %s", profile_type_str.c_str());

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "progressive_source_flag: %i", progressive_source_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "interlaced_source_flag: %i", interlaced_source_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "non_packed_constraint_flag: %i", non_packed_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "frame_only_constraint_flag: %i", frame_only_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_12bit_constraint_flag: %i", max_12bit_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_10bit_constraint_flag: %i", max_10bit_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_8bit_constraint_flag: %i", max_8bit_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_422chroma_constraint_flag: %i",
          max_422chroma_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_420chroma_constraint_flag: %i",
          max_420chroma_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_monochrome_constraint_flag: %i",
          max_monochrome_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "intra_constraint_flag: %i", intra_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "one_picture_only_constraint_flag: %i",
          one_picture_only_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "lower_bit_rate_constraint_flag: %i",
          lower_bit_rate_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_14bit_constraint_flag: %i", max_14bit_constraint_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved_zero_33bits: %" PRIu64 "", reserved_zero_33bits);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved_zero_34bits: %" PRIu64 "", reserved_zero_34bits);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved_zero_7bits: %" PRIu32 "", reserved_zero_7bits);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved_zero_35bits: %" PRIu64 "", reserved_zero_35bits);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved_zero_43bits: %" PRIu64 "", reserved_zero_43bits);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "inbld_flag: %i", inbld_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved_zero_bit: %i", reserved_zero_bit);
}

void H265ProfileTierLevelParser::ProfileTierLevelState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "profile_tier_level {");
  indent_level = indent_level_incr(indent_level);

  if (profilePresentFlag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "general {");
    indent_level = indent_level_incr(indent_level);

    general->fdump(outfp, indent_level);

    indent_level = indent_level_decr(indent_level);
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "}");
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "general_level_idc: %i", general_level_idc);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "sub_layer_profile_present_flag {");
  for (const uint32_t& v : sub_layer_profile_present_flag) {
    fprintf(outfp, " %i", v);
  }
  fprintf(outfp, " }");

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "sub_layer_level_present_flag {");
  for (const uint32_t& v : sub_layer_level_present_flag) {
    fprintf(outfp, " %i ", v);
  }
  fprintf(outfp, " }");

  if (maxNumSubLayersMinus1 > 0) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "reserved_zero_2bits {");
    for (const uint32_t& v : reserved_zero_2bits) {
      fprintf(outfp, " %i ", v);
    }
    fprintf(outfp, " }");
  }

  if (maxNumSubLayersMinus1 > 0) {
    for (auto& v : sub_layer) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "sub_layer {");
      indent_level = indent_level_incr(indent_level);

      v->fdump(outfp, indent_level);

      indent_level = indent_level_decr(indent_level);
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "}");
    }

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "sub_layer_level_idc {");
    for (const uint32_t& v : sub_layer_level_idc) {
      fprintf(outfp, " %i ", v);
    }
    fprintf(outfp, " }");
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
