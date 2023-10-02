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
  auto user_data_sei = dynamic_cast<H265SeiUserDataRegisteredItuTT35Parser::
                                        H265SeiUserDataRegisteredItuTT35State*>(
      sei_message->payload_state.get());
  EXPECT_TRUE(user_data_sei != nullptr);
  EXPECT_EQ(user_data_sei->itu_t_t35_country_code, 181);
  EXPECT_EQ(user_data_sei->itu_t_t35_country_code_extension_byte, 0);
  std::vector<uint8_t> serialized_bytes;
  sei_message->serialize(serialized_bytes);
  EXPECT_EQ(memcmp(buffer, serialized_bytes.data(), serialized_bytes.size()), 0);
}

// The data in this test is no kind of valid sei, it is just used to test that we
// create the "Unimplemented" sei state with the raw payload bytes
TEST_F(H265SeiParserTest, TestSeiNotImplementedSei) {

  std::vector<uint8_t> buffer;
  buffer.push_back(0x03); // a currently unimplemented sei type
  buffer.push_back(0xff); // add an ff_byte so we test the sei message handling of ff_bytes in size
  buffer.push_back(0x01);
  for (size_t i=0; i<256; i++) {
    buffer.push_back(static_cast<u_int8_t>(i%255));
  } 

  auto sei_message = H265SeiMessageParser::ParseSei(buffer.data(), buffer.size());

  EXPECT_TRUE(sei_message != nullptr);
  EXPECT_EQ(sei_message->payload_type, h265nal::SeiType::filler_payload);
  EXPECT_EQ(sei_message->payload_size, 256);
  auto unimplemented_state = dynamic_cast<H265SeiNotImplementedParser::H265SeiNotImplementedState*>(sei_message->payload_state.get());
  EXPECT_TRUE(unimplemented_state != nullptr);
  EXPECT_EQ(unimplemented_state->payload.size(), 256);
  std::vector<uint8_t> serialized_bytes;
  sei_message->serialize(serialized_bytes);
  EXPECT_EQ(memcmp(buffer.data(), serialized_bytes.data(), serialized_bytes.size()), 0);
}

}  // namespace h265nal
