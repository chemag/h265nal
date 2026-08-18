/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "rtc_common.h"

namespace h265nal {

// A class for parsing out a picture parameter set range extension syntax
// (`pps_range_extension()`, as defined in Section 7.3.2.3.2 of the 2016-12
// standard) from an H265 NALU.
class H265PpsRangeExtensionParser {
 public:
  // Section 7.4.3.3.2: "The value of diff_cu_chroma_qp_offset_depth shall
  // be in the range of 0 to log2_diff_max_min_luma_coding_block_size,
  // inclusive". That lives in the sps, which ParsePpsRangeExtension() does
  // not resolve, so bound it by the largest value it can reach instead:
  // CtbLog2SizeY is at most 6 and MinCbLog2SizeY at least 3.
  const static uint32_t kDiffCuChromaQpOffsetDepthMin = 0;
  const static uint32_t kDiffCuChromaQpOffsetDepthMax = 3;

  // Section 7.4.3.3.2: "The value of chroma_qp_offset_list_len_minus1
  // shall be in the range of 0 to 5, inclusive".
  const static uint32_t kChromaQpOffsetListLenMinus1Min = 0;
  const static uint32_t kChromaQpOffsetListLenMinus1Max = 5;

  // Section 7.4.3.3.2: "The value of cb_qp_offset_list[i] and
  // cr_qp_offset_list[i] shall be in the range of -12 to +12, inclusive".
  const static int32_t kQpOffsetListMin = -12;
  const static int32_t kQpOffsetListMax = 12;

  // Section 7.4.3.3.2: "The value of log2_sao_offset_scale_luma shall be
  // in the range of 0 to Max(0, BitDepthY - 10), inclusive", and
  // log2_sao_offset_scale_chroma likewise against BitDepthC. Both bit
  // depths live in the sps, and are at most 16.
  const static uint32_t kLog2SaoOffsetScaleMin = 0;
  const static uint32_t kLog2SaoOffsetScaleMax = 6;

  // The parsed state of the PpsRangeExtension.
  struct PpsRangeExtensionState {
    PpsRangeExtensionState() = default;
    ~PpsRangeExtensionState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    PpsRangeExtensionState(const PpsRangeExtensionState&) = delete;
    PpsRangeExtensionState(PpsRangeExtensionState&&) = delete;
    PpsRangeExtensionState& operator=(const PpsRangeExtensionState&) = delete;
    PpsRangeExtensionState& operator=(PpsRangeExtensionState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    // input parameters
    uint32_t transform_skip_enabled_flag = 0;

    // contents
    uint32_t log2_max_transform_skip_block_size_minus2 = 0;
    uint32_t cross_component_prediction_enabled_flag = 0;
    uint32_t chroma_qp_offset_list_enabled_flag = 0;
    uint32_t diff_cu_chroma_qp_offset_depth = 0;
    uint32_t chroma_qp_offset_list_len_minus1 = 0;
    std::vector<int32_t> cb_qp_offset_list;
    std::vector<int32_t> cr_qp_offset_list;
    uint32_t log2_sao_offset_scale_luma = 0;
    uint32_t log2_sao_offset_scale_chroma = 0;
  };

  // Unpack RBSP and parse PpsRangeExtension state from the supplied buffer.
  static std::unique_ptr<PpsRangeExtensionState> ParsePpsRangeExtension(
      const uint8_t* data, size_t length,
      uint32_t transform_skip_enabled_flag) noexcept;
  static std::unique_ptr<PpsRangeExtensionState> ParsePpsRangeExtension(
      BitBuffer* bit_buffer, uint32_t transform_skip_enabled_flag) noexcept;
};

}  // namespace h265nal
