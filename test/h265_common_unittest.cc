/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_common.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h265nal {

class H265CommonTest : public ::testing::Test {
 public:
  H265CommonTest() {}
  ~H265CommonTest() override {}
};

TEST_F(H265CommonTest, TestIsSliceSegment) {
  EXPECT_TRUE(IsSliceSegment(TRAIL_N));
  EXPECT_FALSE(IsSliceSegment(VPS_NUT));
}

TEST_F(H265CommonTest, TestIsNalUnitTypeVcl) {
  EXPECT_TRUE(IsNalUnitTypeVcl(BLA_W_LP));
  EXPECT_TRUE(IsNalUnitTypeVcl(BLA_W_LP));
  EXPECT_FALSE(IsNalUnitTypeVcl(VPS_NUT));
  EXPECT_FALSE(IsNalUnitTypeVcl(RSV_NVCL43));
}

TEST_F(H265CommonTest, TestIsNalUnitTypeNonVcl) {
  EXPECT_TRUE(IsNalUnitTypeNonVcl(VPS_NUT));
  EXPECT_TRUE(IsNalUnitTypeNonVcl(RSV_NVCL43));
  EXPECT_FALSE(IsNalUnitTypeNonVcl(BLA_W_LP));
}

TEST_F(H265CommonTest, TestIsNalUnitTypeUnspecified) {
  EXPECT_TRUE(IsNalUnitTypeUnspecified(UNSPEC50));
  EXPECT_TRUE(IsNalUnitTypeUnspecified(UNSPEC63));
  EXPECT_FALSE(IsNalUnitTypeUnspecified(BLA_W_LP));
  EXPECT_FALSE(IsNalUnitTypeUnspecified(RSV_NVCL47));

}


}  // namespace h265nal
