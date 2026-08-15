/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h265_st_ref_pic_set_parser_unittest.cc.
// Do not edit directly.

#include "h265_st_ref_pic_set_parser.h"
#include "h265_common.h"
#include "h265_sps_parser.h"
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
  auto sps = std::make_shared<H265SpsParser::SpsState>();
  sps->num_short_term_ref_pic_sets = 0;
  uint32_t max_num_pics = 1;
  auto st_ref_pic_set = H265StRefPicSetParser::ParseStRefPicSet(
      data, size, 0, 1, &(sps->st_ref_pic_set), max_num_pics);
  }
  {
  std::vector<std::unique_ptr<struct H265StRefPicSetParser::StRefPicSetState>>
      st_ref_pic_set_state_vector;
  auto st_ref_pic_set = H265StRefPicSetParser::ParseStRefPicSet(
      data, size, 1, 1, &st_ref_pic_set_state_vector, 16);
  }
  {
  std::vector<std::unique_ptr<struct H265StRefPicSetParser::StRefPicSetState>>
      st_ref_pic_set_state_vector;
  auto st_ref_pic_set = H265StRefPicSetParser::ParseStRefPicSet(
      data, size, 0, 1, &st_ref_pic_set_state_vector, 16);
  }
  return 0;
}
