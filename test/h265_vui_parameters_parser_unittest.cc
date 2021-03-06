/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_vui_parameters_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h265nal {

class H265VuiParametersParserTest : public ::testing::Test {
 public:
  H265VuiParametersParserTest() {}
  ~H265VuiParametersParserTest() override {}
};

TEST_F(H265VuiParametersParserTest, TestSampleVuiParameters) {
  // VUI
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x2b, 0x05, 0x06, 0x06, 0x02, 0xed, 0x0a, 0x12
  };
  // fuzzer::conv: begin
  auto vui_parameters =
      H265VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(vui_parameters != nullptr);

  EXPECT_EQ(0, vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(0, vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(1, vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(2, vui_parameters->video_format);
  EXPECT_EQ(1, vui_parameters->video_full_range_flag);
  EXPECT_EQ(1, vui_parameters->colour_description_present_flag);
  EXPECT_EQ(5, vui_parameters->colour_primaries);
  EXPECT_EQ(6, vui_parameters->transfer_characteristics);
  EXPECT_EQ(6, vui_parameters->matrix_coeffs);
  EXPECT_EQ(0, vui_parameters->chroma_loc_info_present_flag);
  EXPECT_EQ(0, vui_parameters->neutral_chroma_indication_flag);
  EXPECT_EQ(0, vui_parameters->field_seq_flag);
  EXPECT_EQ(0, vui_parameters->frame_field_info_present_flag);
  EXPECT_EQ(0, vui_parameters->default_display_window_flag);
  EXPECT_EQ(0, vui_parameters->vui_timing_info_present_flag);
  EXPECT_EQ(1, vui_parameters->bitstream_restriction_flag);
  EXPECT_EQ(0, vui_parameters->tiles_fixed_structure_flag);
  EXPECT_EQ(1, vui_parameters->motion_vectors_over_pic_boundaries_flag);
  EXPECT_EQ(1, vui_parameters->restricted_ref_pic_lists_flag);
  EXPECT_EQ(0, vui_parameters->min_spatial_segmentation_idc);
  EXPECT_EQ(2, vui_parameters->max_bytes_per_pic_denom);
  EXPECT_EQ(1, vui_parameters->max_bits_per_min_cu_denom);
  EXPECT_EQ(9, vui_parameters->log2_max_mv_length_horizontal);
  EXPECT_EQ(8, vui_parameters->log2_max_mv_length_vertical);
}

TEST_F(H265VuiParametersParserTest, TestSampleVuiParameters2) {
  // VUI
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0xff, 0x80, 0x40, 0x00, 0x3a, 0xb4, 0x04, 0x00,
      0x00, 0x0f, 0xa4, 0x00, 0x01, 0xd4, 0xc0, 0x20
  };
  // fuzzer::conv: begin
  auto vui_parameters =
      H265VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(vui_parameters != nullptr);

  EXPECT_EQ(1, vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(255, vui_parameters->aspect_ratio_idc);
  EXPECT_EQ(128, vui_parameters->sar_width);
  EXPECT_EQ(117, vui_parameters->sar_height);
  EXPECT_EQ(0, vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(1, vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(5, vui_parameters->video_format);
  EXPECT_EQ(0, vui_parameters->video_full_range_flag);
  EXPECT_EQ(0, vui_parameters->colour_description_present_flag);
  EXPECT_EQ(0, vui_parameters->chroma_loc_info_present_flag);
  EXPECT_EQ(0, vui_parameters->neutral_chroma_indication_flag);
  EXPECT_EQ(0, vui_parameters->field_seq_flag);
  EXPECT_EQ(0, vui_parameters->frame_field_info_present_flag);
  EXPECT_EQ(0, vui_parameters->default_display_window_flag);
  EXPECT_EQ(1, vui_parameters->vui_timing_info_present_flag);
  EXPECT_EQ(1001, vui_parameters->vui_num_units_in_tick);
  EXPECT_EQ(30000, vui_parameters->vui_time_scale);
  EXPECT_EQ(0, vui_parameters->vui_poc_proportional_to_timing_flag);
  EXPECT_EQ(0, vui_parameters->vui_hrd_parameters_present_flag);
  EXPECT_EQ(0, vui_parameters->bitstream_restriction_flag);
}

}  // namespace h265nal
