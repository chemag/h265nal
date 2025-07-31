/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_vui_parameters_parser.h"


#include <cinttypes>
#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h265_common.h"
#include "h265_hrd_parameters_parser.h"

namespace h265nal {

// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Unpack RBSP and parse VUI Parameters state from the supplied buffer.
std::unique_ptr<H265VuiParametersParser::VuiParametersState>
H265VuiParametersParser::ParseVuiParameters(
    const uint8_t* data, size_t length,
    uint32_t sps_max_sub_layers_minus1) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseVuiParameters(&bit_buffer, sps_max_sub_layers_minus1);
}

std::unique_ptr<H265VuiParametersParser::VuiParametersState>
H265VuiParametersParser::ParseVuiParameters(
    BitBuffer* bit_buffer, uint32_t sps_max_sub_layers_minus1) noexcept {
  // H265 vui_parameters() parser.
  // Section E.2.1 ("VUI parameters syntax") of the H.265 standard for
  // a complete description.
  auto vui = std::make_unique<VuiParametersState>();

  // input
  vui->sps_max_sub_layers_minus1 = sps_max_sub_layers_minus1;

  // aspect_ratio_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->aspect_ratio_info_present_flag)) {
    return nullptr;
  }

  if (vui->aspect_ratio_info_present_flag) {
    // aspect_ratio_idc  u(8)
    if (!bit_buffer->ReadBits(8, vui->aspect_ratio_idc)) {
      return nullptr;
    }
    if (vui->aspect_ratio_idc == AR_EXTENDED_SAR) {
      // sar_width  u(16)
      if (!bit_buffer->ReadBits(16, vui->sar_width)) {
        return nullptr;
      }
      // sar_height  u(16)
      if (!bit_buffer->ReadBits(16, vui->sar_height)) {
        return nullptr;
      }
    }
  }

  // overscan_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->overscan_info_present_flag)) {
    return nullptr;
  }

  if (vui->overscan_info_present_flag) {
    // overscan_appropriate_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->overscan_appropriate_flag)) {
      return nullptr;
    }
  }

  // video_signal_type_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->video_signal_type_present_flag)) {
    return nullptr;
  }

  if (vui->video_signal_type_present_flag) {
    // video_format  u(3)
    if (!bit_buffer->ReadBits(3, vui->video_format)) {
      return nullptr;
    }
    // video_full_range_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->video_full_range_flag)) {
      return nullptr;
    }
    // colour_description_present_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->colour_description_present_flag)) {
      return nullptr;
    }
    if (vui->colour_description_present_flag) {
      // colour_primaries  u(8)
      if (!bit_buffer->ReadBits(8, vui->colour_primaries)) {
        return nullptr;
      }
      // transfer_characteristics  u(8)
      if (!bit_buffer->ReadBits(8, vui->transfer_characteristics)) {
        return nullptr;
      }
      // matrix_coeffs  u(8)
      if (!bit_buffer->ReadBits(8, vui->matrix_coeffs)) {
        return nullptr;
      }
    }
  }

  // chroma_loc_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->chroma_loc_info_present_flag)) {
    return nullptr;
  }
  if (vui->chroma_loc_info_present_flag) {
    // chroma_sample_loc_type_top_field  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            vui->chroma_sample_loc_type_top_field)) {
      return nullptr;
    }
    if (vui->chroma_sample_loc_type_top_field <
            kChromaSampleLocTypeTopFieldMin ||
        vui->chroma_sample_loc_type_top_field >
            kChromaSampleLocTypeTopFieldMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid chroma_sample_loc_type_top_field: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->chroma_sample_loc_type_top_field,
              kChromaSampleLocTypeTopFieldMin, kChromaSampleLocTypeTopFieldMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // chroma_sample_loc_type_bottom_field  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            vui->chroma_sample_loc_type_bottom_field)) {
      return nullptr;
    }
    if (vui->chroma_sample_loc_type_bottom_field <
            kChromaSampleLocTypeBottomFieldMin ||
        vui->chroma_sample_loc_type_bottom_field >
            kChromaSampleLocTypeBottomFieldMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid chroma_sample_loc_type_bottom_field: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->chroma_sample_loc_type_bottom_field,
              kChromaSampleLocTypeBottomFieldMin,
              kChromaSampleLocTypeBottomFieldMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }
  }

  // neutral_chroma_indication_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->neutral_chroma_indication_flag)) {
    return nullptr;
  }

  // field_seq_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->field_seq_flag)) {
    return nullptr;
  }

  // frame_field_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->frame_field_info_present_flag)) {
    return nullptr;
  }

  // default_display_window_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->default_display_window_flag)) {
    return nullptr;
  }
  if (vui->default_display_window_flag) {
    // def_disp_win_left_offset ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->def_disp_win_left_offset)) {
      return nullptr;
    }
    if (vui->def_disp_win_left_offset < kDefDispWinLeftOffsetMin ||
        vui->def_disp_win_left_offset > kDefDispWinLeftOffsetMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid def_disp_win_left_offset: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->def_disp_win_left_offset, kDefDispWinLeftOffsetMin,
              kDefDispWinLeftOffsetMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // def_disp_win_right_offset  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->def_disp_win_right_offset)) {
      return nullptr;
    }
    if (vui->def_disp_win_right_offset < kDefDispWinRightOffsetMin ||
        vui->def_disp_win_right_offset > kDefDispWinRightOffsetMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid def_disp_win_right_offset: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->def_disp_win_right_offset, kDefDispWinRightOffsetMin,
              kDefDispWinRightOffsetMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // def_disp_win_top_offset  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->def_disp_win_top_offset)) {
      return nullptr;
    }
    if (vui->def_disp_win_top_offset < kDefDispWinTopOffsetMin ||
        vui->def_disp_win_top_offset > kDefDispWinTopOffsetMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid def_disp_win_top_offset: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->def_disp_win_top_offset, kDefDispWinTopOffsetMin,
              kDefDispWinTopOffsetMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // def_disp_win_bottom_offset  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->def_disp_win_bottom_offset)) {
      return nullptr;
    }
    if (vui->def_disp_win_bottom_offset < kDefDispWinBottomOffsetMin ||
        vui->def_disp_win_bottom_offset > kDefDispWinBottomOffsetMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid def_disp_win_bottom_offset: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->def_disp_win_bottom_offset, kDefDispWinBottomOffsetMin,
              kDefDispWinBottomOffsetMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }
  }

  // vui_timing_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->vui_timing_info_present_flag)) {
    return nullptr;
  }
  if (vui->vui_timing_info_present_flag) {
    // vui_num_units_in_tick  u(32)
    if (!bit_buffer->ReadBits(32, vui->vui_num_units_in_tick)) {
      return nullptr;
    }
    // vui_time_scale  u(32)
    if (!bit_buffer->ReadBits(32, vui->vui_time_scale)) {
      return nullptr;
    }
    // vui_poc_proportional_to_timing_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->vui_poc_proportional_to_timing_flag)) {
      return nullptr;
    }
    if (vui->vui_poc_proportional_to_timing_flag) {
      // vui_num_ticks_poc_diff_one_minus1  ue(v)
      if (!bit_buffer->ReadExponentialGolomb(
              vui->vui_num_ticks_poc_diff_one_minus1)) {
        return nullptr;
      }
      if (vui->vui_num_ticks_poc_diff_one_minus1 <
              kVuiNumTicksPocDiffOneMinus1Min ||
          vui->vui_num_ticks_poc_diff_one_minus1 >
              kVuiNumTicksPocDiffOneMinus1Max) {
#ifdef FPRINT_ERRORS
        fprintf(stderr,
                "invalid vui_num_ticks_poc_diff_one_minus1: %" PRIu32
                " not in range "
                "[%" PRIu32 ", %" PRIu32 "]\n",
                vui->vui_num_ticks_poc_diff_one_minus1,
                kVuiNumTicksPocDiffOneMinus1Min,
                kVuiNumTicksPocDiffOneMinus1Max);
#endif  // FPRINT_ERRORS
        return nullptr;
      }
    }
    // vui_hrd_parameters_present_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->vui_hrd_parameters_present_flag)) {
      return nullptr;
    }
    if (vui->vui_hrd_parameters_present_flag) {
      // hrd_parameters(1, sps_max_sub_layers_minus1)
      vui->hrd_parameters = H265HrdParametersParser::ParseHrdParameters(
          bit_buffer, 1, vui->sps_max_sub_layers_minus1);
      if (vui->hrd_parameters == nullptr) {
        return nullptr;
      }
    }
  }

  // bitstream_restriction_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->bitstream_restriction_flag)) {
    return nullptr;
  }
  if (vui->bitstream_restriction_flag) {
    // tiles_fixed_structure_flag u(1)
    if (!bit_buffer->ReadBits(1, vui->tiles_fixed_structure_flag)) {
      return nullptr;
    }
    // motion_vectors_over_pic_boundaries_flag  u(1)
    if (!bit_buffer->ReadBits(1,
                              vui->motion_vectors_over_pic_boundaries_flag)) {
      return nullptr;
    }
    // restricted_ref_pic_lists_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->restricted_ref_pic_lists_flag)) {
      return nullptr;
    }
    // min_spatial_segmentation_idc  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->min_spatial_segmentation_idc)) {
      return nullptr;
    }
    if (vui->min_spatial_segmentation_idc < kMinSpatialSegmentationIdcMin ||
        vui->min_spatial_segmentation_idc > kMinSpatialSegmentationIdcMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid min_spatial_segmentation_idc: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->min_spatial_segmentation_idc, kMinSpatialSegmentationIdcMin,
              kMinSpatialSegmentationIdcMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }
    // max_bytes_per_pic_denom  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->max_bytes_per_pic_denom)) {
      return nullptr;
    }
    if (vui->max_bytes_per_pic_denom < kMaxBytesPerPicDenomMin ||
        vui->max_bytes_per_pic_denom > kMaxBytesPerPicDenomMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid max_bytes_per_pic_denom: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->max_bytes_per_pic_denom, kMaxBytesPerPicDenomMin,
              kMaxBytesPerPicDenomMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }
    // max_bits_per_min_cu_denom  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->max_bits_per_min_cu_denom)) {
      return nullptr;
    }
    if (vui->max_bits_per_min_cu_denom < kMaxBitsPerMinCuDenomMin ||
        vui->max_bits_per_min_cu_denom > kMaxBitsPerMinCuDenomMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid max_bits_per_min_cu_denom: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->max_bits_per_min_cu_denom, kMaxBitsPerMinCuDenomMin,
              kMaxBitsPerMinCuDenomMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }
    // log2_max_mv_length_horizontal  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            vui->log2_max_mv_length_horizontal)) {
      return nullptr;
    }
    if (vui->log2_max_mv_length_horizontal < kLog2MaxMvLengthHorizontalMin ||
        vui->log2_max_mv_length_horizontal > kLog2MaxMvLengthHorizontalMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid log2_max_mv_length_horizontal: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->log2_max_mv_length_horizontal, kLog2MaxMvLengthHorizontalMin,
              kLog2MaxMvLengthHorizontalMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }
    // log2_max_mv_length_vertical  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->log2_max_mv_length_vertical)) {
      return nullptr;
    }
    if (vui->log2_max_mv_length_vertical < kLog2MaxMvLengthVerticalMin ||
        vui->log2_max_mv_length_vertical > kLog2MaxMvLengthVerticalMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid log2_max_mv_length_vertical: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->log2_max_mv_length_vertical, kLog2MaxMvLengthVerticalMin,
              kLog2MaxMvLengthVerticalMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }
  }

  return vui;
}

