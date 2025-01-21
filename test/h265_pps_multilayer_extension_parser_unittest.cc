/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_pps_multilayer_extension_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "rtc_common.h"

namespace h265nal {

class H265PpsMultilayerExtensionParserTest : public ::testing::Test {
 public:
  H265PpsMultilayerExtensionParserTest() {}
  ~H265PpsMultilayerExtensionParserTest() override {}
};

TEST_F(H265PpsMultilayerExtensionParserTest, TestSamplePpsMultilayerExtension) {
  // pps_multilayer_extension
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x20};
  // fuzzer::conv: begin
  auto pps_multilayer_extension =
      H265PpsMultilayerExtensionParser::ParsePpsMultilayerExtension(
          buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(pps_multilayer_extension != nullptr);

  EXPECT_EQ(0, pps_multilayer_extension->poc_reset_info_present_flag);
  EXPECT_EQ(0, pps_multilayer_extension->pps_infer_scaling_list_flag);
  EXPECT_EQ(0, pps_multilayer_extension->num_ref_loc_offsets);
  EXPECT_EQ(0, pps_multilayer_extension->colour_mapping_enabled_flag);
}

}  // namespace h265nal
