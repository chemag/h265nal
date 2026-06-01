/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_st_ref_pic_set_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "h265_sps_parser.h"
#include "rtc_common.h"

namespace h265nal {

class H265StRefPicSetParserTest : public ::testing::Test {
 public:
  H265StRefPicSetParserTest() {}
  ~H265StRefPicSetParserTest() override {}
};

TEST_F(H265StRefPicSetParserTest, TestSampleStRefPicSet) {
  // st_ref_pic_set
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x5d};
  // fuzzer::conv: begin
  auto sps = std::make_shared<H265SpsParser::SpsState>();
  sps->num_short_term_ref_pic_sets = 0;
  uint32_t max_num_pics = 1;
  auto st_ref_pic_set = H265StRefPicSetParser::ParseStRefPicSet(
      buffer, arraysize(buffer), 0, 1, &(sps->st_ref_pic_set), max_num_pics);
  // fuzzer::conv: end

  EXPECT_TRUE(st_ref_pic_set != nullptr);

  EXPECT_EQ(1, st_ref_pic_set->num_negative_pics);
  EXPECT_EQ(0, st_ref_pic_set->num_positive_pics);
  EXPECT_THAT(st_ref_pic_set->delta_poc_s0_minus1,
              ::testing::ElementsAreArray({0}));
  EXPECT_THAT(st_ref_pic_set->delta_poc_s0_minus1,
              ::testing::ElementsAreArray({0}));
}

TEST_F(H265StRefPicSetParserTest, TestDeriveValuesOobBoundsCheck) {
  // Verify that DeriveValues() rejects inputs that would overflow the
  // stack-allocated arrays of size HEVC_MAX_REFS (16).
  // Construct a reference with 8 negative + 8 positive = 16 pics and
  // call DeriveValues() directly to bypass the ParseStRefPicSet
  // NumDeltaPocs_RefRpsIdx < HEVC_MAX_DPB_SIZE guard.
  // With a large negative delta_rps, DeriveValues would accumulate
  // i = 8 (positive) + 1 (delta_rps) + 8 (negative) = 17 writes,
  // overflowing index 15. The bounds check must return false.
  auto sps = std::make_shared<H265SpsParser::SpsState>();

  auto ref = std::make_unique<H265StRefPicSetParser::StRefPicSetState>();
  ref->num_negative_pics = 8;
  ref->num_positive_pics = 8;
  for (uint32_t i = 0; i < 8; i++) {
    ref->delta_poc_s0_minus1.push_back(0);
    ref->used_by_curr_pic_s0_flag.push_back(1);
    ref->delta_poc_s1_minus1.push_back(0);
    ref->used_by_curr_pic_s1_flag.push_back(1);
  }
  sps->st_ref_pic_set.push_back(std::move(ref));

  // Set up a target StRefPicSetState that uses inter prediction
  H265StRefPicSetParser::StRefPicSetState target;
  target.inter_ref_pic_set_prediction_flag = 1;
  target.delta_rps_sign = 1;
  target.abs_delta_rps_minus1 = 99;
  // NumDeltaPocs_RefRpsIdx = 16, so we need 17 flag entries (0..16)
  for (uint32_t i = 0; i <= 16; i++) {
    target.used_by_curr_pic_flag.push_back(1);
    target.use_delta_flag.push_back(1);
  }

  bool result = target.DeriveValues(&(sps->st_ref_pic_set), 0);
  EXPECT_FALSE(result);
}

}  // namespace h265nal
