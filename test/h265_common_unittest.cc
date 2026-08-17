/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_common.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "rtc_common.h"

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
  std::string description;
  std::vector<uint8_t> buffer;
  size_t cur_byte_offset;
  size_t cur_bit_offset;
  bool expected_result;
};

class H265CommonMoreRbspDataTest
    : public ::testing::TestWithParam<H265CommonMoreRbspDataParameterTestData> {
};

const auto& kH265CommonMoreRbspDataParameterTestcases = *new std::vector<
    H265CommonMoreRbspDataParameterTestData>{
    {"case 1: no more data in the bit buffer", {0x00, 0x00}, 2, 0, false},
    {"case 2: more than 1 byte left",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0xb0},
     4,
     4,
     true},
    {"case 3: at last byte (begin), with 1000,0000",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x80},
     5,
     0,
     false},
    {"case 4: at last byte (begin), with 1000,1000",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x88},
     5,
     0,
     true},
    {"case 5: at last byte (bit 1), with 1-100,0000",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0xc0},
     5,
     1,
     false},
    {"case 6: at last byte (bit 1), with 0-100,0000",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x40},
     5,
     1,
     false},
    {"case 7: at last byte (bit 1), with 0-000,0000 (no rbsp_trailing_bits())",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x00},
     5,
     1,
     true},
    {"case 8: at last byte (bit 1), with 0-100,1000",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x48},
     5,
     1,
     true},
    {"case 9: at last byte (bit 4), with 1-1000",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x18},
     5,
     4,
     false},
    {"case 10: at last byte (bit 4), with 0-1000",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x08},
     5,
     4,
     false},
    {"case 11: at last byte (bit 4), with 0000 (no rbsp_trailing_bits())",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x00},
     5,
     4,
     true},
    {"case 12: at last byte (bit 4), with 0010",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x02},
     5,
     4,
     true},
    {"case 13: at last byte (bit 7), with 1",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x01},
     5,
     7,
     false},
    {"case 14: at last byte (bit 7), with 0 (no rbsp_trailing_bits())",
     {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x00},
     5,
     7,
     true},
};

TEST_P(H265CommonMoreRbspDataTest, Run) {
  const auto& testcase = GetParam();
  BitBuffer bit_buffer(testcase.buffer.data(), testcase.buffer.size());
  bit_buffer.Seek(testcase.cur_byte_offset, testcase.cur_bit_offset);
  // 1: store the bit buffer status before the more_rbsp_data call
  size_t pre_out_byte_offset;
  size_t pre_out_bit_offset;
  bit_buffer.GetCurrentOffset(&pre_out_byte_offset, &pre_out_bit_offset);
  // 2: check more_rbsp_data
  EXPECT_EQ(testcase.expected_result, more_rbsp_data(&bit_buffer))
      << "description: " << testcase.description;
  // 3: store the bit buffer status after the more_rbsp_data call
  size_t post_out_byte_offset;
  size_t post_out_bit_offset;
  bit_buffer.GetCurrentOffset(&post_out_byte_offset, &post_out_bit_offset);
  // then make sure the function is a const
  EXPECT_EQ(pre_out_byte_offset, post_out_byte_offset);
  EXPECT_EQ(pre_out_bit_offset, post_out_bit_offset);
}

INSTANTIATE_TEST_SUITE_P(
    Parameter, H265CommonMoreRbspDataTest,
    ::testing::ValuesIn(kH265CommonMoreRbspDataParameterTestcases));

class H265CommonUnimplementedTest : public ::testing::Test {
 public:
  H265CommonUnimplementedTest() {}
  ~H265CommonUnimplementedTest() override {}
};

TEST_F(H265CommonUnimplementedTest, TestUnimplementedCount) {
  // the count is what lets the h265nal tool tell a bitstream we do not
  // support yet from a bitstream that is broken
  reset_unimplemented_count();
  EXPECT_EQ(0, get_unimplemented_count());

  report_unimplemented("pps_range_extension()", "pps()");
  EXPECT_EQ(1, get_unimplemented_count());

  report_unimplemented("colour_mapping_table()", "pps_multilayer_extension()");
  EXPECT_EQ(2, get_unimplemented_count());

  reset_unimplemented_count();
  EXPECT_EQ(0, get_unimplemented_count());
}

class H265CommonSignedGolombTest : public ::testing::Test {
 public:
  H265CommonSignedGolombTest() {}
  ~H265CommonSignedGolombTest() override {}
};

TEST_F(H265CommonSignedGolombTest, TestReadSignedExponentialGolomb) {
  // Section 9.1.1 maps codeNum k to (-1)^(k+1) * Ceil(k / 2).
  struct {
    std::vector<uint8_t> buffer;
    int32_t expected;
    const char* what;
  } kTestcases[] = {
      // small values, to pin the mapping itself
      {{0x80}, 0, "k=0"},
      {{0x40}, 1, "k=1"},
      {{0x60}, -1, "k=2"},
      {{0x20}, 2, "k=3"},
      // k == INT32_MAX. Reachable: a 31 zero prefix reads a 32 bit value,
      // so k covers [INT32_MAX, UINT32_MAX - 1]. Computing this branch as
      // static_cast<int32_t>(k) + 1 overflows a signed int, which is
      // undefined and in practice wrapped to INT32_MIN, so this returned
      // -1073741824: the right magnitude with the wrong sign.
      {{0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}, 1073741824,
       "k=INT32_MAX"},
      // the extremes of the mapping, which do fit in an int32_t
      {{0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xfc}, 2147483647,
       "k=UINT32_MAX-2, the largest odd codeNum"},
      {{0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xfe}, -2147483647,
       "k=UINT32_MAX-1, the largest even codeNum"},
  };

  for (const auto& testcase : kTestcases) {
    BitBuffer bit_buffer(testcase.buffer.data(), testcase.buffer.size());
    int32_t value = 0;
    EXPECT_TRUE(bit_buffer.ReadSignedExponentialGolomb(value)) << testcase.what;
    EXPECT_EQ(testcase.expected, value) << testcase.what;
  }
}

