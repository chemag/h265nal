/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h265_slice_parser_unittest.cc.
// Do not edit directly.

#include "h265_slice_parser.h"
#include "h265_bitstream_parser_state.h"
#include "h265_common.h"
#include "h265_nal_unit_parser.h"
#include "h265_pps_parser.h"
#include "h265_sps_parser.h"
#include "h265_vps_parser.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  // the code below is copied verbatim out of a unit test, where it sits
  // inside "namespace h265nal", and so refers to the project's types
  // unqualified. This entry point cannot: it has to be extern "C" at
  // global scope. Pull the namespace in rather than qualifying each name,
  // which would mean keeping a list of them here.
  using namespace h265nal;  // NOLINT(build/namespaces)
  // a test with no fuzzer::conv markers converts to an empty body
  (void)data;
  (void)size;
  {
  // get some mock state
  H265BitstreamParserState bitstream_parser_state;
  auto vps = std::make_shared<H265VpsParser::VpsState>();
  bitstream_parser_state.vps[0] = vps;
  auto sps = std::make_shared<H265SpsParser::SpsState>();
  sps->sample_adaptive_offset_enabled_flag = 1;
  sps->chroma_format_idc = 1;
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<H265PpsParser::PpsState>();
  bitstream_parser_state.pps[0] = pps;
  auto slice_segment_layer =
      H265SliceSegmentLayerParser::ParseSliceSegmentLayer(
          data, size, NalUnitType::IDR_W_RADL,
          &bitstream_parser_state);
  }
  {
  // get some mock state
  H265BitstreamParserState bitstream_parser_state;
  auto vps = std::make_shared<H265VpsParser::VpsState>();
  bitstream_parser_state.vps[0] = vps;
  auto sps = std::make_shared<H265SpsParser::SpsState>();
  sps->chroma_format_idc = 1;
  sps->sample_adaptive_offset_enabled_flag = 0;
  sps->log2_max_pic_order_cnt_lsb_minus4 = 4;
  sps->num_short_term_ref_pic_sets = 1;
  sps->sps_temporal_mvp_enabled_flag = 1;
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<H265PpsParser::PpsState>();
  pps->num_ref_idx_l0_default_active_minus1 = 1;
  pps->num_ref_idx_l1_default_active_minus1 = 1;
  bitstream_parser_state.pps[0] = pps;
  auto slice_segment_layer =
      H265SliceSegmentLayerParser::ParseSliceSegmentLayer(
          data, size, NalUnitType::TRAIL_R,
          &bitstream_parser_state);
  }
  {
  // get some mock state
  H265BitstreamParserState bitstream_parser_state;
  auto vps = std::make_shared<H265VpsParser::VpsState>();
  bitstream_parser_state.vps[0] = vps;
  auto sps = std::make_shared<H265SpsParser::SpsState>();
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<H265PpsParser::PpsState>();
  bitstream_parser_state.pps[0] = pps;
  auto slice_segment_layer =
      H265SliceSegmentLayerParser::ParseSliceSegmentLayer(
          data, size, NalUnitType::TRAIL_R,
          &bitstream_parser_state);
  }
  return 0;
}
