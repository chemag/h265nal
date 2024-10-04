/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "h265_bitstream_parser_state.h"
#include "h265_common.h"
#include "h265_nal_unit_parser.h"
#include "rtc_common.h"

namespace h265nal {

// A class for parsing out an H265 Bitstream.
class H265BitstreamParser {
 public:
  // The parsed state of the bitstream (a list of parsed NAL units plus
  // the accumulated VPS/PPS/SPS state).
  struct BitstreamState {
    BitstreamState() = default;
    ~BitstreamState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    BitstreamState(const BitstreamState&) = delete;
    BitstreamState(BitstreamState&&) = delete;
    BitstreamState& operator=(const BitstreamState&) = delete;
    BitstreamState& operator=(BitstreamState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE
    struct ParsingOptions parsing_options;
    // NAL units
    std::vector<std::unique_ptr<struct H265NalUnitParser::NalUnitState>>
        nal_units;
  };

  // (1) Explicit NAL unit separator version.
  // Unpack RBSP and parse bitstream state from the supplied buffer.
  static std::unique_ptr<BitstreamState> ParseBitstream(
      const uint8_t* data, size_t length,
      H265BitstreamParserState* bitstream_parser_state,
      ParsingOptions parsing_options) noexcept;

  // Unpack RBSP and parse bitstream (internal state)
  static std::unique_ptr<BitstreamState> ParseBitstream(
      const uint8_t* data, size_t length,
      ParsingOptions parsing_options) noexcept;

  // (2) Explicit NAL unit length field version.
  // NALU length
  static std::unique_ptr<BitstreamState> ParseBitstreamNALULength(
      const uint8_t* data, size_t length, size_t nalu_length_bytes,
      H265BitstreamParserState* bitstream_parser_state,
      ParsingOptions parsing_options) noexcept;
  static std::unique_ptr<BitstreamState> ParseBitstreamNALULength(
      const uint8_t* data, size_t length, size_t nalu_length_bytes,
      ParsingOptions parsing_options) noexcept;

  struct NaluIndex {
    // Start index of NALU, including start sequence.
    size_t start_offset;
    // Start index of NALU payload, typically type header.
    size_t payload_start_offset;
    // Length of NALU payload, in bytes, counting from payload_start_offset.
    size_t payload_size;
  };
  // Returns a vector of the NALU indices in the given buffer.
  static std::vector<NaluIndex> FindNaluIndices(const uint8_t* data,
                                                size_t length) noexcept;
  static std::vector<NaluIndex> FindNaluIndicesExplicitFraming(
      const uint8_t* data, size_t length) noexcept;
};

}  // namespace h265nal
