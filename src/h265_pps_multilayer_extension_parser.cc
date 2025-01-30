/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_pps_multilayer_extension_parser.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h265_common.h"

namespace h265nal {

// General note: this is based off the 2024/07 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Unpack RBSP and parse pps_multilayer_extension state from the supplied
// buffer.
std::unique_ptr<H265PpsMultilayerExtensionParser::PpsMultilayerExtensionState>
H265PpsMultilayerExtensionParser::ParsePpsMultilayerExtension(
    const uint8_t* data, size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParsePpsMultilayerExtension(&bit_buffer);
}

std::unique_ptr<H265PpsMultilayerExtensionParser::PpsMultilayerExtensionState>
H265PpsMultilayerExtensionParser::ParsePpsMultilayerExtension(
    BitBuffer* bit_buffer) noexcept {
  uint32_t bits_tmp;
  int32_t sgolomb_tmp;

  // H265 pps_multilayer_extension() NAL Unit.
  // Section F.7.3.2.3.4 ("Picture parameter set multilayer extension syntax")
  // of the H.265 standard for a complete description.
  auto pps_multilayer_extension =
      std::make_unique<PpsMultilayerExtensionState>();

  // poc_reset_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(
          1, pps_multilayer_extension->poc_reset_info_present_flag)) {
    return nullptr;
  }

  // pps_infer_scaling_list_flag  u(1)
  if (!bit_buffer->ReadBits(
          1, pps_multilayer_extension->pps_infer_scaling_list_flag)) {
    return nullptr;
  }

  if (pps_multilayer_extension->pps_infer_scaling_list_flag) {
    // pps_scaling_list_ref_layer_id  u(6)
    if (!bit_buffer->ReadBits(
            6, pps_multilayer_extension->pps_scaling_list_ref_layer_id)) {
      return nullptr;
    }
  }

  // num_ref_loc_offsets  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(
          pps_multilayer_extension->num_ref_loc_offsets)) {
    return nullptr;
  }

  for (uint32_t i = 0; i < pps_multilayer_extension->num_ref_loc_offsets; i++) {
    // ref_loc_offset_layer_id[i]  u(6)
    if (!bit_buffer->ReadBits(6, bits_tmp)) {
      return nullptr;
    }
    pps_multilayer_extension->ref_loc_offset_layer_id.push_back(bits_tmp);

    // scaled_ref_layer_offset_present_flag[i]  u(1)
    if (!bit_buffer->ReadBits(1, bits_tmp)) {
      return nullptr;
    }
    pps_multilayer_extension->scaled_ref_layer_offset_present_flag.push_back(
        bits_tmp);

    if (pps_multilayer_extension->scaled_ref_layer_offset_present_flag.back()) {
      // TODO(chema): fix this: should be a dictionary
      // scaled_ref_layer_left_offset[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->scaled_ref_layer_left_offset.push_back(
          sgolomb_tmp);

      // scaled_ref_layer_top_offset[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->scaled_ref_layer_top_offset.push_back(
          sgolomb_tmp);

      // scaled_ref_layer_right_offset[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->scaled_ref_layer_right_offset.push_back(
          sgolomb_tmp);

      // scaled_ref_layer_bottom_offset[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->scaled_ref_layer_bottom_offset.push_back(
          sgolomb_tmp);
    }

    // ref_region_offset_present_flag[i]  u(1)
    if (!bit_buffer->ReadBits(1, bits_tmp)) {
      return nullptr;
    }
    pps_multilayer_extension->ref_region_offset_present_flag.push_back(
        bits_tmp);

    if (pps_multilayer_extension->ref_region_offset_present_flag.back()) {
      // ref_region_left_offset[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->ref_region_left_offset.push_back(sgolomb_tmp);

      // ref_region_top_offset[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->ref_region_top_offset.push_back(sgolomb_tmp);

      // ref_region_right_offset[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->ref_region_right_offset.push_back(sgolomb_tmp);

      // ref_region_bottom_offset[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->ref_region_bottom_offset.push_back(sgolomb_tmp);
    }

    // resample_phase_set_present_flag[i]  u(1)
    if (!bit_buffer->ReadBits(1, bits_tmp)) {
      return nullptr;
    }
    pps_multilayer_extension->resample_phase_set_present_flag.push_back(
        bits_tmp);

    if (pps_multilayer_extension->resample_phase_set_present_flag.back()) {
      // phase_hor_luma[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->phase_hor_luma.push_back(sgolomb_tmp);

      // phase_ver_luma[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->phase_ver_luma.push_back(sgolomb_tmp);

      // phase_hor_chroma_plus8[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->phase_hor_chroma_plus8.push_back(sgolomb_tmp);

      // phase_ver_chroma_plus8[ref_loc_offset_layer_id[i]]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      pps_multilayer_extension->phase_ver_chroma_plus8.push_back(sgolomb_tmp);
    }
  }

  // colour_mapping_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(
          1, pps_multilayer_extension->colour_mapping_enabled_flag)) {
    return nullptr;
  }

  if (pps_multilayer_extension->colour_mapping_enabled_flag) {
    // colour_mapping_table(()
    // TODO(chemag): add support for colour_mapping_table(()
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "error: unimplemented colour_mapping_table(() in "
            "pps_multilayer_extension(\n");
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  return pps_multilayer_extension;
}

