/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <cstdint>
#include <vector>


namespace h265nal {
// Methods for parsing RBSP. See section 7.4.1 of the H265 spec.
//
// Decoding is simply a matter of finding any 00 00 03 sequence and removing
// the 03 byte (emulation byte).

// Parse the given data and remove any emulation byte escaping.
std::vector<uint8_t> ParseRbsp(const uint8_t* data, size_t length);

}  // namespace h265nal
