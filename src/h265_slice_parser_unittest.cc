/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */


#include "h265_slice_parser.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "h265_common.h"
#include "h265_bitstream_parser_state.h"
#include "h265_vps_parser.h"
#include "h265_sps_parser.h"
#include "h265_pps_parser.h"
#include "absl/types/optional.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h265nal {

class H265SliceSegmentLayerParserTest : public ::testing::Test {
 public:
  H265SliceSegmentLayerParserTest() {}
  ~H265SliceSegmentLayerParserTest() override {}

  absl::optional<H265SliceSegmentLayerParser::SliceSegmentLayerState>
      slice_segment_layer_;
};

TEST_F(H265SliceSegmentLayerParserTest, TestSampleSlice) {
  const uint8_t buffer[] = {
      0xaf, 0x09, 0x40, 0xf3, 0xb8, 0xd5, 0x39, 0xba,
      0x1f, 0xe4, 0xa6, 0x08, 0x5c, 0x6e, 0xb1, 0x8f,
      0x00, 0x38, 0xf1, 0xa6, 0xfc, 0xf1, 0x40, 0x04,
      0x3a, 0x86, 0xcb, 0x90, 0x74, 0xce, 0xf0, 0x46,
      0x61, 0x93, 0x72, 0xd6, 0xfc, 0x35, 0xe3, 0xc5
  };

  // get some mock state
  H265BitstreamParserState bitstream_parser_state;
  H265VpsParser::VpsState vps;
  bitstream_parser_state.vps[0] = vps;
  H265SpsParser::SpsState sps;
  sps.sample_adaptive_offset_enabled_flag = 1;
  sps.chroma_format_idc = 1;
  bitstream_parser_state.sps[0] = sps;
  H265PpsParser::PpsState pps;
  bitstream_parser_state.pps[0] = pps;

  slice_segment_layer_ = H265SliceSegmentLayerParser::ParseSliceSegmentLayer(
      buffer, arraysize(buffer), NalUnitType::IDR_W_RADL,
      &bitstream_parser_state);
  EXPECT_TRUE(slice_segment_layer_ != absl::nullopt);

  auto& slice_segment_header = slice_segment_layer_->slice_segment_header;

  EXPECT_EQ(1, slice_segment_header.first_slice_segment_in_pic_flag);
  EXPECT_EQ(0, slice_segment_header.no_output_of_prior_pics_flag);
  EXPECT_EQ(0, slice_segment_header.slice_pic_parameter_set_id);
  EXPECT_EQ(2, slice_segment_header.slice_type);
  EXPECT_EQ(1, slice_segment_header.slice_sao_luma_flag);
  EXPECT_EQ(1, slice_segment_header.slice_sao_chroma_flag);
  EXPECT_EQ(9, slice_segment_header.slice_qp_delta);

#if 0
  EXPECT_EQ(1, slice_segment_header.alignment_bit_equal_to_one);
  EXPECT_EQ(0, slice_segment_header.alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_segment_header.alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_segment_header.alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_segment_header.alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_segment_header.alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_segment_header.alignment_bit_equal_to_zero);
#endif
}

}  // namespace h265nal
