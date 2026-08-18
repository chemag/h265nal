/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_pps_range_extension_parser.h"

#include <stdio.h>

#include <cinttypes>
#include <cstdint>
#include <memory>
#include <vector>

#include "h265_common.h"

namespace h265nal {

// General note: this is based off the 2016/12 version of the H.265 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.265

// Unpack RBSP and parse pps_range_extension state from the supplied buffer.
std::unique_ptr<H265PpsRangeExtensionParser::PpsRangeExtensionState>
H265PpsRangeExtensionParser::ParsePpsRangeExtension(
    const uint8_t* data, size_t length,
    uint32_t transform_skip_enabled_flag) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParsePpsRangeExtension(&bit_buffer, transform_skip_enabled_flag);
}

std::unique_ptr<H265PpsRangeExtensionParser::PpsRangeExtensionState>
H265PpsRangeExtensionParser::ParsePpsRangeExtension(
    BitBuffer* bit_buffer, uint32_t transform_skip_enabled_flag) noexcept {
  int32_t sgolomb_tmp;

  // H265 pps_range_extension() NAL Unit.
  // Section 7.3.2.3.2 ("Picture parameter set range extension syntax") of
  // the H.265 standard for a complete description.
  auto pps_range_extension = std::make_unique<PpsRangeExtensionState>();

  // input parameters
  pps_range_extension->transform_skip_enabled_flag =
      transform_skip_enabled_flag;

  if (pps_range_extension->transform_skip_enabled_flag) {
    // log2_max_transform_skip_block_size_minus2  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            pps_range_extension->log2_max_transform_skip_block_size_minus2)) {
      return nullptr;
    }
  }

  // cross_component_prediction_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(
          1, pps_range_extension->cross_component_prediction_enabled_flag)) {
    return nullptr;
  }

  // chroma_qp_offset_list_enabled_flag  u(1)
  if (!bit_buffer->ReadBits(
          1, pps_range_extension->chroma_qp_offset_list_enabled_flag)) {
    return nullptr;
  }

  if (pps_range_extension->chroma_qp_offset_list_enabled_flag) {
    // diff_cu_chroma_qp_offset_depth  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            pps_range_extension->diff_cu_chroma_qp_offset_depth)) {
      return nullptr;
    }
    if (pps_range_extension->diff_cu_chroma_qp_offset_depth <
            kDiffCuChromaQpOffsetDepthMin ||
        pps_range_extension->diff_cu_chroma_qp_offset_depth >
            kDiffCuChromaQpOffsetDepthMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid diff_cu_chroma_qp_offset_depth: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              pps_range_extension->diff_cu_chroma_qp_offset_depth,
              kDiffCuChromaQpOffsetDepthMin, kDiffCuChromaQpOffsetDepthMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // chroma_qp_offset_list_len_minus1  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            pps_range_extension->chroma_qp_offset_list_len_minus1)) {
      return nullptr;
    }
    if (pps_range_extension->chroma_qp_offset_list_len_minus1 <
            kChromaQpOffsetListLenMinus1Min ||
        pps_range_extension->chroma_qp_offset_list_len_minus1 >
            kChromaQpOffsetListLenMinus1Max) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid chroma_qp_offset_list_len_minus1: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              pps_range_extension->chroma_qp_offset_list_len_minus1,
              kChromaQpOffsetListLenMinus1Min,
              kChromaQpOffsetListLenMinus1Max);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    for (uint32_t i = 0;
         i <= pps_range_extension->chroma_qp_offset_list_len_minus1; i++) {
      // cb_qp_offset_list[i]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      if (sgolomb_tmp < kQpOffsetListMin || sgolomb_tmp > kQpOffsetListMax) {
#ifdef FPRINT_ERRORS
        fprintf(stderr,
                "invalid cb_qp_offset_list[%" PRIu32 "]: %" PRId32
                " not in range "
                "[%" PRId32 ", %" PRId32 "]\n",
                i, sgolomb_tmp, kQpOffsetListMin, kQpOffsetListMax);
#endif  // FPRINT_ERRORS
        return nullptr;
      }
      pps_range_extension->cb_qp_offset_list.push_back(sgolomb_tmp);

      // cr_qp_offset_list[i]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(sgolomb_tmp)) {
        return nullptr;
      }
      if (sgolomb_tmp < kQpOffsetListMin || sgolomb_tmp > kQpOffsetListMax) {
#ifdef FPRINT_ERRORS
        fprintf(stderr,
                "invalid cr_qp_offset_list[%" PRIu32 "]: %" PRId32
                " not in range "
                "[%" PRId32 ", %" PRId32 "]\n",
                i, sgolomb_tmp, kQpOffsetListMin, kQpOffsetListMax);
#endif  // FPRINT_ERRORS
        return nullptr;
      }
      pps_range_extension->cr_qp_offset_list.push_back(sgolomb_tmp);
    }
  }

  // log2_sao_offset_scale_luma  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(
          pps_range_extension->log2_sao_offset_scale_luma)) {
    return nullptr;
  }
  if (pps_range_extension->log2_sao_offset_scale_luma <
          kLog2SaoOffsetScaleMin ||
      pps_range_extension->log2_sao_offset_scale_luma >
          kLog2SaoOffsetScaleMax) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "invalid log2_sao_offset_scale_luma: %" PRIu32
            " not in range "
            "[%" PRIu32 ", %" PRIu32 "]\n",
            pps_range_extension->log2_sao_offset_scale_luma,
            kLog2SaoOffsetScaleMin, kLog2SaoOffsetScaleMax);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // log2_sao_offset_scale_chroma  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(
          pps_range_extension->log2_sao_offset_scale_chroma)) {
    return nullptr;
  }
  if (pps_range_extension->log2_sao_offset_scale_chroma <
          kLog2SaoOffsetScaleMin ||
      pps_range_extension->log2_sao_offset_scale_chroma >
          kLog2SaoOffsetScaleMax) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "invalid log2_sao_offset_scale_chroma: %" PRIu32
            " not in range "
            "[%" PRIu32 ", %" PRIu32 "]\n",
            pps_range_extension->log2_sao_offset_scale_chroma,
            kLog2SaoOffsetScaleMin, kLog2SaoOffsetScaleMax);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  return pps_range_extension;
}

#ifdef FDUMP_DEFINE
void H265PpsRangeExtensionParser::PpsRangeExtensionState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "pps_range_extension {");
  indent_level = indent_level_incr(indent_level);

  if (transform_skip_enabled_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "log2_max_transform_skip_block_size_minus2: %i",
            log2_max_transform_skip_block_size_minus2);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "cross_component_prediction_enabled_flag: %i",
          cross_component_prediction_enabled_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "chroma_qp_offset_list_enabled_flag: %i",
          chroma_qp_offset_list_enabled_flag);

  if (chroma_qp_offset_list_enabled_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "diff_cu_chroma_qp_offset_depth: %i",
            diff_cu_chroma_qp_offset_depth);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "chroma_qp_offset_list_len_minus1: %i",
            chroma_qp_offset_list_len_minus1);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "cb_qp_offset_list {");
    for (const int32_t& v : cb_qp_offset_list) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "cr_qp_offset_list {");
    for (const int32_t& v : cr_qp_offset_list) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "log2_sao_offset_scale_luma: %i", log2_sao_offset_scale_luma);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "log2_sao_offset_scale_chroma: %i",
          log2_sao_offset_scale_chroma);

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h265nal
