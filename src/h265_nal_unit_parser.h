/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>

#include "h265_aud_parser.h"
#include "h265_common.h"
#include "h265_pps_parser.h"
#include "h265_slice_parser.h"
#include "h265_sps_parser.h"
#include "h265_vps_parser.h"
#include "rtc_base/bit_buffer.h"

namespace h265nal {

// A class for parsing out an H265 NAL Unit Header.
class H265NalUnitHeaderParser {
 public:
  // The parsed state of the NAL Unit Header.
  struct NalUnitHeaderState {
    NalUnitHeaderState() = default;
    ~NalUnitHeaderState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    NalUnitHeaderState(const NalUnitHeaderState&) = delete;
    NalUnitHeaderState(NalUnitHeaderState&&) = delete;
    NalUnitHeaderState& operator=(const NalUnitHeaderState&) = delete;
    NalUnitHeaderState& operator=(NalUnitHeaderState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    uint32_t forbidden_zero_bit = 0;
    uint32_t nal_unit_type = 0;
    uint32_t nuh_layer_id = 0;
    uint32_t nuh_temporal_id_plus1 = 0;
  };

  // Unpack RBSP and parse NAL unit header state from the supplied buffer.
  static std::unique_ptr<NalUnitHeaderState> ParseNalUnitHeader(
      const uint8_t* data, size_t length) noexcept;
  static std::unique_ptr<NalUnitHeaderState> ParseNalUnitHeader(
      rtc::BitBuffer* bit_buffer) noexcept;
  // Parses nalu type from the given buffer
  static bool GetNalUnitType(const uint8_t* data, const size_t length,
                             NalUnitType& naluType) noexcept;
};

// A class for parsing out an H265 NAL Unit Payload.
class H265NalUnitPayloadParser {
 public:
  // The parsed state of the NAL Unit Payload. Only some select values are
  // stored. Add more as they are actually needed.
  struct NalUnitPayloadState {
    NalUnitPayloadState() = default;
    ~NalUnitPayloadState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    NalUnitPayloadState(const NalUnitPayloadState&) = delete;
    NalUnitPayloadState(NalUnitPayloadState&&) = delete;
    NalUnitPayloadState& operator=(const NalUnitPayloadState&) = delete;
    NalUnitPayloadState& operator=(NalUnitPayloadState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level, uint32_t nal_unit_type) const;
#endif  // FDUMP_DEFINE

    std::shared_ptr<struct H265VpsParser::VpsState> vps;
    std::shared_ptr<struct H265SpsParser::SpsState> sps;
    std::shared_ptr<struct H265PpsParser::PpsState> pps;
    std::unique_ptr<struct H265AudParser::AudState> aud;
    std::unique_ptr<struct H265SliceSegmentLayerParser::SliceSegmentLayerState>
        slice_segment_layer;
  };

  // Unpack RBSP and parse NAL unit payload state from the supplied buffer.
  static std::unique_ptr<NalUnitPayloadState> ParseNalUnitPayload(
      const uint8_t* data, size_t length, uint32_t nal_unit_type,
      struct H265BitstreamParserState* bitstream_parser_state) noexcept;
  static std::unique_ptr<NalUnitPayloadState> ParseNalUnitPayload(
      rtc::BitBuffer* bit_buffer, uint32_t nal_unit_type,
      struct H265BitstreamParserState* bitstream_parser_state) noexcept;
};

// A class for parsing out an H265 NAL Unit.
class H265NalUnitParser {
 public:
  // The parsed state of the NAL Unit. Only some select values are stored.
  // Add more as they are actually needed.
  struct NalUnitState {
    NalUnitState() = default;
    ~NalUnitState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    NalUnitState(const NalUnitState&) = delete;
    NalUnitState(NalUnitState&&) = delete;
    NalUnitState& operator=(const NalUnitState&) = delete;
    NalUnitState& operator=(NalUnitState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level, bool add_offset, bool add_length,
               bool add_parsed_length, bool add_checksum) const;
#endif  // FDUMP_DEFINE

    // NAL Unit offset in the full blob
    size_t offset;
    // NAL Unit length
    size_t length;
    // NAL Unit parsed length
    size_t parsed_length;
    // NAL Unit checksum
    std::shared_ptr<NaluChecksum> checksum;

    std::unique_ptr<struct H265NalUnitHeaderParser::NalUnitHeaderState>
        nal_unit_header;
    std::unique_ptr<struct H265NalUnitPayloadParser::NalUnitPayloadState>
        nal_unit_payload;
  };

  // Unpack RBSP and parse NAL unit state from the supplied buffer.
  static std::unique_ptr<NalUnitState> ParseNalUnit(
      const uint8_t* data, size_t length,
      struct H265BitstreamParserState* bitstream_parser_state,
      bool add_checksum) noexcept;
  static std::unique_ptr<NalUnitState> ParseNalUnit(
      rtc::BitBuffer* bit_buffer,
      struct H265BitstreamParserState* bitstream_parser_state,
      bool add_checksum) noexcept;
};

}  // namespace h265nal
