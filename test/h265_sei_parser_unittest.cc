/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_sei_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h265nal {

class H265SeiParserTest : public ::testing::Test {
 public:
  H265SeiParserTest() {}
  ~H265SeiParserTest() override {}
};

TEST_F(H265SeiParserTest, TestUserDataRegisteredItuTT35Sei) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {
    0x04, 0x47, 0xB5, 0x00, 0x31, 0x47, 0x41, 0x39, 0x34, 0x03, 0x54, 0x00,
    0xFC, 0x80, 0x80, 0xFD, 0x80, 0x80, 0xFA, 0x00, 0x00, 0xFA, 0x00,
    0x00, 0xFA, 0x00, 0x00, 0xFA, 0x00, 0x00, 0xFA, 0x00, 0x00, 0xFA,
    0x00, 0x00, 0xFA, 0x00, 0x00, 0xFA, 0x00, 0x00, 0xFA, 0x00, 0x00,
    0xFA, 0x00, 0x00, 0xFA, 0x00, 0x00, 0xFA, 0x00, 0x00, 0xFA, 0x00,
    0x00, 0xFA, 0x00, 0x00, 0xFA, 0x00, 0x00, 0xFA, 0x00, 0x00, 0xFA,
    0x00, 0x00, 0xFA, 0x00, 0x00, 0xFF };
  // fuzzer::conv: begin
  auto sei = H265SeiParser::ParseSei(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(sei != nullptr);
  EXPECT_EQ(sei->payload_type, h265nal::SeiType::user_data_registered_itu_t_t35);
  EXPECT_EQ(sei->payload_size, 71);
  auto user_data_sei = dynamic_cast<H265SeiParser::SeiStateUserData*>(sei.get());
  EXPECT_TRUE(user_data_sei != nullptr);
  EXPECT_EQ(user_data_sei->itu_t_t35_country_code, 181);
  EXPECT_EQ(user_data_sei->itu_t_t35_country_code_extension_byte, 0);
}

}  // namespace h265nal
