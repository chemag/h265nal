/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */


#include "h265_bitstream_parser.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "h265_common.h"
#include "absl/types/optional.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h265nal {

class H265BitstreamParserTest : public ::testing::Test {
 public:
  H265BitstreamParserTest() {}
  ~H265BitstreamParserTest() override {}

  absl::optional<H265BitstreamParser::BitstreamState> bitstream_;
};

TEST_F(H265BitstreamParserTest, TestSampleBitstream) {
  // VPS, SPS, PPS for a 1280x720 camera capture.
  const uint8_t buffer[] = {
      0x00, 0x00, 0x00, 0x01, 0x40, 0x01, 0x0c, 0x01,
      0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00,
      0xb0, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00,
      0x5d, 0xac, 0x59, 0x00, 0x00, 0x00, 0x01, 0x42,
      0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00,
      0xb0, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00,
      0x5d, 0xa0, 0x02, 0x80, 0x80, 0x2e, 0x1f, 0x13,
      0x96, 0xbb, 0x93, 0x24, 0xbb, 0x95, 0x82, 0x83,
      0x03, 0x01, 0x76, 0x85, 0x09, 0x40, 0x00, 0x00,
      0x00, 0x01, 0x44, 0x01, 0xc0, 0xf3, 0xc0, 0x02,
      0x10, 0x00
  };
  bitstream_ = H265BitstreamParser::ParseBitstream(buffer, arraysize(buffer));
  EXPECT_TRUE(bitstream_ != absl::nullopt);

  // check there are 3 NAL units
  EXPECT_EQ(3, bitstream_->nal_units.size());

  // check the first NAL unit
  EXPECT_EQ(0, bitstream_->nal_units[0].nal_unit_header.forbidden_zero_bit);
  EXPECT_EQ(NalUnitType::VPS_NUT,
      bitstream_->nal_units[0].nal_unit_header.nal_unit_type);
  EXPECT_EQ(0, bitstream_->nal_units[0].nal_unit_header.nuh_layer_id);
  EXPECT_EQ(1, bitstream_->nal_units[0].nal_unit_header.nuh_temporal_id_plus1);

  // check the second NAL unit
  EXPECT_EQ(0, bitstream_->nal_units[1].nal_unit_header.forbidden_zero_bit);
  EXPECT_EQ(NalUnitType::SPS_NUT,
      bitstream_->nal_units[1].nal_unit_header.nal_unit_type);
  EXPECT_EQ(0, bitstream_->nal_units[1].nal_unit_header.nuh_layer_id);
  EXPECT_EQ(1, bitstream_->nal_units[1].nal_unit_header.nuh_temporal_id_plus1);

  // check the third NAL unit
  EXPECT_EQ(0, bitstream_->nal_units[2].nal_unit_header.forbidden_zero_bit);
  EXPECT_EQ(NalUnitType::PPS_NUT,
      bitstream_->nal_units[2].nal_unit_header.nal_unit_type);
  EXPECT_EQ(0, bitstream_->nal_units[2].nal_unit_header.nuh_layer_id);
  EXPECT_EQ(1, bitstream_->nal_units[2].nal_unit_header.nuh_temporal_id_plus1);
}

}  // namespace h265nal
