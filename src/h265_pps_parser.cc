/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_pps_parser.h"

#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h265_common.h"
#include "h265_pps_multilayer_extension_parser.h"
#include "h265_pps_scc_extension_parser.h"
#include "h265_profile_tier_level_parser.h"
#include "h265_scaling_list_data_parser.h"

namespace h265nal {

// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Unpack RBSP and parse PPS state from the supplied buffer.
std::shared_ptr<H265PpsParser::PpsState> H265PpsParser::ParsePps(
    const uint8_t* data, size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParsePps(&bit_buffer);
}

std::shared_ptr<H265PpsParser::PpsState> H265PpsParser::ParsePps(
    BitBuffer* bit_buffer) noexcept {
  uint32_t golomb_tmp;

  // H265 PPS NAL Unit (pic_parameter_set_rbsp()) parser.
  // Section 7.3.2.3 ("Picture parameter set data syntax") of the H.265
  // standard for a complete description.
  auto pps = std::make_shared<PpsState>();

  // pps_pic_parameter_set_id  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(pps->pps_pic_parameter_set_id)) {
    return nullptr;
  }

  // pps_seq_parameter_set_id  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(pps->pps_seq_parameter_set_id)) {
    return nullptr;
  }

  // dependent_slice_segments_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->dependent_slice_segments_enabled_flag)) {
    return nullptr;
  }

  // output_flag_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->output_flag_present_flag)) {
    return nullptr;
  }

  // num_extra_slice_header_bits  u(3)
  if (!bit_buffer->ReadBits(3, pps->num_extra_slice_header_bits)) {
    return nullptr;
  }

  // sign_data_hiding_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->sign_data_hiding_enabled_flag)) {
    return nullptr;
  }

  // cabac_init_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->cabac_init_present_flag)) {
    return nullptr;
  }

  // num_ref_idx_l0_default_active_minus1  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(
          pps->num_ref_idx_l0_default_active_minus1)) {
    return nullptr;
  }

  // num_ref_idx_l1_default_active_minus1  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(
          pps->num_ref_idx_l1_default_active_minus1)) {
    return nullptr;
  }

  // init_qp_minus26  se(v)
  if (!bit_buffer->ReadSignedExponentialGolomb(pps->init_qp_minus26)) {
    return nullptr;
  }

  // constrained_intra_pred_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->constrained_intra_pred_flag)) {
    return nullptr;
  }

  // transform_skip_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->transform_skip_enabled_flag)) {
    return nullptr;
  }

  // cu_qp_delta_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->cu_qp_delta_enabled_flag)) {
    return nullptr;
  }

  if (pps->cu_qp_delta_enabled_flag) {
    // diff_cu_qp_delta_depth  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(pps->diff_cu_qp_delta_depth)) {
      return nullptr;
    }
  }

  // pps_cb_qp_offset  se(v)
  if (!bit_buffer->ReadSignedExponentialGolomb(pps->pps_cb_qp_offset)) {
    return nullptr;
  }

  // pps_cr_qp_offset  se(v)
  if (!bit_buffer->ReadSignedExponentialGolomb(pps->pps_cr_qp_offset)) {
    return nullptr;
  }

  // pps_slice_chroma_qp_offsets_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->pps_slice_chroma_qp_offsets_present_flag)) {
    return nullptr;
  }

  // weighted_pred_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->weighted_pred_flag)) {
    return nullptr;
  }

  // weighted_bipred_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->weighted_bipred_flag)) {
    return nullptr;
  }

  // transquant_bypass_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->transquant_bypass_enabled_flag)) {
    return nullptr;
  }

  // tiles_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->tiles_enabled_flag)) {
    return nullptr;
  }

  // entropy_coding_sync_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->entropy_coding_sync_enabled_flag)) {
    return nullptr;
  }

  if (pps->tiles_enabled_flag) {
    // num_tile_columns_minus1  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(pps->num_tile_columns_minus1)) {
      return nullptr;
    }
    // num_tile_rows_minus1  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(pps->num_tile_rows_minus1)) {
      return nullptr;
    }
    // uniform_spacing_flag  u(1)
    if (!bit_buffer->ReadBits(1, pps->uniform_spacing_flag)) {
      return nullptr;
    }

    if (!pps->uniform_spacing_flag) {
      for (uint32_t i = 0; i < pps->num_tile_columns_minus1; i++) {
        // column_width_minus1[i]  ue(v)
        if (!bit_buffer->ReadExponentialGolomb(golomb_tmp)) {
          return nullptr;
        }
        pps->column_width_minus1.push_back(golomb_tmp);
      }
      for (uint32_t i = 0; i < pps->num_tile_rows_minus1; i++) {
        // row_height_minus1[i]  ue(v)
        if (!bit_buffer->ReadExponentialGolomb(golomb_tmp)) {
          return nullptr;
        }
        pps->row_height_minus1.push_back(golomb_tmp);
      }
    }

    // loop_filter_across_tiles_enabled_flag u(1)
    if (!bit_buffer->ReadBits(1, pps->loop_filter_across_tiles_enabled_flag)) {
      return nullptr;
    }
  }

  // pps_loop_filter_across_slices_enabled_flag u(1)
  if (!bit_buffer->ReadBits(1,
                            pps->pps_loop_filter_across_slices_enabled_flag)) {
    return nullptr;
  }

  // deblocking_filter_control_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->deblocking_filter_control_present_flag)) {
    return nullptr;
  }

  if (pps->deblocking_filter_control_present_flag) {
    // deblocking_filter_override_enabled_flag u(1)
    if (!bit_buffer->ReadBits(1,
                              pps->deblocking_filter_override_enabled_flag)) {
      return nullptr;
    }

    // pps_deblocking_filter_disabled_flag  u(1)
    if (!bit_buffer->ReadBits(1, pps->pps_deblocking_filter_disabled_flag)) {
      return nullptr;
    }

    if (!pps->pps_deblocking_filter_disabled_flag) {
      // pps_beta_offset_div2  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(pps->pps_beta_offset_div2)) {
        return nullptr;
      }

      // pps_tc_offset_div2  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(pps->pps_tc_offset_div2)) {
        return nullptr;
      }
    }
  }

  // pps_scaling_list_data_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->pps_scaling_list_data_present_flag)) {
    return nullptr;
  }

  if (pps->pps_scaling_list_data_present_flag) {
    // scaling_list_data()
    pps->scaling_list_data =
        H265ScalingListDataParser::ParseScalingListData(bit_buffer);
    if (pps->scaling_list_data == nullptr) {
      return nullptr;
    }
  }

  // lists_modification_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->lists_modification_present_flag)) {
    return nullptr;
  }

  // log2_parallel_merge_level_minus2  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(
          pps->log2_parallel_merge_level_minus2)) {
    return nullptr;
  }

  // slice_segment_header_extension_present_flag  u(1)
  if (!bit_buffer->ReadBits(1,
                            pps->slice_segment_header_extension_present_flag)) {
    return nullptr;
  }

  // pps_extension_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, pps->pps_extension_present_flag)) {
    return nullptr;
  }

  if (pps->pps_extension_present_flag) {
    // pps_range_extension_flag  u(1)
    if (!bit_buffer->ReadBits(1, pps->pps_range_extension_flag)) {
      return nullptr;
    }

    // pps_multilayer_extension_flag  u(1)
    if (!bit_buffer->ReadBits(1, pps->pps_multilayer_extension_flag)) {
      return nullptr;
    }

    // pps_3d_extension_flag  u(1)
    if (!bit_buffer->ReadBits(1, pps->pps_3d_extension_flag)) {
      return nullptr;
    }

    // pps_scc_extension_flag  u(1)
    if (!bit_buffer->ReadBits(1, pps->pps_scc_extension_flag)) {
      return nullptr;
    }

    // pps_extension_4bits  u(4)
    if (!bit_buffer->ReadBits(4, pps->pps_extension_4bits)) {
      return nullptr;
    }
  }

  if (pps->pps_range_extension_flag) {
    // pps_range_extension()
    // TODO(chemag): add support for pps_range_extension()
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: unimplemented pps_range_extension() in pps\n");
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  if (pps->pps_multilayer_extension_flag) {
    // pps_multilayer_extension() // specified in Annex F
    pps->pps_multilayer_extension =
        H265PpsMultilayerExtensionParser::ParsePpsMultilayerExtension(
            bit_buffer);
  }

  if (pps->pps_3d_extension_flag) {
    // pps_3d_extension() // specified in Annex I
    // TODO(chemag): add support for pps_3d_extension()
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: unimplemented pps_3d_extension() in pps\n");
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  if (pps->pps_scc_extension_flag) {
    // pps_range_extension()
    pps->pps_scc_extension =
        H265PpsSccExtensionParser::ParsePpsSccExtension(bit_buffer);
    if (pps->pps_scc_extension == nullptr) {
      return nullptr;
    }
  }

  if (pps->pps_extension_4bits) {
    while (more_rbsp_data(bit_buffer)) {
      // pps_extension_data_flag  u(1)
      if (!bit_buffer->ReadBits(1, pps->pps_extension_data_flag)) {
        return nullptr;
      }
    }
  }

  rbsp_trailing_bits(bit_buffer);

  return pps;
}

#ifdef FDUMP_DEFINE
void H265PpsParser::PpsState::fdump(FILE* outfp, int indent_level) const {
  fprintf(outfp, "pps {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pps_pic_parameter_set_id: %i", pps_pic_parameter_set_id);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pps_seq_parameter_set_id: %i", pps_seq_parameter_set_id);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "dependent_slice_segments_enabled_flag: %i",
          dependent_slice_segments_enabled_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "output_flag_present_flag: %i", output_flag_present_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "num_extra_slice_header_bits: %i",
          num_extra_slice_header_bits);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "sign_data_hiding_enabled_flag: %i",
          sign_data_hiding_enabled_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "cabac_init_present_flag: %i", cabac_init_present_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "num_ref_idx_l0_default_active_minus1: %i",
          num_ref_idx_l0_default_active_minus1);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "num_ref_idx_l1_default_active_minus1: %i",
          num_ref_idx_l1_default_active_minus1);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "init_qp_minus26: %i", init_qp_minus26);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "constrained_intra_pred_flag: %i",
          constrained_intra_pred_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "transform_skip_enabled_flag: %i",
          transform_skip_enabled_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "cu_qp_delta_enabled_flag: %i", cu_qp_delta_enabled_flag);

  if (cu_qp_delta_enabled_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "cu_qp_delta_enabled_flag: %i", cu_qp_delta_enabled_flag);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pps_cb_qp_offset: %i", pps_cb_qp_offset);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pps_cr_qp_offset: %i", pps_cr_qp_offset);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pps_slice_chroma_qp_offsets_present_flag: %i",
          pps_slice_chroma_qp_offsets_present_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "weighted_pred_flag: %i", weighted_pred_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "weighted_bipred_flag: %i", weighted_bipred_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "transquant_bypass_enabled_flag: %i",
          transquant_bypass_enabled_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "tiles_enabled_flag: %i", tiles_enabled_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "entropy_coding_sync_enabled_flag: %i",
          entropy_coding_sync_enabled_flag);

  if (tiles_enabled_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "num_tile_columns_minus1: %i", num_tile_columns_minus1);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "num_tile_rows_minus1: %i", num_tile_rows_minus1);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "uniform_spacing_flag: %i", uniform_spacing_flag);

    if (!uniform_spacing_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "column_width_minus1 {");
      for (const uint32_t& v : column_width_minus1) {
        fprintf(outfp, " %i", v);
      }
      fprintf(outfp, " }");

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "row_height_minus1 {");
      for (const uint32_t& v : row_height_minus1) {
        fprintf(outfp, " %i", v);
      }
      fprintf(outfp, " }");
    }

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "loop_filter_across_tiles_enabled_flag: %i",
            loop_filter_across_tiles_enabled_flag);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pps_loop_filter_across_slices_enabled_flag: %i",
          pps_loop_filter_across_slices_enabled_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "deblocking_filter_control_present_flag: %i",
          deblocking_filter_control_present_flag);

  if (deblocking_filter_control_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "deblocking_filter_override_enabled_flag: %i",
            deblocking_filter_override_enabled_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "pps_deblocking_filter_disabled_flag: %i",
            pps_deblocking_filter_disabled_flag);

    if (!pps_deblocking_filter_disabled_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "pps_beta_offset_div2: %i", pps_beta_offset_div2);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "pps_tc_offset_div2: %i", pps_tc_offset_div2);
    }
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pps_scaling_list_data_present_flag: %i",
          pps_scaling_list_data_present_flag);

  if (pps_scaling_list_data_present_flag && scaling_list_data) {
    fdump_indent_level(outfp, indent_level);
    scaling_list_data->fdump(outfp, indent_level);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "lists_modification_present_flag: %i",
          lists_modification_present_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "log2_parallel_merge_level_minus2: %i",
          log2_parallel_merge_level_minus2);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "slice_segment_header_extension_present_flag: %i",
          slice_segment_header_extension_present_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pps_extension_present_flag: %i", pps_extension_present_flag);

  if (pps_extension_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "pps_range_extension_flag: %i", pps_range_extension_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "pps_multilayer_extension_flag: %i",
            pps_multilayer_extension_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "pps_3d_extension_flag: %i", pps_3d_extension_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "pps_scc_extension_flag: %i", pps_scc_extension_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "pps_extension_4bits: %i", pps_extension_4bits);
  }

  if (pps_range_extension_flag) {
    // pps_range_extension()
    // TODO(chemag): add support for pps_range_extension()
    fprintf(stderr, "error: unimplemented pps_range_extension() in pps\n");
  }

  if (pps_multilayer_extension_flag) {
    fdump_indent_level(outfp, indent_level);
    pps_multilayer_extension->fdump(outfp, indent_level);
  }

  if (pps_3d_extension_flag) {
    // pps_3d_extension() // specified in Annex I
    // TODO(chemag): add support for pps_3d_extension()
    fprintf(stderr, "error: unimplemented pps_3d_extension_flag() in pps\n");
  }

  if (pps_scc_extension_flag) {
    // pps_scc_extension()
    fdump_indent_level(outfp, indent_level);
    pps_scc_extension->fdump(outfp, indent_level);
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
