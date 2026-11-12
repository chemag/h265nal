/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// Hand-written fuzz target, NOT auto-generated.
//
// converter.py derives a target from the Parse*(data, size) calls in a unit
// test. h265_common_unittest.cc has none: it exercises UnescapeRbsp() and
// the Section 7.2 syntax helpers, which take other shapes. There is
// therefore no h265_common_fuzzer at all, and this surface has never been
// fuzzed.
//
// This file covers it directly. converter.py names its output after the
// unit test it converts (converter.py:53, s/unittest/fuzzer/), so nothing
// regenerates this file while there is no
// test/h265_common__unescape_rbsp_unittest.cc. If one is ever added, rename
// this target rather than letting the generator clobber it.

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "h265_common.h"
#include "rtc_common.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  // UnescapeRbsp() scans for the 00 00 03 emulation prevention sequence and
  // drops the 03. Reached today only via the byte-stream parsers, so the
  // buffer it sees is always a well-formed NAL unit payload.
  {
    std::vector<uint8_t> unescaped = h265nal::UnescapeRbsp(data, size);
    // Read the result back so the returned buffer is not optimized away,
    // and so its contents are actually touched.
    if (!unescaped.empty()) {
      h265nal::BitBuffer bit_buffer(unescaped.data(), unescaped.size());
      uint32_t val = 0;
      bit_buffer.ReadBits(1, val);
    }
  }

  // The Section 7.2 syntax helpers. Consume a prefix of the input first so
  // they are exercised at varying bit offsets, including unaligned ones,
  // rather than only at offset zero.
  {
    h265nal::BitBuffer bit_buffer(data, size);
    if (size > 0) {
      // 0..16 bits, so the read position crosses byte boundaries.
      size_t bits_to_skip = data[0] % 17;
      if (bits_to_skip > 0) {
        uint32_t scratch = 0;
        bit_buffer.ReadBits(bits_to_skip, scratch);
      }
    }
    h265nal::byte_aligned(&bit_buffer);
    h265nal::get_current_offset(&bit_buffer);
    h265nal::more_rbsp_data(&bit_buffer);
    h265nal::rbsp_trailing_bits(&bit_buffer);
  }

  // The NAL unit type and profile helpers, on every input byte: The parsers
  // only call them with values a bitstream syntax element can hold, so
  // out-of-range values (e.g. a 5-bit nal_unit_type field holds 0..31, but
  // the helpers take a uint32_t) are never seen otherwise.
  {
    std::string str;
    for (size_t i = 0; i < size && i < 64; i++) {
      uint32_t value = data[i];
      if (i + 1 < size && (data[i] & 0x80)) {
        // sometimes widen to 16 bits, to reach values above 255
        value = (value << 8) | data[i + 1];
      }
      h265nal::NalUnitTypeToString(value);
      h265nal::IsSliceSegment(value);
      h265nal::IsNalUnitTypeVcl(value);
      h265nal::IsNalUnitTypeNonVcl(value);
      h265nal::IsNalUnitTypeUnspecified(value);
      h265nal::profileTypeToString(static_cast<h265nal::ProfileType>(data[i]),
                                   str);
    }
  }

  // NaluChecksum, on the input from a varying bit offset, then dumped
  // through fdump() into a buffer of varying length. The length starts at
  // 1: fdump() with output_len 0 writes output[-1], a known bug, and the
  // fuzzer would find it on every run.
  {
    h265nal::BitBuffer bit_buffer(data, size);
    if (size > 0) {
      size_t bits_to_skip = data[0] % 17;
      if (bits_to_skip > 0) {
        uint32_t scratch = 0;
        bit_buffer.ReadBits(bits_to_skip, scratch);
      }
    }
    auto checksum = h265nal::NaluChecksum::GetNaluChecksum(&bit_buffer);
    if (checksum != nullptr) {
      char output[2 * h265nal::NaluChecksum::kMaxLength + 1];
      int output_len = 1;
      if (size > 0) {
        output_len += data[size - 1] % static_cast<int>(sizeof(output));
      }
      checksum->fdump(output, output_len);
      checksum->GetPrintableChecksum();
    }
  }

  return 0;
}
