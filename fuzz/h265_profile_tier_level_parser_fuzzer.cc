/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h265_profile_tier_level_parser_unittest.cc.
// Do not edit directly.

#include "h265_profile_tier_level_parser.h"
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
  auto ptls = H265ProfileTierLevelParser::ParseProfileTierLevel(
      data, size, true, 0);
  }
  return 0;
}
