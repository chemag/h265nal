/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_nal_unit_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_bitstream_parser_state.h"
#include "h265_common.h"
#include "rtc_common.h"

namespace h265nal {

class H265NalUnitParserTest : public ::testing::Test {
 public:
  H265NalUnitParserTest() {}
  ~H265NalUnitParserTest() override {}
};

TEST_F(H265NalUnitParserTest, TestSampleNalUnit) {
  // VPS for a 1280x720 camera capture.
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60,
                            0x00, 0x00, 0x03, 0x00, 0xb0, 0x00, 0x00, 0x03,
                            0x00, 0x00, 0x03, 0x00, 0x5d, 0xac, 0x59, 0x00};
  // fuzzer::conv: begin
  H265BitstreamParserState bitstream_parser_state;
  ParsingOptions parsing_options;
  parsing_options.add_checksum = true;
  auto nal_unit = H265NalUnitParser::ParseNalUnit(buffer, arraysize(buffer),
                                                  &bitstream_parser_state,
                                                  parsing_options);
  // fuzzer::conv: end

  EXPECT_TRUE(nal_unit != nullptr);

  // check the parsed length
  EXPECT_EQ(20, nal_unit->parsed_length);

  // check the checksum
  EXPECT_THAT(std::vector<char>(nal_unit->checksum->GetChecksum(),
                                nal_unit->checksum->GetChecksum() +
                                    nal_unit->checksum->GetLength()),
              ::testing::ElementsAreArray({0xbf, 0xa2, 0x45, 0x94}));

  // test the checksum's ascii dumper
  char checksum_printable[64] = {};
  nal_unit->checksum->fdump(checksum_printable, 64);
  EXPECT_STREQ(checksum_printable, "bfa24594");

  // check the header
  EXPECT_EQ(0, nal_unit->nal_unit_header->forbidden_zero_bit);
  EXPECT_EQ(NalUnitType::VPS_NUT, nal_unit->nal_unit_header->nal_unit_type);
  EXPECT_EQ(0, nal_unit->nal_unit_header->nuh_layer_id);
  EXPECT_EQ(1, nal_unit->nal_unit_header->nuh_temporal_id_plus1);

  // check the payload
  auto vps = nal_unit->nal_unit_payload->vps;
  EXPECT_EQ(0, vps->vps_video_parameter_set_id);
  EXPECT_EQ(1, vps->vps_base_layer_internal_flag);
  EXPECT_EQ(1, vps->vps_base_layer_available_flag);
  EXPECT_EQ(0, vps->vps_max_layers_minus1);
  EXPECT_EQ(0, vps->vps_max_sub_layers_minus1);
  EXPECT_EQ(1, vps->vps_temporal_id_nesting_flag);
  EXPECT_EQ(0xffff, vps->vps_reserved_0xffff_16bits);
  // profile_tier_level start
  EXPECT_EQ(0, vps->profile_tier_level->general->profile_space);
  EXPECT_EQ(0, vps->profile_tier_level->general->tier_flag);
  EXPECT_EQ(1, vps->profile_tier_level->general->profile_idc);
  EXPECT_THAT(vps->profile_tier_level->general->profile_compatibility_flag,
              ::testing::ElementsAreArray({0, 1, 1, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0}));
  EXPECT_EQ(1, vps->profile_tier_level->general->progressive_source_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->interlaced_source_flag);
  EXPECT_EQ(1, vps->profile_tier_level->general->non_packed_constraint_flag);
  EXPECT_EQ(1, vps->profile_tier_level->general->frame_only_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_12bit_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_10bit_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_8bit_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_422chroma_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_420chroma_constraint_flag);
  EXPECT_EQ(0,
            vps->profile_tier_level->general->max_monochrome_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->intra_constraint_flag);
  EXPECT_EQ(0,
            vps->profile_tier_level->general->one_picture_only_constraint_flag);
  EXPECT_EQ(0,
            vps->profile_tier_level->general->lower_bit_rate_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_14bit_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->reserved_zero_33bits);
  EXPECT_EQ(0, vps->profile_tier_level->general->reserved_zero_34bits);
  EXPECT_EQ(0, vps->profile_tier_level->general->reserved_zero_43bits);
  EXPECT_EQ(0, vps->profile_tier_level->general->inbld_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->reserved_zero_bit);
  EXPECT_EQ(93, vps->profile_tier_level->general_level_idc);
  EXPECT_EQ(0, vps->profile_tier_level->sub_layer_profile_present_flag.size());
  EXPECT_EQ(0, vps->profile_tier_level->sub_layer_level_present_flag.size());
  EXPECT_EQ(0, vps->profile_tier_level->reserved_zero_2bits.size());
  EXPECT_EQ(0, vps->profile_tier_level->sub_layer.size());
  // profile_tier_level end
  EXPECT_EQ(1, vps->vps_sub_layer_ordering_info_present_flag);
  EXPECT_THAT(vps->vps_max_dec_pic_buffering_minus1,
              ::testing::ElementsAreArray({1}));
  EXPECT_THAT(vps->vps_max_num_reorder_pics, ::testing::ElementsAreArray({0}));
  EXPECT_THAT(vps->vps_max_latency_increase_plus1,
              ::testing::ElementsAreArray({0}));
  EXPECT_EQ(5, vps->vps_max_layer_id);
  EXPECT_EQ(0, vps->vps_num_layer_sets_minus1);
  EXPECT_EQ(0, vps->layer_id_included_flag.size());
  EXPECT_EQ(0, vps->vps_timing_info_present_flag);
  EXPECT_EQ(0, vps->vps_num_units_in_tick);
  EXPECT_EQ(0, vps->vps_time_scale);
  EXPECT_EQ(0, vps->vps_poc_proportional_to_timing_flag);
  EXPECT_EQ(0, vps->vps_num_ticks_poc_diff_one_minus1);
  EXPECT_EQ(0, vps->vps_num_hrd_parameters);
  EXPECT_EQ(0, vps->hrd_layer_set_idx.size());
  EXPECT_EQ(0, vps->cprms_present_flag.size());
  EXPECT_EQ(0, vps->vps_extension_flag);
  EXPECT_EQ(0, vps->vps_extension_data_flag);
}

