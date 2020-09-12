/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */


#include "h265_bitstream_parser.h"

#include <stdio.h>

#include <cstdint>
#include <vector>

#include "h265_common.h"
#include "absl/types/optional.h"
#include "rtc_base/bit_buffer.h"

namespace {
typedef absl::optional<h265nal::H265BitstreamParser::
    BitstreamState> OptionalBitstream;
typedef absl::optional<h265nal::H265NalUnitParser::NalUnitState>
    OptionalNalUnit;

// The size of a full NALU start sequence {0 0 0 1}, used for the first NALU
// of an access unit, and for SPS and PPS blocks.
const size_t kNaluLongStartSequenceSize = 4;

// The size of a shortened NALU start sequence {0 0 1}, that may be used if
// not the first NALU of an access unit or an SPS or PPS block.
const size_t kNaluShortStartSequenceSize = 3;
}  // namespace


namespace h265nal {

// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

std::vector<H265BitstreamParser::NaluIndex>
H265BitstreamParser::FindNaluIndices(const uint8_t* data, size_t length) {
  // This is sorta like Boyer-Moore, but with only the first optimization step:
  // given a 3-byte sequence we're looking at, if the 3rd byte isn't 1 or 0,
  // skip ahead to the next 3-byte sequence. 0s and 1s are relatively rare, so
  // this will skip the majority of reads/checks.
  std::vector<NaluIndex> sequences;
  if (length < kNaluShortStartSequenceSize)
    return sequences;

  const size_t end = length - kNaluShortStartSequenceSize;
  for (size_t i = 0; i < end;) {
    if (data[i + 2] > 1) {
      i += 3;
    } else if (data[i + 2] == 0x01 && data[i + 1] == 0x00 && data[i] == 0x00) {
      // We found a start sequence, now check if it was a 3 of 4 byte one.
      NaluIndex index = {i, i + 3, 0};
      if (index.start_offset > 0 && data[index.start_offset - 1] == 0)
        --index.start_offset;

      // Update length of previous entry.
      auto it = sequences.rbegin();
      if (it != sequences.rend())
        it->payload_size = index.start_offset - it->payload_start_offset;

      sequences.push_back(index);

      i += 3;
    } else {
      ++i;
    }
  }

  // Update length of last entry, if any.
  auto it = sequences.rbegin();
  if (it != sequences.rend())
    it->payload_size = length - it->payload_start_offset;

  return sequences;
}


// Unpack RBSP and parse NAL Unit state from the supplied buffer.
absl::optional<H265BitstreamParser::BitstreamState>
H265BitstreamParser::ParseBitstream(const uint8_t* data, size_t length) {
  BitstreamState bitstream;

  std::vector<NaluIndex> nalu_indices = FindNaluIndices(data, length);
  for (const NaluIndex& nalu_index : nalu_indices) {
    OptionalNalUnit nal_unit = H265NalUnitParser::ParseNalUnit(
        &data[nalu_index.payload_start_offset],
        nalu_index.payload_size);
      if (nal_unit != absl::nullopt) {
        nal_unit->offset = nalu_index.payload_start_offset;
        nal_unit->length = nalu_index.payload_size;
        bitstream.nal_units.push_back(*nal_unit);
      }
  }
  return OptionalBitstream(bitstream);
}

void H265BitstreamParser::BitstreamState::fdump(
    FILE* outfp, int indent_level) const {
  for (const struct H265NalUnitParser::NalUnitState& nal_unit : nal_units) {
    nal_unit.fdump(outfp, indent_level, add_offset, add_length);
    fprintf(outfp, "\n");
  }
}

}  // namespace h265nal
