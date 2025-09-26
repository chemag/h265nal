/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "rtc_common.h"

namespace h265nal {

// A class for parsing out a sequence parameter set multilayer extension syntax
// (`pps_multilayer_extension()`, as defined in Section F.7.3.2.3.4 of the
// 2018-02 standard) from an H265 NALU.
class H265PpsMultilayerExtensionParser {
 public:
  // The parsed state of the PpsMultilayerExtension.
  struct PpsMultilayerExtensionState {
    PpsMultilayerExtensionState() = default;
    ~PpsMultilayerExtensionState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    PpsMultilayerExtensionState(const PpsMultilayerExtensionState&) = delete;
    PpsMultilayerExtensionState(PpsMultilayerExtensionState&&) = delete;
    PpsMultilayerExtensionState& operator=(const PpsMultilayerExtensionState&) =
        delete;
    PpsMultilayerExtensionState& operator=(PpsMultilayerExtensionState&&) =
        delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    // contents
    uint32_t poc_reset_info_present_flag = 0;
    uint32_t pps_infer_scaling_list_flag = 0;
    uint32_t pps_scaling_list_ref_layer_id = 0;
    uint32_t num_ref_loc_offsets = 0;
    std::vector<uint32_t> ref_loc_offset_layer_id;
    std::vector<uint32_t> scaled_ref_layer_offset_present_flag;
    std::vector<int32_t> scaled_ref_layer_left_offset;
    std::vector<int32_t> scaled_ref_layer_top_offset;
    std::vector<int32_t> scaled_ref_layer_right_offset;
    std::vector<int32_t> scaled_ref_layer_bottom_offset;
    std::vector<uint32_t> ref_region_offset_present_flag;
    std::vector<int32_t> ref_region_left_offset;
    std::vector<int32_t> ref_region_top_offset;
    std::vector<int32_t> ref_region_right_offset;
    std::vector<int32_t> ref_region_bottom_offset;
    std::vector<uint32_t> resample_phase_set_present_flag;
    std::vector<int32_t> phase_hor_luma;
    std::vector<int32_t> phase_ver_luma;
    std::vector<int32_t> phase_hor_chroma_plus8;
    std::vector<int32_t> phase_ver_chroma_plus8;
    uint32_t colour_mapping_enabled_flag;
    // colour_mapping_table(()
  };

  // Unpack RBSP and parse PpsMultilayerExtension state from the supplied
  // buffer.
  static std::unique_ptr<PpsMultilayerExtensionState>
  ParsePpsMultilayerExtension(const uint8_t* data, size_t length) noexcept;
  static std::unique_ptr<PpsMultilayerExtensionState>
  ParsePpsMultilayerExtension(BitBuffer* bit_buffer) noexcept;
};

}  // namespace h265nal
