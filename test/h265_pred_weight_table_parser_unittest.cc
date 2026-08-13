/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_pred_weight_table_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "h265_slice_parser.h"
#include "rtc_common.h"

namespace h265nal {

class H265PredWeightTableParserTest : public ::testing::Test {
 public:
  H265PredWeightTableParserTest() {}
  ~H265PredWeightTableParserTest() override {}
};

TEST_F(H265PredWeightTableParserTest, TestSamplePredWeightTable1) {
  // pred_weight_table
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x10, 0xc6};
  // fuzzer::conv: begin
  auto pred_weight_table = H265PredWeightTableParser::ParsePredWeightTable(
      buffer, arraysize(buffer), 1, SliceType_P, 0, 0);
  // fuzzer::conv: end

  EXPECT_TRUE(pred_weight_table != nullptr);

  EXPECT_EQ(7, pred_weight_table->luma_log2_weight_denom);
  EXPECT_EQ(-1, pred_weight_table->delta_chroma_log2_weight_denom);
  EXPECT_THAT(pred_weight_table->luma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
  EXPECT_THAT(pred_weight_table->chroma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
}

TEST_F(H265PredWeightTableParserTest, TestSamplePredWeightTable2) {
  // pred_weight_table
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x10, 0xc0, 0x60};
  // fuzzer::conv: begin
  auto pred_weight_table = H265PredWeightTableParser::ParsePredWeightTable(
      buffer, arraysize(buffer), 1, SliceType_P, 2, 0);
  // fuzzer::conv: end

  EXPECT_TRUE(pred_weight_table != nullptr);

  EXPECT_EQ(7, pred_weight_table->luma_log2_weight_denom);
  EXPECT_EQ(-1, pred_weight_table->delta_chroma_log2_weight_denom);
  EXPECT_THAT(pred_weight_table->luma_weight_l0_flag,
              ::testing::ElementsAreArray({0, 0, 0}));
  EXPECT_THAT(pred_weight_table->chroma_weight_l0_flag,
              ::testing::ElementsAreArray({0, 0, 0}));
}

TEST_F(H265PredWeightTableParserTest, TestSamplePredWeightTableBSlice) {
  // The pred_weight_table() of the second slice of
  // pps_a_qualcomm_7.bit (hevc_v1 conformance suite): a B slice, so the
  // l1 half of Section 7.3.6.3 is present too. The 6 bits are
  // luma_log2_weight_denom, delta_chroma_log2_weight_denom, and then one
  // flag each for l0 luma, l0 chroma, l1 luma, and l1 chroma.
  // fuzzer::conv: data
  const uint8_t buffer[] = {0xc0};
  // fuzzer::conv: begin
  auto pred_weight_table = H265PredWeightTableParser::ParsePredWeightTable(
      buffer, arraysize(buffer), 1, SliceType_B, 0, 0);
  // fuzzer::conv: end

  EXPECT_TRUE(pred_weight_table != nullptr);

  EXPECT_EQ(0, pred_weight_table->luma_log2_weight_denom);
  EXPECT_EQ(0, pred_weight_table->delta_chroma_log2_weight_denom);
  EXPECT_THAT(pred_weight_table->luma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
  EXPECT_THAT(pred_weight_table->chroma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
  EXPECT_THAT(pred_weight_table->luma_weight_l1_flag,
              ::testing::ElementsAreArray({0}));
  EXPECT_THAT(pred_weight_table->chroma_weight_l1_flag,
              ::testing::ElementsAreArray({0}));
}

TEST_F(H265PredWeightTableParserTest, TestSyntheticPredWeightTableBSliceWeights) {
  // A synthetic pred_weight_table(): every bitstream in the 201810
  // conformance suite that carries an l1 table leaves all of its weight
  // flags unset, so the delta and offset loops of Section 7.3.6.3 never
  // run. This buffer sets them, with a different value in each field so
  // that a swapped or skipped read shows up.
  //
  // num_ref_idx_l0_active_minus1 is 1 and num_ref_idx_l1_active_minus1
  // is 0, and the weight flags are:
  //   luma_weight_l0_flag   { 1 0 }    chroma_weight_l0_flag { 0 1 }
  //   luma_weight_l1_flag   { 1 }      chroma_weight_l1_flag { 1 }
  // so l0 carries luma values for i=0 and chroma values for i=1, and l1
  // carries both for i=0.
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x3b, 0x92, 0x99, 0x09, 0x38, 0xac,
      0x40, 0xd2, 0x0e, 0x16, 0x80
  };
  // fuzzer::conv: begin
  auto pred_weight_table = H265PredWeightTableParser::ParsePredWeightTable(
      buffer, arraysize(buffer), 1, SliceType_B, 1, 0);
  // fuzzer::conv: end

  EXPECT_TRUE(pred_weight_table != nullptr);

  EXPECT_EQ(6, pred_weight_table->luma_log2_weight_denom);
  EXPECT_EQ(-1, pred_weight_table->delta_chroma_log2_weight_denom);

  EXPECT_THAT(pred_weight_table->luma_weight_l0_flag,
              ::testing::ElementsAreArray({1, 0}));
  EXPECT_THAT(pred_weight_table->chroma_weight_l0_flag,
              ::testing::ElementsAreArray({0, 1}));
  EXPECT_THAT(pred_weight_table->delta_luma_weight_l0,
              ::testing::ElementsAreArray({-2}));
  EXPECT_THAT(pred_weight_table->luma_offset_l0,
              ::testing::ElementsAreArray({3}));
  EXPECT_THAT(pred_weight_table->delta_chroma_weight_l0,
              ::testing::ElementsAre(::testing::ElementsAreArray({1, -3})));
  EXPECT_THAT(pred_weight_table->delta_chroma_offset_l0,
              ::testing::ElementsAre(::testing::ElementsAreArray({-4, 5})));

  EXPECT_THAT(pred_weight_table->luma_weight_l1_flag,
              ::testing::ElementsAreArray({1}));
  EXPECT_THAT(pred_weight_table->chroma_weight_l1_flag,
              ::testing::ElementsAreArray({1}));
  EXPECT_THAT(pred_weight_table->delta_luma_weight_l1,
              ::testing::ElementsAreArray({4}));
  EXPECT_THAT(pred_weight_table->luma_offset_l1,
              ::testing::ElementsAreArray({-6}));
  EXPECT_THAT(pred_weight_table->delta_chroma_weight_l1,
              ::testing::ElementsAre(::testing::ElementsAreArray({2, -5})));
  EXPECT_THAT(pred_weight_table->delta_chroma_offset_l1,
              ::testing::ElementsAre(::testing::ElementsAreArray({7, 1})));
}

}  // namespace h265nal
