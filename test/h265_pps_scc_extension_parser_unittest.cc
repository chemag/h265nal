/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_pps_scc_extension_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "rtc_common.h"

namespace h265nal {

class H265PpsSccExtensionParserTest : public ::testing::Test {
 public:
  H265PpsSccExtensionParserTest() {}
  ~H265PpsSccExtensionParserTest() override {}
};

TEST_F(H265PpsSccExtensionParserTest, TestSamplePpsSccExtension) {
  // pps_scc_extension
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x00};
  // fuzzer::conv: begin
  auto pps_scc_extension = H265PpsSccExtensionParser::ParsePpsSccExtension(
      buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(pps_scc_extension != nullptr);

  EXPECT_EQ(0, pps_scc_extension->pps_curr_pic_ref_enabled_flag);
  EXPECT_EQ(0,
            pps_scc_extension->residual_adaptive_colour_transform_enabled_flag);
  EXPECT_EQ(0,
            pps_scc_extension->pps_palette_predictor_initializer_present_flag);
  EXPECT_EQ(0, pps_scc_extension->pps_palette_predictor_initializers.size());
}

TEST_F(H265PpsSccExtensionParserTest, TestEnormousLumaBitDepth) {
  // pps_scc_extension
  // fuzzer::conv: data
  const uint8_t buffer[] = {0xa8, 0x00, 0x00, 0x00, 0x03, 0x03, 0xff, 0xff,
                            0xff, 0xe4, 0x00, 0x00};
  // fuzzer::conv: begin
  auto pps_scc_extension = H265PpsSccExtensionParser::ParsePpsSccExtension(
      buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(pps_scc_extension == nullptr);
}

TEST_F(H265PpsSccExtensionParserTest, TestPaletteInitializersNonMonochrome) {
  // A synthetic pps_scc_extension() carrying palette predictor
  // initializers, which no other test in this file reaches: the sample
  // leaves pps_palette_predictor_initializer_present_flag unset, and the
  // enormous-bit-depth one only checks that the parse is rejected.
  //
  // monochrome_palette_flag is 0, so Section 7.3.2.3.3 has
  // chroma_bit_depth_entry_minus8 present, and all three components carry
  // initializers: luma at 8 bits, chroma at 10. Reading that condition the
  // wrong way round consumes the wrong field and misreads every value
  // after it, which is a wrong answer rather than a crash, so only
  // checking the values catches it.
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x2d, 0x62, 0x46, 0x88, 0x0c, 0x05, 0x81, 0x80, 0x80
  };
  // fuzzer::conv: begin
  auto pps_scc_extension = H265PpsSccExtensionParser::ParsePpsSccExtension(
      buffer, arraysize(buffer));
  // fuzzer::conv: end

  ASSERT_TRUE(pps_scc_extension != nullptr);

  EXPECT_EQ(1,
            pps_scc_extension->pps_palette_predictor_initializer_present_flag);
  EXPECT_EQ(2, pps_scc_extension->pps_num_palette_predictor_initializer);
  EXPECT_EQ(0, pps_scc_extension->monochrome_palette_flag);
  EXPECT_EQ(0, pps_scc_extension->luma_bit_depth_entry_minus8);
  EXPECT_EQ(2, pps_scc_extension->chroma_bit_depth_entry_minus8);

  ASSERT_EQ(3, pps_scc_extension->pps_palette_predictor_initializers.size());
  EXPECT_THAT(pps_scc_extension->pps_palette_predictor_initializers[0],
              ::testing::ElementsAreArray({0x12, 0x34}));
  EXPECT_THAT(pps_scc_extension->pps_palette_predictor_initializers[1],
              ::testing::ElementsAreArray({0x101, 0x202}));
  EXPECT_THAT(pps_scc_extension->pps_palette_predictor_initializers[2],
              ::testing::ElementsAreArray({0x303, 0x004}));
}

}  // namespace h265nal