TEST_F(H265NalUnitParserTest, TestEmptyNalUnit) {
  const uint8_t buffer[] = {0};
  H265BitstreamParserState bitstream_parser_state;
  ParsingOptions parsing_options;
  parsing_options.add_checksum = false;
  auto nal_unit =
      H265NalUnitParser::ParseNalUnit(buffer, 0, &bitstream_parser_state,
                                      parsing_options);
  EXPECT_TRUE(nal_unit == nullptr);
}

#ifdef FDUMP_DEFINE
TEST_F(H265NalUnitParserTest, TestUnparsedChecksumFdump) {
  // fdump() takes its own ParsingOptions, so a caller can ask for a
  // checksum that ParseNalUnit() was never told to calculate. Note that
  // neither the h265nal tool nor BitstreamState::fdump() can get here:
  // both hand the same options to the parser and to the dumper.
  const uint8_t buffer[] = {0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60,
                            0x00, 0x00, 0x03, 0x00, 0xb0, 0x00, 0x00, 0x03,
                            0x00, 0x00, 0x03, 0x00, 0x5d, 0xac, 0x59, 0x00};
  H265BitstreamParserState bitstream_parser_state;
  ParsingOptions parsing_options;
  parsing_options.add_checksum = false;
  auto nal_unit = H265NalUnitParser::ParseNalUnit(buffer, arraysize(buffer),
                                                  &bitstream_parser_state,
                                                  parsing_options);

  EXPECT_TRUE(nal_unit != nullptr);
  EXPECT_TRUE(nal_unit->checksum == nullptr);

  // dumping the null checksum used to dereference it
  ParsingOptions dump_options;
  dump_options.add_checksum = true;
  FILE* outfp = tmpfile();
  ASSERT_TRUE(outfp != nullptr);
  // indent_level -1 keeps the dump in a single line
  nal_unit->fdump(outfp, -1, dump_options);

  // check the dump ran, and just skipped the checksum it does not have
  std::vector<char> dump(4096, '\0');
  rewind(outfp);
  size_t dump_length = fread(dump.data(), 1, dump.size() - 1, outfp);
  fclose(outfp);
  EXPECT_GT(dump_length, 0);
  EXPECT_THAT(dump.data(), ::testing::HasSubstr("nal_unit_type: 32"));
  EXPECT_THAT(dump.data(), ::testing::Not(::testing::HasSubstr("checksum:")));
}

TEST_F(H265NalUnitParserTest, TestTruncatedAudNalUnitFdump) {
  // An AUD NAL unit header (nal_unit_type: 35) and nothing else. The
  // access_unit_delimiter_rbsp() needs 3 more bits for pic_type, so the
  // aud parse fails, and the payload keeps a null aud. Note that the NAL
  // unit itself still parses: an unparseable payload is not fatal here.
  const uint8_t buffer[] = {0x46, 0x01};
  H265BitstreamParserState bitstream_parser_state;
  ParsingOptions parsing_options;
  auto nal_unit = H265NalUnitParser::ParseNalUnit(buffer, arraysize(buffer),
                                                  &bitstream_parser_state,
                                                  parsing_options);

  EXPECT_TRUE(nal_unit != nullptr);
  EXPECT_EQ(NalUnitType::AUD_NUT, nal_unit->nal_unit_header->nal_unit_type);
  EXPECT_TRUE(nal_unit->nal_unit_payload->aud == nullptr);
  // which is what IsPayloadParsed() reports to the caller
  EXPECT_FALSE(nal_unit->nal_unit_payload->IsPayloadParsed(
      NalUnitType::AUD_NUT));

  // dumping the null aud used to dereference it
  FILE* outfp = tmpfile();
  ASSERT_TRUE(outfp != nullptr);
  // indent_level -1 keeps the dump in a single line
  nal_unit->fdump(outfp, -1, parsing_options);

  // check the dump ran, and left the payload empty
  std::vector<char> dump(1024, '\0');
  rewind(outfp);
  size_t dump_length = fread(dump.data(), 1, dump.size() - 1, outfp);
  fclose(outfp);
  EXPECT_GT(dump_length, 0);
  EXPECT_THAT(dump.data(), ::testing::HasSubstr("nal_unit_type: 35"));
  EXPECT_THAT(dump.data(), ::testing::HasSubstr("nal_unit_payload {  }"));
}
#endif  // FDUMP_DEFINE

class H265NalUnitHeaderParserTest : public ::testing::Test {
 public:
  H265NalUnitHeaderParserTest() {}
  ~H265NalUnitHeaderParserTest() override {}
};

TEST_F(H265NalUnitHeaderParserTest, VerifParseNaluType) {
  const uint8_t buffer[] = {0x20, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60,
                            0x00, 0x00, 0x03, 0x00, 0xb0, 0x00, 0x00, 0x03,
                            0x00, 0x00, 0x03, 0x00, 0x5d, 0xac, 0x59, 0x00};
  NalUnitType naluType;
  H265NalUnitHeaderParser::GetNalUnitType(buffer, arraysize(buffer), naluType);
  EXPECT_EQ(NalUnitType::BLA_W_LP, naluType);
}

}  // namespace h265nal
