/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */


#include "h265_rtp_parser.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "h265_common.h"
#include "absl/types/optional.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h265nal {

class H265RtpParserTest : public ::testing::Test {
 public:
  H265RtpParserTest() {}
  ~H265RtpParserTest() override {}

  absl::optional<H265RtpParser::RtpState> rtp_;
};

TEST_F(H265RtpParserTest, TestSampleVps) {
  // Single NAL Unit Packet (VPS for a 1280x720 camera capture).
  const uint8_t buffer[] = {0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60,
                            0x00, 0x00, 0x03, 0x00, 0xb0, 0x00, 0x00, 0x03,
                            0x00, 0x00, 0x03, 0x00, 0x5d, 0xac, 0x59, 0x00};
  H265BitstreamParserState bitstream_parser_state;
  rtp_ = H265RtpParser::ParseRtp(
      buffer, arraysize(buffer),
      &bitstream_parser_state);
  EXPECT_TRUE(rtp_ != absl::nullopt);

  // check the header
  auto rtp_single = rtp_->rtp_single;
  EXPECT_EQ(0, rtp_single.nal_unit_header.forbidden_zero_bit);
  EXPECT_EQ(NalUnitType::VPS_NUT, rtp_single.nal_unit_header.nal_unit_type);
  EXPECT_EQ(0, rtp_single.nal_unit_header.nuh_layer_id);
  EXPECT_EQ(1, rtp_single.nal_unit_header.nuh_temporal_id_plus1);
}

}  // namespace h265nal
