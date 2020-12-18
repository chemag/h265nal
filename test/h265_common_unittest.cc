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

}  // namespace h265nal
