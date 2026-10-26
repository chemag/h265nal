/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_scaling_list_data_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "rtc_common.h"

namespace h265nal {

class H265ScalingListDataParserTest : public ::testing::Test {
 public:
  H265ScalingListDataParserTest() {}
  ~H265ScalingListDataParserTest() override {}
};

TEST_F(H265ScalingListDataParserTest, TestSampleScalingListData01) {
  // scaling_list_data
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  };
  // fuzzer::conv: begin
  auto scaling_list_data = H265ScalingListDataParser::ParseScalingListData(
      buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(scaling_list_data != nullptr);

  // scaling_list_pred_mode_flag
  EXPECT_EQ(4, scaling_list_data->scaling_list_pred_mode_flag.size());
  EXPECT_THAT(scaling_list_data->scaling_list_pred_mode_flag[0],
              ::testing::ElementsAreArray({1, 1, 1, 1, 1, 1}));
  EXPECT_THAT(scaling_list_data->scaling_list_pred_mode_flag[1],
              ::testing::ElementsAreArray({1, 1, 1, 1, 1, 1}));
  EXPECT_THAT(scaling_list_data->scaling_list_pred_mode_flag[2],
              ::testing::ElementsAreArray({1, 1, 1, 1, 1, 1}));
  EXPECT_THAT(scaling_list_data->scaling_list_pred_mode_flag[3],
              ::testing::ElementsAreArray({1, 0, 0, 1, 0, 0}));

  // scaling_list_pred_matrix_id_delta[]
  EXPECT_EQ(4, scaling_list_data->scaling_list_pred_matrix_id_delta.size());
  EXPECT_THAT(scaling_list_data->scaling_list_pred_matrix_id_delta[0],
              ::testing::ElementsAreArray({0, 0, 0, 0, 0, 0}));
  EXPECT_THAT(scaling_list_data->scaling_list_pred_matrix_id_delta[1],
              ::testing::ElementsAreArray({0, 0, 0, 0, 0, 0}));
  EXPECT_THAT(scaling_list_data->scaling_list_pred_matrix_id_delta[2],
              ::testing::ElementsAreArray({0, 0, 0, 0, 0, 0}));
  EXPECT_THAT(scaling_list_data->scaling_list_pred_matrix_id_delta[3],
              ::testing::ElementsAreArray({0, 0, 0, 0, 0, 0}));

  // scaling_list_dc_coef_minus8[]
  EXPECT_EQ(4, scaling_list_data->scaling_list_dc_coef_minus8.size());
  EXPECT_THAT(scaling_list_data->scaling_list_dc_coef_minus8[0],
              ::testing::ElementsAreArray({0, 0, 0, 0, 0, 0}));
  EXPECT_THAT(scaling_list_data->scaling_list_dc_coef_minus8[1],
              ::testing::ElementsAreArray({0, 0, 0, 0, 0, 0}));
  EXPECT_THAT(scaling_list_data->scaling_list_dc_coef_minus8[2],
              ::testing::ElementsAreArray({0, 0, 0, 0, 0, 0}));
  EXPECT_THAT(scaling_list_data->scaling_list_dc_coef_minus8[3],
              ::testing::ElementsAreArray({0, 0, 0, 0, 0, 0}));

  // ScalingList[]
  EXPECT_EQ(4, scaling_list_data->ScalingList.size());

  // ScalingList[0]
  EXPECT_EQ(6, scaling_list_data->ScalingList[0].size());
  for (uint32_t matrixId = 0; matrixId < 6; matrixId += 1) {
    EXPECT_THAT(scaling_list_data->ScalingList[0][matrixId],
                ::testing::ElementsAreArray({8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8}));
  }

  // ScalingList[1]
  EXPECT_EQ(6, scaling_list_data->ScalingList[1].size());
  for (uint32_t matrixId = 0; matrixId < 6; matrixId += 1) {
    EXPECT_THAT(scaling_list_data->ScalingList[1][matrixId],
                ::testing::ElementsAreArray({8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8}));
  }

  // ScalingList[2]
  EXPECT_EQ(6, scaling_list_data->ScalingList[2].size());
  for (uint32_t matrixId = 0; matrixId < 6; matrixId += 1) {
    EXPECT_THAT(scaling_list_data->ScalingList[2][matrixId],
                ::testing::ElementsAreArray({8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8,
                                             8, 8, 8, 8, 8, 8, 8, 8}));
  }

  // ScalingList[3]
  EXPECT_EQ(6, scaling_list_data->ScalingList[3].size());
  EXPECT_THAT(scaling_list_data->ScalingList[3][0],
              ::testing::ElementsAreArray({8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8}));
  EXPECT_EQ(0, scaling_list_data->ScalingList[3][1].size());
  EXPECT_EQ(0, scaling_list_data->ScalingList[3][2].size());
  EXPECT_THAT(scaling_list_data->ScalingList[3][3],
              ::testing::ElementsAreArray({8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8,
                                           8, 8, 8, 8, 8, 8, 8, 8}));
  EXPECT_EQ(0, scaling_list_data->ScalingList[3][4].size());
  EXPECT_EQ(0, scaling_list_data->ScalingList[3][5].size());
}

TEST_F(H265ScalingListDataParserTest, TestScalingListDeltaCoefOutOfRange) {
  // A synthetic scaling_list_data() whose first scaling_list_delta_coef is
  // 128, one past the +127 that Section 7.4.5 allows. nextCoef is derived
  // from it in an int32_t, so an unbounded value overflows.
  //
  // The rest of the structure is well formed: the other 15 coefficients of
  // the first list are zero, and the remaining 19 matrices are predicted
  // with a delta of zero. So the parse only fails on the range check, and
  // the test still means something if that check is removed.
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x80, 0x40, 0x3f, 0xff, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
  };
  // fuzzer::conv: begin
  auto scaling_list_data = H265ScalingListDataParser::ParseScalingListData(
      buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(scaling_list_data == nullptr);
}

TEST_F(H265ScalingListDataParserTest, TestScalingListDcCoefMinus8OutOfRange) {
  // A synthetic scaling_list_data() whose only explicit list (sizeId 2,
  // matrixId 0) has scaling_list_dc_coef_minus8 equal to 248, one past the
  // +247 that Section 7.4.5 allows. nextCoef is derived from it, and then
  // used in an int32_t sum, so an unbounded value overflows.
  //
  // The rest of the structure is well formed: the 64 coefficients of that
  // list are zero, and the other 19 matrices are predicted with a delta of
  // zero. So the parse only fails on the range check, and the test still
  // means something if that check is removed.
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x55, 0x55, 0x55, 0x80, 0x7c, 0x3f, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xd5, 0x55
  };
  // fuzzer::conv: begin
  auto scaling_list_data = H265ScalingListDataParser::ParseScalingListData(
      buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(scaling_list_data == nullptr);
}

TEST_F(H265ScalingListDataParserTest, TestScalingListDcCoefMinus8BelowRange) {
  // Same structure, with scaling_list_dc_coef_minus8 equal to -8, one below
  // the -7 that Section 7.4.5 allows.
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x55, 0x55, 0x55, 0x84, 0x7f, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xd5, 0x55
  };
  // fuzzer::conv: begin
  auto scaling_list_data = H265ScalingListDataParser::ParseScalingListData(
      buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(scaling_list_data == nullptr);
}

TEST_F(H265ScalingListDataParserTest, TestScalingListDcCoefMinus8MaxValue) {
  // Same structure, with scaling_list_dc_coef_minus8 equal to 247, the
  // largest value Section 7.4.5 allows: the DC coefficient is 255, and all
  // the (zero-delta) coefficients of the list stay at 255.
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x55, 0x55, 0x55, 0x80, 0x7b, 0xbf, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xd5, 0x55
  };
  // fuzzer::conv: begin
  auto scaling_list_data = H265ScalingListDataParser::ParseScalingListData(
      buffer, arraysize(buffer));
  // fuzzer::conv: end

  ASSERT_TRUE(scaling_list_data != nullptr);
  EXPECT_EQ(247, scaling_list_data->scaling_list_dc_coef_minus8[0][0]);
  EXPECT_EQ(1, scaling_list_data->scaling_list_pred_mode_flag[2][0]);
  ASSERT_EQ(64, scaling_list_data->ScalingList[2][0].size());
  for (const auto& coef : scaling_list_data->ScalingList[2][0]) {
    EXPECT_EQ(255, coef);
  }
}

}  // namespace h265nal