#ifdef FDUMP_DEFINE
void H265PpsMultilayerExtensionParser::PpsMultilayerExtensionState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "pps_multilayer_extension {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "poc_reset_info_present_flag: %i",
          poc_reset_info_present_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pps_infer_scaling_list_flag: %i",
          pps_infer_scaling_list_flag);

  if (pps_infer_scaling_list_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "pps_scaling_list_ref_layer_id: %i",
            pps_scaling_list_ref_layer_id);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "num_ref_loc_offsets: %i", num_ref_loc_offsets);

  if (num_ref_loc_offsets > 0) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ref_loc_offset_layer_id {");
    for (const uint32_t& v : ref_loc_offset_layer_id) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "scaled_ref_layer_offset_present_flag {");
    for (const uint32_t& v : scaled_ref_layer_offset_present_flag) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "scaled_ref_layer_left_offset {");
    for (const int32_t& v : scaled_ref_layer_left_offset) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "scaled_ref_layer_top_offset {");
    for (const int32_t& v : scaled_ref_layer_top_offset) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "scaled_ref_layer_right_offset {");
    for (const int32_t& v : scaled_ref_layer_right_offset) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "scaled_ref_layer_bottom_offset {");
    for (const int32_t& v : scaled_ref_layer_bottom_offset) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ref_region_offset_present_flag {");
    for (const uint32_t& v : ref_region_offset_present_flag) {
      fprintf(outfp, " %u", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ref_region_left_offset {");
    for (const int32_t& v : ref_region_left_offset) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ref_region_top_offset {");
    for (const int32_t& v : ref_region_top_offset) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ref_region_right_offset {");
    for (const int32_t& v : ref_region_right_offset) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ref_region_bottom_offset {");
    for (const int32_t& v : ref_region_bottom_offset) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "resample_phase_set_present_flag {");
    for (const int32_t& v : resample_phase_set_present_flag) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "phase_hor_luma {");
    for (const int32_t& v : phase_hor_luma) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "phase_ver_luma {");
    for (const int32_t& v : phase_ver_luma) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "phase_hor_chroma_plus8 {");
    for (const int32_t& v : phase_hor_chroma_plus8) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "phase_ver_chroma_plus8 {");
    for (const int32_t& v : phase_ver_chroma_plus8) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "colour_mapping_enabled_flag: %i",
          colour_mapping_enabled_flag);

  if (colour_mapping_enabled_flag) {
    // colour_mapping_table() // specified in Annex F
    // TODO(chemag): add support for colour_mapping_table()
    fprintf(stderr, "error: unimplemented colour_mapping_table() in pps\n");
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