float H265VuiParametersParser::VuiParametersState::getFramerate()
    const noexcept {
  // Equation D-2
  float framerate = (float)vui_time_scale / (float)vui_num_units_in_tick;
  return framerate;
}

#ifdef FDUMP_DEFINE
void H265VuiParametersParser::VuiParametersState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "vui_parameters {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "aspect_ratio_info_present_flag: %i",
          aspect_ratio_info_present_flag);

  if (aspect_ratio_info_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "aspect_ratio_idc: %i", aspect_ratio_idc);

    if (aspect_ratio_idc == AR_EXTENDED_SAR) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "sar_width: %i", sar_width);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "sar_height: %i", sar_height);
    }
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "overscan_info_present_flag: %i", overscan_info_present_flag);

  if (overscan_info_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "overscan_appropriate_flag: %i", overscan_appropriate_flag);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "video_signal_type_present_flag: %i",
          video_signal_type_present_flag);

  if (video_signal_type_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "video_format: %i", video_format);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "video_full_range_flag: %i", video_full_range_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "colour_description_present_flag: %i",
            colour_description_present_flag);

    if (colour_description_present_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "colour_primaries: %i", colour_primaries);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "transfer_characteristics: %i", transfer_characteristics);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "matrix_coeffs: %i", matrix_coeffs);
    }
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "chroma_loc_info_present_flag: %i",
          chroma_loc_info_present_flag);

  if (chroma_loc_info_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "chroma_sample_loc_type_top_field: %i",
            chroma_sample_loc_type_top_field);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "chroma_sample_loc_type_bottom_field: %i",
            chroma_sample_loc_type_bottom_field);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "neutral_chroma_indication_flag: %i",
          neutral_chroma_indication_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "field_seq_flag: %i", field_seq_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "frame_field_info_present_flag: %i",
          frame_field_info_present_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "default_display_window_flag: %i",
          default_display_window_flag);

  if (default_display_window_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "def_disp_win_left_offset: %i", def_disp_win_left_offset);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "def_disp_win_right_offset: %i", def_disp_win_right_offset);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "def_disp_win_top_offset: %i", def_disp_win_top_offset);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "def_disp_win_bottom_offset: %i",
            def_disp_win_bottom_offset);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "vui_timing_info_present_flag: %i",
          vui_timing_info_present_flag);

  if (vui_timing_info_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "vui_num_units_in_tick: %i", vui_num_units_in_tick);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "vui_time_scale: %i", vui_time_scale);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "vui_poc_proportional_to_timing_flag: %i",
            vui_poc_proportional_to_timing_flag);

    if (vui_poc_proportional_to_timing_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "vui_num_ticks_poc_diff_one_minus1: %i",
              vui_num_ticks_poc_diff_one_minus1);
    }

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "vui_hrd_parameters_present_flag: %i",
            vui_hrd_parameters_present_flag);

    if (vui_hrd_parameters_present_flag) {
      // hrd_parameters(1, sps_max_sub_layers_minus1)
      fdump_indent_level(outfp, indent_level);
      hrd_parameters->fdump(outfp, indent_level);
    }
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "bitstream_restriction_flag: %i", bitstream_restriction_flag);

  if (bitstream_restriction_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "tiles_fixed_structure_flag: %i",
            tiles_fixed_structure_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "motion_vectors_over_pic_boundaries_flag: %i",
            motion_vectors_over_pic_boundaries_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "restricted_ref_pic_lists_flag: %i",
            restricted_ref_pic_lists_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "min_spatial_segmentation_idc: %i",
            min_spatial_segmentation_idc);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "max_bytes_per_pic_denom: %i", max_bytes_per_pic_denom);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "max_bits_per_min_cu_denom: %i", max_bits_per_min_cu_denom);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "log2_max_mv_length_horizontal: %i",
            log2_max_mv_length_horizontal);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "log2_max_mv_length_vertical: %i",
            log2_max_mv_length_vertical);
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