class H265CommonZeroWidthReadTest : public ::testing::Test {
 public:
  H265CommonZeroWidthReadTest() {}
  ~H265CommonZeroWidthReadTest() override {}
};

TEST_F(H265CommonZeroWidthReadTest, TestPeekAndReadZeroBits) {
  // A zero bit read used to be an RTC_DCHECK, that is an assert(): it
  // aborted a debug build rather than failing the parse, and vanished
  // under NDEBUG, where PeekBits() went on to dereference the current
  // byte before testing anything. With no bits left that reads one byte
  // past the buffer, which ASan reports as a buffer overflow.
  const uint8_t buffer[] = {0xab};
  BitBuffer bit_buffer(buffer, arraysize(buffer));

  uint32_t val32 = 0;
  uint64_t val64 = 0;
  // with bits still available
  EXPECT_FALSE(bit_buffer.PeekBits(0, val32));
  EXPECT_FALSE(bit_buffer.PeekBits(0, val64));
  EXPECT_FALSE(bit_buffer.ReadBits(0, val32));

  // and with the buffer exhausted, which is the case that read out of
  // bounds
  ASSERT_TRUE(bit_buffer.ReadBits(8, val32));
  ASSERT_EQ(0u, bit_buffer.RemainingBitCount());
  EXPECT_FALSE(bit_buffer.PeekBits(0, val32));
  EXPECT_FALSE(bit_buffer.PeekBits(0, val64));
  EXPECT_FALSE(bit_buffer.ReadBits(0, val32));
}

class H265CommonNaluChecksumTest : public ::testing::Test {
 public:
  H265CommonNaluChecksumTest() {}
  ~H265CommonNaluChecksumTest() override {}
};

// Note: a regression here does not fail, it hangs. GetNaluChecksum used to
// loop forever on a bit buffer that was not byte aligned, so the assertion
// below is only reached if the loop terminates at all.
TEST_F(H265CommonNaluChecksumTest, TestUnalignedBitBuffer) {
  // 6 bytes, with 1 bit consumed before the call. That leaves 47 bits: the
  // 32 bit loop takes 32, the byte loop takes 8, and 7 are left over.
  // 7 is "> 0" but too few for ReadUInt8(), and a failed read consumes
  // nothing, so a loop that tests RemainingBitCount() > 0 and ignores the
  // result of the read never advances.
  const uint8_t buffer[] = {0xde, 0xad, 0xbe, 0xef, 0x12, 0x34};
  BitBuffer bit_buffer(buffer, arraysize(buffer));
  uint32_t bit = 0;
  ASSERT_TRUE(bit_buffer.ReadBits(1, bit));

  auto checksum = NaluChecksum::GetNaluChecksum(&bit_buffer);
  ASSERT_TRUE(checksum != nullptr);
  EXPECT_EQ(4, checksum->GetLength());
}

TEST_F(H265CommonNaluChecksumTest, TestAlignedBitBuffer) {
  // the aligned path, to show the fix did not change the ordinary result.
  // 6 bytes: the 32 bit loop takes 32 and the byte loop takes the other 2
  // bytes, so i ends at 2 and every shift is in range.
  const uint8_t buffer[] = {0xde, 0xad, 0xbe, 0xef, 0x12, 0x34};
  BitBuffer bit_buffer(buffer, arraysize(buffer));

  auto checksum = NaluChecksum::GetNaluChecksum(&bit_buffer);
  ASSERT_TRUE(checksum != nullptr);
  EXPECT_EQ(4, checksum->GetLength());
  // GetNaluChecksum() restores the bit buffer, so nothing is consumed
  EXPECT_EQ(arraysize(buffer) * 8, bit_buffer.RemainingBitCount());
}

class H265CommonBitBufferTest : public ::testing::Test {
 public:
  H265CommonBitBufferTest() {}
  ~H265CommonBitBufferTest() override {}
};

TEST_F(H265CommonBitBufferTest, TestReadExponentialGolombRewindsOnFailure) {
  // 40 leading zero bits, so zero_bit_count reaches 40 and the value would
  // be 41 bits wide, past the 32 bit cap, and the read fails. Counting
  // those zeros consumed them, so the failure path has to rewind: a caller
  // that tries a different syntax element after a failed ue(v) must see
  // the bit buffer where it left it.
  //
  // Note this only discriminates in a build with NDEBUG defined. The
  // rewind used to sit inside RTC_CHECK(), which is assert(), so it did
  // run in a debug build and did not in a release one.
  const uint8_t buffer[] = {0x00, 0x00, 0x00, 0x00, 0x00};
  BitBuffer bit_buffer(buffer, arraysize(buffer));

  size_t byte_offset = 0;
  size_t bit_offset = 0;
  bit_buffer.GetCurrentOffset(&byte_offset, &bit_offset);
  EXPECT_EQ(0, byte_offset);
  EXPECT_EQ(0, bit_offset);

  uint32_t val = 0;
  EXPECT_FALSE(bit_buffer.ReadExponentialGolomb(val));

  bit_buffer.GetCurrentOffset(&byte_offset, &bit_offset);
  EXPECT_EQ(0, byte_offset);
  EXPECT_EQ(0, bit_offset);
}

}  // namespace h265nal
