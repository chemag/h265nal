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

  return 0;
}
