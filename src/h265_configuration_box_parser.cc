/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_configuration_box_parser.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h265_common.h"
#include "h265_nal_unit_parser.h"

namespace h265nal {

// General note: this is based on hvcC (HEVCConfigurationBoxParser aka h265
// configuration box aka isobmff HEVCDecoderConfigurationRecord aka hevc
// configuration record), as defined in ISO/IEC 14496-15:2022, Section 8.3.2.1.2

// Unpack RBSP and parse hvcC state from the supplied buffer.
std::shared_ptr<H265ConfigurationBoxParser::ConfigurationBoxState>
H265ConfigurationBoxParser::ParseConfigurationBox(
    const uint8_t* data, size_t length,
    struct H265BitstreamParserState* bitstream_parser_state,
    ParsingOptions parsing_options) noexcept {
  // hvcC boxes are already unescaped
  BitBuffer bit_buffer(data, length);

  return ParseConfigurationBox(&bit_buffer, bitstream_parser_state,
                               parsing_options);
}

std::shared_ptr<H265ConfigurationBoxParser::ConfigurationBoxState>
H265ConfigurationBoxParser::ParseConfigurationBox(
    BitBuffer* bit_buffer,
    struct H265BitstreamParserState* bitstream_parser_state,
    ParsingOptions parsing_options) noexcept {
  uint32_t bits_tmp;
  uint32_t golomb_tmp;

  // H265 configuration box (HEVCDecoderConfigurationRecord()) blob.
  auto configuration_box = std::make_shared<ConfigurationBoxState>();

  // unsigned int(8) configurationVersion = 1;
  if (!bit_buffer->ReadBits(8, configuration_box->configurationVersion)) {
    return nullptr;
  }
  if (configuration_box->configurationVersion != 1) {
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: configurationVersion is not 1: %u\n",
            configuration_box->configurationVersion);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // unsigned int(2) general_profile_space;
  if (!bit_buffer->ReadBits(2, configuration_box->general_profile_space)) {
    return nullptr;
  }

  // unsigned int(1) general_tier_flag;
  if (!bit_buffer->ReadBits(1, configuration_box->general_tier_flag)) {
    return nullptr;
  }

  // unsigned int(5) general_profile_idc;
  if (!bit_buffer->ReadBits(5, configuration_box->general_profile_idc)) {
    return nullptr;
  }

  // unsigned int(32) general_profile_compatibility_flags;
  for (uint32_t j = 0; j < 32; j++) {
    if (!bit_buffer->ReadBits(
            1, configuration_box->general_profile_compatibility_flags[j])) {
      return nullptr;
    }
  }

  // unsigned int(48) general_constraint_indicator_flags;
  if (!bit_buffer->ReadBits(
          48, configuration_box->general_constraint_indicator_flags)) {
    return nullptr;
  }

  // unsigned int(8) general_level_idc;
  if (!bit_buffer->ReadBits(8, configuration_box->general_level_idc)) {
    return nullptr;
  }

  // bit(4) reserved = `1111'b;
  if (!bit_buffer->ReadBits(4, configuration_box->reserved1)) {
    return nullptr;
  }
  if (configuration_box->reserved1 != 0b1111) {
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: reserved1 is not 0b1111: %u\n",
            configuration_box->reserved1);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // unsigned int(12) min_spatial_segmentation_idc;
  if (!bit_buffer->ReadBits(12,
                            configuration_box->min_spatial_segmentation_idc)) {
    return nullptr;
  }

  // bit(6) reserved = `111111'b;
  if (!bit_buffer->ReadBits(6, configuration_box->reserved2)) {
    return nullptr;
  }
  if (configuration_box->reserved2 != 0b111111) {
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: reserved2 is not 0b111111: %u\n",
            configuration_box->reserved2);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // unsigned int(2) parallelismType;
  if (!bit_buffer->ReadBits(2, configuration_box->parallelismType)) {
    return nullptr;
  }

  // bit(6) reserved = `111111'b;
  if (!bit_buffer->ReadBits(6, configuration_box->reserved3)) {
    return nullptr;
  }
  if (configuration_box->reserved3 != 0b111111) {
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: reserved3 is not 0b111111: %u\n",
            configuration_box->reserved3);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // unsigned int(2) chroma_format_idc;
  if (!bit_buffer->ReadBits(2, configuration_box->chroma_format_idc)) {
    return nullptr;
  }

  // bit(5) reserved = `11111'b;
  if (!bit_buffer->ReadBits(5, configuration_box->reserved4)) {
    return nullptr;
  }
  if (configuration_box->reserved4 != 0b11111) {
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: reserved4 is not 0b11111: %u\n",
            configuration_box->reserved4);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // unsigned int(3) bit_depth_luma_minus8;
  if (!bit_buffer->ReadBits(3, configuration_box->bit_depth_luma_minus8)) {
    return nullptr;
  }

  // bit(5) reserved = `11111'b;
  if (!bit_buffer->ReadBits(5, configuration_box->reserved5)) {
    return nullptr;
  }
  if (configuration_box->reserved5 != 0b11111) {
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: reserved5 is not 0b11111: %u\n",
            configuration_box->reserved5);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // unsigned int(3) bit_depth_chroma_minus8;
  if (!bit_buffer->ReadBits(3, configuration_box->bit_depth_chroma_minus8)) {
    return nullptr;
  }

  // bit(16) avgFrameRate;
  if (!bit_buffer->ReadBits(16, configuration_box->avgFrameRate)) {
    return nullptr;
  }

  // bit(2) constantFrameRate;
  if (!bit_buffer->ReadBits(2, configuration_box->constantFrameRate)) {
    return nullptr;
  }

  // bit(3) numTemporalLayers;
  if (!bit_buffer->ReadBits(3, configuration_box->numTemporalLayers)) {
    return nullptr;
  }

  // bit(1) temporalIdNested;
  if (!bit_buffer->ReadBits(1, configuration_box->temporalIdNested)) {
    return nullptr;
  }

  // unsigned int(2) lengthSizeMinusOne;
  if (!bit_buffer->ReadBits(2, configuration_box->lengthSizeMinusOne)) {
    return nullptr;
  }

  // unsigned int(8) numOfArrays;
  if (!bit_buffer->ReadBits(8, configuration_box->numOfArrays)) {
    return nullptr;
  }

  for (int j = 0; j < configuration_box->numOfArrays; j++) {
    // bit(1) array_completeness;
    if (!bit_buffer->ReadBits(1, bits_tmp)) {
      return nullptr;
    }
    configuration_box->array_completeness.push_back(bits_tmp);

    // unsigned int(1) reserved = 0;
    if (!bit_buffer->ReadBits(1, bits_tmp)) {
      return nullptr;
    }
    configuration_box->reserved6.push_back(bits_tmp);
    if (configuration_box->reserved6.back() != 0) {
#ifdef FPRINT_ERRORS
      fprintf(stderr, "error: reserved6[%i] is not 0: %u\n", j,
              configuration_box->reserved6.back());
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // unsigned int(6) NAL_unit_type;
    if (!bit_buffer->ReadBits(6, bits_tmp)) {
      return nullptr;
    }
    configuration_box->NAL_unit_type.push_back(bits_tmp);

    // unsigned int(16) numNalus;
    if (!bit_buffer->ReadBits(16, bits_tmp)) {
      return nullptr;
    }
    configuration_box->numNalus.push_back(bits_tmp);

    configuration_box->nalUnitLength.emplace_back();
    configuration_box->nalUnit.emplace_back();
    for (int i = 0; i < configuration_box->numNalus.back(); i++) {
      // unsigned int(16) nalUnitLength;
      if (!bit_buffer->ReadBits(16, bits_tmp)) {
        return nullptr;
      }
      configuration_box->nalUnitLength.back().push_back(bits_tmp);

      // bit(8*nalUnitLength) nalUnit;
      size_t length = configuration_box->nalUnitLength.back().back();
      uint8_t data[length];
      if (!bit_buffer->ReadBytes(length, data)) {
        return nullptr;
      }
      configuration_box->nalUnit.back().push_back(
          H265NalUnitParser::ParseNalUnit(data, length, bitstream_parser_state,
                                          parsing_options));
    }
  }

  return configuration_box;
}

#ifdef FDUMP_DEFINE
void H265ConfigurationBoxParser::ConfigurationBoxState::fdump(
    FILE* outfp, int indent_level, ParsingOptions parsing_options) const {
  fprintf(outfp, "configuration_box {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "configurationVersion: %i", configurationVersion);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "general_profile_space: %i", general_profile_space);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "general_tier_flag: %i", general_tier_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "general_profile_idc: %i", general_profile_idc);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "general_profile_compatibility_flags {");
  for (const uint32_t& v : general_profile_compatibility_flags) {
    fprintf(outfp, " %i", v);
  }
  fprintf(outfp, " }");

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "general_constraint_indicator_flags: %lu",
          general_constraint_indicator_flags);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "general_level_idc: %i", general_level_idc);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved1: %i", reserved1);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "min_spatial_segmentation_idc: %i",
          min_spatial_segmentation_idc);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved2: %i", reserved2);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "parallelismType: %i", parallelismType);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved3: %i", reserved3);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "chroma_format_idc: %i", chroma_format_idc);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved4: %i", reserved4);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "bit_depth_luma_minus8: %i", bit_depth_luma_minus8);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved5: %i", reserved5);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "bit_depth_chroma_minus8: %i", bit_depth_chroma_minus8);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "avgFrameRate: %i", avgFrameRate);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "constantFrameRate: %i", constantFrameRate);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "numTemporalLayers: %i", numTemporalLayers);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "temporalIdNested: %i", temporalIdNested);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "lengthSizeMinusOne: %i", lengthSizeMinusOne);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "numOfArrays: %i", numOfArrays);

  for (int j = 0; j < numOfArrays; j++) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "array_completeness[%i]: %i", j, array_completeness[j]);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "reserved6[%i]: %i", j, reserved6[j]);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "NAL_unit_type[%i]: %i", j, NAL_unit_type[j]);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "numNalus[%i]: %i", j, numNalus[j]);

    for (int i = 0; i < numNalus[j]; i++) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "nalUnitLength[%i][%i]: %i", j, i, nalUnitLength[j][i]);

      fdump_indent_level(outfp, indent_level);
      nalUnit[j][i]->fdump(outfp, indent_level, parsing_options);
    }
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
