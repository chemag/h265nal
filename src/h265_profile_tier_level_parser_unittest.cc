/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "h265_profile_tier_level_parser.h"

#include <gtest/gtest.h>

#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h265nal {

class H265ProfileTierLevelParserTest : public ::testing::Test {
 public:
  H265ProfileTierLevelParserTest() {}
  ~H265ProfileTierLevelParserTest() override {}

  absl::optional<H265ProfileTierLevelParser::ProfileTierLevelState> ptls_;
};

TEST_F(H265ProfileTierLevelParserTest, TestSampleValue) {
  // VPS for a 1280x720 camera capture.
  const uint8_t buffer[] = {0x0c, 0x01, 0xff, 0xff, 0x01, 0x60, 0x00, 0x00,
                            0x03, 0x00, 0xb0, 0x00, 0x00, 0x03,
                            0x00, 0x00, 0x03, 0x00, 0x5a, 0xac, 0x09};
  ptls_ = H265ProfileTierLevelParser::ParseProfileTierLevel(
      buffer, arraysize(buffer), true, 0);
  EXPECT_TRUE(ptls_ != absl::nullopt);
  EXPECT_EQ(0, ptls_->general.profile_space);
  EXPECT_EQ(0, ptls_->general.tier_flag);
  EXPECT_EQ(12, ptls_->general.profile_idc);
  for (int i = 0; i < 32; i++) {
    // {0, 0, 0, 0, 0, 0, 0, 1 <repeats 17 times>, 0, 0, 0, 0, 0, 0, 0, 1}
    int expected_value = 0;
    if ((i >= 7 && i <= 23) || i == 31) {
      expected_value = 1;
    }
    EXPECT_EQ(expected_value, ptls_->general.profile_compatibility_flag[i]);
  }
  EXPECT_EQ(0, ptls_->general.progressive_source_flag);
  EXPECT_EQ(1, ptls_->general.interlaced_source_flag);
  EXPECT_EQ(1, ptls_->general.non_packed_constraint_flag);
  EXPECT_EQ(0, ptls_->general.frame_only_constraint_flag);
  EXPECT_EQ(0, ptls_->general.max_12bit_constraint_flag);
  EXPECT_EQ(0, ptls_->general.max_10bit_constraint_flag);
  EXPECT_EQ(0, ptls_->general.max_8bit_constraint_flag);
  EXPECT_EQ(0, ptls_->general.max_422chroma_constraint_flag);
  EXPECT_EQ(0, ptls_->general.max_420chroma_constraint_flag);
  EXPECT_EQ(0, ptls_->general.max_monochrome_constraint_flag);
  EXPECT_EQ(0, ptls_->general.intra_constraint_flag);
  EXPECT_EQ(0, ptls_->general.one_picture_only_constraint_flag);
  EXPECT_EQ(0, ptls_->general.lower_bit_rate_constraint_flag);
  EXPECT_EQ(0, ptls_->general.max_14bit_constraint_flag);
  EXPECT_EQ(0x5800, ptls_->general.reserved_zero_33bits);
  EXPECT_EQ(0, ptls_->general.reserved_zero_34bits);
  EXPECT_EQ(0, ptls_->general.reserved_zero_43bits);
  EXPECT_EQ(0, ptls_->general.inbld_flag);
  EXPECT_EQ(0, ptls_->general.reserved_zero_bit);
  EXPECT_EQ(0, ptls_->general.level_idc);
}

}  // namespace h265nal
