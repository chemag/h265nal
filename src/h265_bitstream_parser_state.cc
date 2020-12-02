/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_bitstream_parser_state.h"

#include <stdio.h>

#include <cstdint>
#include <vector>

#include "absl/types/optional.h"
#include "h265_common.h"
#include "h265_pps_parser.h"
#include "h265_sps_parser.h"
#include "h265_vps_parser.h"

namespace {
typedef absl::optional<struct h265nal::H265VpsParser::VpsState>
    OptionalVpsState;
typedef absl::optional<struct h265nal::H265SpsParser::SpsState>
    OptionalSpsState;
typedef absl::optional<struct h265nal::H265PpsParser::PpsState>
    OptionalPpsState;
}  // namespace

namespace h265nal {

// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

absl::optional<struct H265VpsParser::VpsState> H265BitstreamParserState::GetVps(
    uint32_t vps_id) const {
  // check the VPS exists in the bitstream parser state
  auto it = vps.find(vps_id);
  if (it == vps.end()) {
    return absl::nullopt;
  }
  return OptionalVpsState(it->second);
}

absl::optional<struct H265SpsParser::SpsState> H265BitstreamParserState::GetSps(
    uint32_t sps_id) const {
  // check the SPS exists in the bitstream parser state
  auto it = sps.find(sps_id);
  if (it == sps.end()) {
    return absl::nullopt;
  }
  return OptionalSpsState(it->second);
}

absl::optional<struct H265PpsParser::PpsState> H265BitstreamParserState::GetPps(
    uint32_t pps_id) const {
  // check the PPS exists in the bitstream parser state
  auto it = pps.find(pps_id);
  if (it == pps.end()) {
    return absl::nullopt;
  }
  return OptionalPpsState(it->second);
}

}  // namespace h265nal
