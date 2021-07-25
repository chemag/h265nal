/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_common.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

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


struct H265CommonMoreRbspDataParameterTestData {
  int line;
  std::vector<uint8_t> buffer;
  size_t cur_byte_offset;
  size_t cur_bit_offset;
  bool expected_result;
  bool expected_get_last_bit_offset_return;
  size_t expected_last_one_byte_offset;
  size_t expected_last_one_bit_offset;
};

class H265CommonMoreRbspDataTest
    : public ::testing::TestWithParam<H265CommonMoreRbspDataParameterTestData> {
};

const auto& kH265CommonMoreRbspDataParameterTestcases = *new std::vector<
    H265CommonMoreRbspDataParameterTestData>{
    {__LINE__, {0xe8, 0x43, 0x82, 0x92, 0xc8, 0xb0}, 4, 4, true, true, 5, 3},
    {__LINE__, {0xe8, 0x43, 0x82, 0x92, 0xc8, 0xb1}, 4, 4, true, true, 5, 7},
    {__LINE__, {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x00}, 4, 4, false, true, 4, 4},
    {__LINE__, {0x00, 0x00}, 0, 0, false, false, 0, 0},
};

TEST_P(H265CommonMoreRbspDataTest, Run) {
  const auto& testcase = GetParam();
  rtc::BitBuffer bit_buffer(testcase.buffer.data(), testcase.buffer.size());
  bit_buffer.Seek(testcase.cur_byte_offset, testcase.cur_bit_offset);
  // first check more_rbsp_data
  EXPECT_EQ(testcase.expected_result, more_rbsp_data(&bit_buffer))
      << "line: " << testcase.line;

  // then check BitBuffer::GetCurrentOffset()
  size_t expected_last_one_byte_offset = 0;
  size_t expected_last_one_bit_offset = 0;
  EXPECT_EQ(testcase.expected_get_last_bit_offset_return,
            bit_buffer.GetLastBitOffset(1, &expected_last_one_byte_offset,
                                        &expected_last_one_bit_offset))
      << "line: " << testcase.line;
  if (testcase.expected_get_last_bit_offset_return) {
    EXPECT_EQ(testcase.expected_last_one_byte_offset,
              expected_last_one_byte_offset)
        << "line: " << testcase.line;
    EXPECT_EQ(testcase.expected_last_one_bit_offset,
              expected_last_one_bit_offset)
        << "line: " << testcase.line;
  }
}

INSTANTIATE_TEST_SUITE_P(
    Parameter, H265CommonMoreRbspDataTest,
    ::testing::ValuesIn(kH265CommonMoreRbspDataParameterTestcases));

}  // namespace h265nal
