/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "h265_nal_unit_parser.h"
#include "rtc_common.h"

namespace h265nal {

// A class for parsing out a h265 configuration box (hvcC) data from
// an H265 NALU.
class H265ConfigurationBoxParser {
 public:
  // The parsed state of the hvcC. Only some selected values are stored.
  // Add more as they are actually needed.
  struct ConfigurationBoxState {
    ConfigurationBoxState() = default;
    ~ConfigurationBoxState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    ConfigurationBoxState(const ConfigurationBoxState&) = delete;
    ConfigurationBoxState(ConfigurationBoxState&&) = delete;
    ConfigurationBoxState& operator=(const ConfigurationBoxState&) = delete;
    ConfigurationBoxState& operator=(ConfigurationBoxState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level,
               ParsingOptions parsing_options) const;
#endif  // FDUMP_DEFINE

    uint32_t configurationVersion = 0;
    uint32_t general_profile_space = 0;
    uint32_t general_tier_flag = 0;
    uint32_t general_profile_idc = 0;
    std::array<uint32_t, 32> general_profile_compatibility_flags;
    uint64_t general_constraint_indicator_flags = 0;
    uint32_t general_level_idc = 0;
    uint32_t reserved1 = 0;
    uint32_t min_spatial_segmentation_idc = 0;
    uint32_t reserved2 = 0;
    uint32_t parallelismType = 0;
    uint32_t reserved3 = 0;
    uint32_t chromaFormat = 0;
    uint32_t reserved4 = 0;
    uint32_t bitDepthLumaMinus8 = 0;
    uint32_t reserved5 = 0;
    uint32_t bitDepthChromaMinus8 = 0;
    uint32_t avgFrameRate = 0;
    uint32_t constantFrameRate = 0;
    uint32_t numTemporalLayers = 0;
    uint32_t temporalIdNested = 0;
    uint32_t lengthSizeMinusOne = 0;
    uint32_t numOfArrays = 0;
    std::vector<uint32_t> array_completeness;
    std::vector<uint32_t> reserved6;
    std::vector<uint32_t> NAL_unit_type;
    std::vector<uint32_t> numNalus;
    std::vector<std::vector<uint32_t>> nalUnitLength;
    std::vector<
        std::vector<std::unique_ptr<struct H265NalUnitParser::NalUnitState>>>
        nalUnit;
  };

  // Unpack RBSP and parse hvcC state from the supplied buffer.
  static std::shared_ptr<ConfigurationBoxState> ParseConfigurationBox(
      const uint8_t* data, size_t length,
      struct H265BitstreamParserState* bitstream_parser_state,
      ParsingOptions parsing_options) noexcept;
  static std::shared_ptr<ConfigurationBoxState> ParseConfigurationBox(
      BitBuffer* bit_buffer,
      struct H265BitstreamParserState* bitstream_parser_state,
      ParsingOptions parsing_options) noexcept;
};

}  // namespace h265nal
