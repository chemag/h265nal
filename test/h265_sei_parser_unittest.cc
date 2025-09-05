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

TEST_F(H265SeiParserTest, TestMasteringDisplayColourVolumeSei) {
  // Test data for mastering display colour volume SEI
  // This represents typical HDR10 metadata values
  const uint8_t buffer[] = {
      0x89, 0x18,  // payload_type = 137 (mastering_display_colour_volume)
      0x84, 0x49,  // display_primaries[0]_x = 0x8449 (33865 -> 0.6773 in CIE xy)
      0x7d, 0x00,  // display_primaries[0]_y = 0x7d00 (32000 -> 0.64 in CIE xy)
      0x33, 0x5c,  // display_primaries[1]_x = 0x335c (13148 -> 0.26296 in CIE xy)
      0xa9, 0xc2,  // display_primaries[1]_y = 0xa9c2 (43458 -> 0.86916 in CIE xy)
      0x1d, 0x4c,  // display_primaries[2]_x = 0x1d4c (7500 -> 0.15 in CIE xy)
      0x0b, 0xb8,  // display_primaries[2]_y = 0x0bb8 (3000 -> 0.06 in CIE xy)
      0x3e, 0x80,  // white_point_x = 0x3e80 (16000 -> 0.32 in CIE xy)
      0x46, 0x50,  // white_point_y = 0x4650 (18000 -> 0.36 in CIE xy)
      0x00, 0x98, 0x96, 0x80,  // max_display_mastering_luminance = 10000000 (1000 cd/m^2)
      0x00, 0x00, 0x00, 0x32   // min_display_mastering_luminance = 50 (0.005 cd/m^2)
  };
  
  auto sei_message = H265SeiMessageParser::ParseSei(buffer, arraysize(buffer));
  
  EXPECT_TRUE(sei_message != nullptr);
  EXPECT_EQ(sei_message->payload_type,
            h265nal::SeiType::mastering_display_colour_volume);
  EXPECT_EQ(sei_message->payload_size, 24);
  
  auto mastering_display_sei = dynamic_cast<
      H265SeiMasteringDisplayColourVolumeParser::
          H265SeiMasteringDisplayColourVolumeState*>(
      sei_message->payload_state.get());
  EXPECT_TRUE(mastering_display_sei != nullptr);
  
  // Check RGB primaries
  EXPECT_EQ(mastering_display_sei->display_primaries_x[0], 0x8449);
  EXPECT_EQ(mastering_display_sei->display_primaries_y[0], 0x7d00);
  EXPECT_EQ(mastering_display_sei->display_primaries_x[1], 0x335c);
  EXPECT_EQ(mastering_display_sei->display_primaries_y[1], 0xa9c2);
  EXPECT_EQ(mastering_display_sei->display_primaries_x[2], 0x1d4c);
  EXPECT_EQ(mastering_display_sei->display_primaries_y[2], 0x0bb8);
  
  // Check white point
  EXPECT_EQ(mastering_display_sei->white_point_x, 0x3e80);
  EXPECT_EQ(mastering_display_sei->white_point_y, 0x4650);
  
  // Check luminance values
  EXPECT_EQ(mastering_display_sei->max_display_mastering_luminance, 10000000);
  EXPECT_EQ(mastering_display_sei->min_display_mastering_luminance, 50);
}

TEST_F(H265SeiParserTest, TestContentLightLevelInfoSei) {
  // Test data for content light level info SEI
  const uint8_t buffer[] = {
      0x90, 0x01, 0x04,  // payload_type = 144 (content_light_level_info)
      0x03, 0xe8,  // max_content_light_level = 1000 cd/m^2
      0x01, 0x90   // max_pic_average_light_level = 400 cd/m^2
  };
  
  auto sei_message = H265SeiMessageParser::ParseSei(buffer, arraysize(buffer));
  
  EXPECT_TRUE(sei_message != nullptr);
  EXPECT_EQ(sei_message->payload_type,
            h265nal::SeiType::content_light_level_info);
  EXPECT_EQ(sei_message->payload_size, 4);
  
  auto content_light_sei = dynamic_cast<
      H265SeiContentLightLevelInfoParser::H265SeiContentLightLevelInfoState*>(
      sei_message->payload_state.get());
  EXPECT_TRUE(content_light_sei != nullptr);
  
  EXPECT_EQ(content_light_sei->max_content_light_level, 1000);
  EXPECT_EQ(content_light_sei->max_pic_average_light_level, 400);
}

}  // namespace h265nal
