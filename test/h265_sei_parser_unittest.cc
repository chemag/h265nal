/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_sei_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "rtc_common.h"

namespace h265nal {

class H265SeiParserTest : public ::testing::Test {
 public:
  H265SeiParserTest() {}
  ~H265SeiParserTest() override {}
};

TEST_F(H265SeiParserTest, TestUserDataRegisteredItuTT35Sei) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x04, 0x47, 0xb5, 0x00, 0x31, 0x47, 0x41, 0x39, 0x34, 0x03, 0x54,
      0x00, 0xfc, 0x80, 0x80, 0xfd, 0x80, 0x80, 0xfa, 0x00, 0x00, 0xfa,
      0x00, 0x00, 0xfa, 0x00, 0x00, 0xfa, 0x00, 0x00, 0xfa, 0x00, 0x00,
      0xfa, 0x00, 0x00, 0xfa, 0x00, 0x00, 0xfa, 0x00, 0x00, 0xfa, 0x00,
      0x00, 0xfa, 0x00, 0x00, 0xfa, 0x00, 0x00, 0xfa, 0x00, 0x00, 0xfa,
      0x00, 0x00, 0xfa, 0x00, 0x00, 0xfa, 0x00, 0x00, 0xfa, 0x00, 0x00,
      0xfa, 0x00, 0x00, 0xfa, 0x00, 0x00, 0xff};
  // fuzzer::conv: begin
  auto sei_message = H265SeiMessageParser::ParseSei(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(sei_message != nullptr);
  EXPECT_EQ(sei_message->payload_type,
            h265nal::SeiType::user_data_registered_itu_t_t35);
  EXPECT_EQ(sei_message->payload_size, 71);
  auto user_data_registered_itu_t_t35_sei =
      dynamic_cast<H265SeiUserDataRegisteredItuTT35Parser::
                       H265SeiUserDataRegisteredItuTT35State*>(
          sei_message->payload_state.get());
  EXPECT_TRUE(user_data_registered_itu_t_t35_sei != nullptr);
  EXPECT_EQ(user_data_registered_itu_t_t35_sei->itu_t_t35_country_code, 181);
  EXPECT_EQ(
      user_data_registered_itu_t_t35_sei->itu_t_t35_country_code_extension_byte,
      0);
}

TEST_F(H265SeiParserTest, TestUserDataUnregisteredSei) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x05, 0x38, 0x2c, 0xa2, 0xde, 0x09, 0xb5, 0x17, 0x47, 0xdb, 0xbb,
      0x55, 0xa4, 0xfe, 0x7f, 0xc2, 0xfc, 0x4e, 0x78, 0x32, 0x36, 0x35,
      0x20, 0x28, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x20, 0x33, 0x31, 0x29,
      0x20, 0x2d, 0x20, 0x31, 0x2e, 0x33, 0x2b, 0x32, 0x30, 0x2d, 0x36,
      0x65, 0x36, 0x37, 0x35, 0x36, 0x66, 0x39, 0x34, 0x62, 0x32, 0x37,
      0x3a, 0x5b, 0x57, 0x69};
  auto sei_message = H265SeiMessageParser::ParseSei(buffer, arraysize(buffer));

  EXPECT_TRUE(sei_message != nullptr);
  EXPECT_EQ(sei_message->payload_type,
            h265nal::SeiType::user_data_unregistered);
  EXPECT_EQ(sei_message->payload_size, 56);
  auto user_data_unregistered_sei = dynamic_cast<
      H265SeiUserDataUnregisteredParser::H265SeiUserDataUnregisteredState*>(
      sei_message->payload_state.get());
  EXPECT_TRUE(user_data_unregistered_sei != nullptr);
  // 2ca2de09-b517-47db-bb55-a4fe7fc2fc4e
  EXPECT_EQ(user_data_unregistered_sei->uuid_iso_iec_11578_1,
            0x2ca2de09b51747db);
  EXPECT_EQ(user_data_unregistered_sei->uuid_iso_iec_11578_2,
            0xbb55a4fe7fc2fc4e);
}

TEST_F(H265SeiParserTest, TestAlphaChannelInfoSei) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {0xa5, 0x04, 0x00, 0x00, 0x7f, 0x90, 0x80};

  auto sei_message = H265SeiMessageParser::ParseSei(buffer, arraysize(buffer));

  EXPECT_TRUE(sei_message != nullptr);
  auto alpha_channel_info_sei = dynamic_cast<
      H265SeiAlphaChannelInfoParser::H265SeiAlphaChannelInfoState*>(
      sei_message->payload_state.get());
  EXPECT_TRUE(alpha_channel_info_sei != nullptr);
  EXPECT_EQ(alpha_channel_info_sei->alpha_channel_cancel_flag, 0);
  EXPECT_EQ(alpha_channel_info_sei->alpha_channel_use_idc, 0);
  EXPECT_EQ(alpha_channel_info_sei->alpha_channel_bit_depth_minus8, 0);
  EXPECT_EQ(alpha_channel_info_sei->alpha_transparent_value, 0);
  EXPECT_EQ(alpha_channel_info_sei->alpha_opaque_value, 255);
  EXPECT_EQ(alpha_channel_info_sei->alpha_channel_incr_flag, 0);
  EXPECT_EQ(alpha_channel_info_sei->alpha_channel_clip_flag, 0);
}

}  // namespace h265nal
