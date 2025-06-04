/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_utils.h"

#include <stdio.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

#include "h265_bitstream_parser.h"
#include "h265_bitstream_parser_state.h"
#include "h265_common.h"
#ifdef RTP_DEFINE
#include "h265_rtp_parser.h"
#endif  // RTP_DEFINE

namespace {
int ReadStdinToBuffer(std::vector<uint8_t>& buffer) {
  constexpr size_t chunk_size = 4096;
  uint8_t temp[chunk_size];
  while (true) {
    std::cin.read(reinterpret_cast<char*>(temp), chunk_size);
    std::streamsize bytes_read = std::cin.gcount();
    if (bytes_read <= 0) break;
    buffer.insert(buffer.end(), temp, temp + bytes_read);
  }
  return 0;
}
}  // namespace

namespace h265nal {

// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Calculate Luminance Slice QP values from slice header and PPS.
namespace {
// internal function
std::unique_ptr<int32_t> GetSliceQpYInternal(
    uint32_t nal_unit_type,
    std::unique_ptr<struct H265NalUnitPayloadParser::NalUnitPayloadState> const&
        payload,
    const struct H265BitstreamParserState* bitstream_parser_state) noexcept {
  // make sure the payload contains a slice header
  if (!IsSliceSegment(nal_unit_type)) {
    return nullptr;
  }

  // check some values
  if ((payload == nullptr) || (payload->slice_segment_layer == nullptr) ||
      (payload->slice_segment_layer->slice_segment_header == nullptr)) {
    return nullptr;
  }
  auto& slice_header = payload->slice_segment_layer->slice_segment_header;
  auto pps_id = slice_header->slice_pic_parameter_set_id;
  auto slice_qp_delta = slice_header->slice_qp_delta;

  // check the PPS exists in the bitstream parser state
  auto pps = bitstream_parser_state->GetPps(pps_id);
  if (pps == nullptr) {
    return nullptr;
  }
  const auto init_qp_minus26 = pps->init_qp_minus26;

  // Equation 7-54, Section 7.4.7.1
  int32_t slice_qpy = 26 + init_qp_minus26 + slice_qp_delta;

  return std::make_unique<int32_t>(slice_qpy);
}
}  // namespace

#ifdef RTP_DEFINE
std::unique_ptr<int32_t> H265Utils::GetSliceQpY(
    std::unique_ptr<struct H265RtpParser::RtpState> const& rtp,
    const H265BitstreamParserState* bitstream_parser_state) noexcept {
  // get the actual NAL header (not the RTP one)
  std::unique_ptr<struct H265NalUnitPayloadParser::NalUnitPayloadState>*
      payload;
  uint32_t nal_unit_type = 0;
  if (rtp->nal_unit_header->nal_unit_type <= 47) {
    auto& header = rtp->rtp_single->nal_unit_header;
    nal_unit_type = header->nal_unit_type;
    payload = &rtp->rtp_single->nal_unit_payload;
  } else if (rtp->nal_unit_header->nal_unit_type == AP) {
    // use the latest NAL unit in the AP
    size_t index = rtp->rtp_ap->nal_unit_headers.size() - 1;
    auto& header = rtp->rtp_ap->nal_unit_headers[index];
    nal_unit_type = header->nal_unit_type;
    payload = &rtp->rtp_ap->nal_unit_payloads[index];
  } else if (rtp->nal_unit_header->nal_unit_type == FU) {
    // check this is the first NAL of the frame
    if (rtp->rtp_fu->s_bit == 0) {
      return nullptr;
    }
    nal_unit_type = rtp->rtp_fu->fu_type;
    payload = &rtp->rtp_fu->nal_unit_payload;
  } else {
    // invalid value
    return nullptr;
  }
  // get the slice QP_Y value
  return GetSliceQpYInternal(nal_unit_type, *payload, bitstream_parser_state);
}
#endif  // RTP_DEFINE

std::vector<int32_t> H265Utils::GetSliceQpY(
    const uint8_t* data, size_t length,
    H265BitstreamParserState* bitstream_parser_state) noexcept {
  std::vector<int32_t> slice_qp_y_vector;

  // parse the incoming bitstream
  ParsingOptions parsing_options;
  parsing_options.add_checksum = false;
  auto bitstream_state = h265nal::H265BitstreamParser::ParseBitstream(
      data, length, bitstream_parser_state, parsing_options);
  // get all possible QP values
  for (auto& nal_unit : bitstream_state->nal_units) {
    uint32_t nal_unit_type = nal_unit->nal_unit_header->nal_unit_type;
    auto& payload = nal_unit->nal_unit_payload;
    // get the slice QP_Y value
    auto slice_qp_y_value =
        GetSliceQpYInternal(nal_unit_type, payload, bitstream_parser_state);
    if (slice_qp_y_value != nullptr) {
      slice_qp_y_vector.push_back(*slice_qp_y_value);
    }
  }
  return slice_qp_y_vector;
}

int H265Utils::ReadFile(const char* filename, std::vector<uint8_t>& buffer) {
  // TODO(chemag): read the infile incrementally
  FILE* infp = nullptr;
  if ((filename == nullptr) || (strlen(filename) == 1 && filename[0] == '-')) {
    // read from stdin
    return ReadStdinToBuffer(buffer);
  }

  // open the file
  infp = fopen(filename, "rb");
  if (infp == nullptr) {
    // did not work
    fprintf(stderr, "Could not open input file: \"%s\"\n", filename);
    return -1;
  }
  fseek(infp, 0, SEEK_END);
  int64_t size = ftell(infp);
  fseek(infp, 0, SEEK_SET);
  // read file into buffer
  buffer.resize(size);
  fread(reinterpret_cast<char*>(buffer.data()), 1, size, infp);
  // clean up
  fclose(infp);
  return 0;
}

}  // namespace h265nal
