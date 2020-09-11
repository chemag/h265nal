/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <cstdint>
#include <vector>

#include "rtc_base/bit_buffer.h"


namespace h265nal {
// Methods for parsing RBSP. See section 7.4.1 of the H265 spec.
//
// Decoding is simply a matter of finding any 00 00 03 sequence and removing
// the 03 byte (emulation byte).

// Remove any emulation byte escaping from a buffer. This is needed for
// byte-stream format packetization (e.g. Annex B data), but not for
// packet-stream format packetization (e.g. RTP payloads).
std::vector<uint8_t> UnescapeRbsp(const uint8_t* data, size_t length);

// Syntax functions and descriptors) (Section 7.2)
bool byte_aligned(rtc::BitBuffer *bit_buffer);
bool more_rbsp_data(rtc::BitBuffer *bit_buffer);
bool rbsp_trailing_bits(rtc::BitBuffer *bit_buffer);

}  // namespace h265nal
