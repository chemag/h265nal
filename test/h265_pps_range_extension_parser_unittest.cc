/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_pps_range_extension_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "rtc_common.h"

namespace h265nal {

class H265PpsRangeExtensionParserTest : public ::testing::Test {
 public:
  H265PpsRangeExtensionParserTest() {}
  ~H265PpsRangeExtensionParserTest() override {}
};

TEST_F(H265PpsRangeExtensionParserTest, TestSamplePpsRangeExtension) {
  // pps_range_extension, from sao_a_rext_mediatek_1.bit of the 201810
  // conformance suite. transform_skip_enabled_flag is set in the enclosing
  // pps, so log2_max_transform_skip_block_size_minus2 is present.
  // fuzzer::conv: data
  const uint8_t buffer[] = {0xcd, 0x80};
  // fuzzer::conv: begin
  auto pps_range_extension =
      H265PpsRangeExtensionParser::ParsePpsRangeExtension(
          buffer, arraysize(buffer), 1);
  // fuzzer::conv: end

  EXPECT_TRUE(pps_range_extension != nullptr);

  EXPECT_EQ(1, pps_range_extension->transform_skip_enabled_flag);
  EXPECT_EQ(0, pps_range_extension->log2_max_transform_skip_block_size_minus2);
  EXPECT_EQ(1, pps_range_extension->cross_component_prediction_enabled_flag);
  EXPECT_EQ(0, pps_range_extension->chroma_qp_offset_list_enabled_flag);
  EXPECT_EQ(2, pps_range_extension->log2_sao_offset_scale_luma);
  EXPECT_EQ(2, pps_range_extension->log2_sao_offset_scale_chroma);
}

TEST_F(H265PpsRangeExtensionParserTest, TestPpsRangeExtensionChromaQpOffsets) {
  // pps_range_extension, from main_422_10_a_rext_sony_2.bin of the 201810
  // conformance suite. This is the branch that
  // chroma_qp_offset_list_enabled_flag opens, which also decides whether
  // the slice segment header reads cu_chroma_qp_offset_enabled_flag.
  // fuzzer::conv: data
  const uint8_t buffer[] = {0xa4, 0x8c, 0x19, 0x80};
  // fuzzer::conv: begin
  auto pps_range_extension =
      H265PpsRangeExtensionParser::ParsePpsRangeExtension(
          buffer, arraysize(buffer), 1);
  // fuzzer::conv: end

  EXPECT_TRUE(pps_range_extension != nullptr);

  EXPECT_EQ(0, pps_range_extension->log2_max_transform_skip_block_size_minus2);
  EXPECT_EQ(0, pps_range_extension->cross_component_prediction_enabled_flag);
  EXPECT_EQ(1, pps_range_extension->chroma_qp_offset_list_enabled_flag);
  EXPECT_EQ(3, pps_range_extension->diff_cu_chroma_qp_offset_depth);
  EXPECT_EQ(0, pps_range_extension->chroma_qp_offset_list_len_minus1);
  ASSERT_EQ(1, pps_range_extension->cb_qp_offset_list.size());
  EXPECT_EQ(6, pps_range_extension->cb_qp_offset_list[0]);
  ASSERT_EQ(1, pps_range_extension->cr_qp_offset_list.size());
  EXPECT_EQ(6, pps_range_extension->cr_qp_offset_list[0]);
  EXPECT_EQ(0, pps_range_extension->log2_sao_offset_scale_luma);
  EXPECT_EQ(0, pps_range_extension->log2_sao_offset_scale_chroma);
}

TEST_F(H265PpsRangeExtensionParserTest, TestPpsRangeExtensionNoTransformSkip) {
  // A synthetic pps_range_extension with transform_skip_enabled_flag unset
  // in the enclosing pps, so log2_max_transform_skip_block_size_minus2 is
  // absent. Same payload as the sample above with that element removed:
  // reading it anyway would shift everything after it.
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x9b};
  // fuzzer::conv: begin
  auto pps_range_extension =
      H265PpsRangeExtensionParser::ParsePpsRangeExtension(
          buffer, arraysize(buffer), 0);
  // fuzzer::conv: end

  EXPECT_TRUE(pps_range_extension != nullptr);

  EXPECT_EQ(0, pps_range_extension->transform_skip_enabled_flag);
  EXPECT_EQ(0, pps_range_extension->log2_max_transform_skip_block_size_minus2);
  EXPECT_EQ(1, pps_range_extension->cross_component_prediction_enabled_flag);
  EXPECT_EQ(0, pps_range_extension->chroma_qp_offset_list_enabled_flag);
  EXPECT_EQ(2, pps_range_extension->log2_sao_offset_scale_luma);
  EXPECT_EQ(2, pps_range_extension->log2_sao_offset_scale_chroma);
}

TEST_F(H265PpsRangeExtensionParserTest, TestPpsRangeExtensionListLenTooLong) {
  // A synthetic pps_range_extension whose chroma_qp_offset_list_len_minus1
  // is 6. Section 7.4.3.3.2 puts it in the range of 0 to 5, inclusive, and
  // it bounds the loop that fills cb_qp_offset_list and cr_qp_offset_list.
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x67};
  // fuzzer::conv: begin
  auto pps_range_extension =
      H265PpsRangeExtensionParser::ParsePpsRangeExtension(
          buffer, arraysize(buffer), 0);
  // fuzzer::conv: end

  EXPECT_TRUE(pps_range_extension == nullptr);
}

}  // namespace h265nal
