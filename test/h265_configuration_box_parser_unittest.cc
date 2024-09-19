/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_configuration_box_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h265_common.h"
#include "rtc_common.h"

namespace h265nal {

class H265ConfigurationBoxParserTest : public ::testing::Test {
 public:
  H265ConfigurationBoxParserTest() {}
  ~H265ConfigurationBoxParserTest() override {}
};

TEST_F(H265ConfigurationBoxParserTest, TestSampleConfigurationBox) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      // offset: 0
      0x01, 0x01, 0x60, 0x00, 0x00, 0x00, 0x80, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x78, 0xf0, 0x00, 0xfc,
      0xfd, 0xf8, 0xf8, 0x00, 0x00, 0x0f,
      // offset: 22
      // unsigned int(8) numOfArrays;
      0x03,
      // offset: 23
      // for (j=0; j < numOfArrays; j++) {
      // bit(1) array_completeness;
      // unsigned int(1) reserved = 0;
      // unsigned int(6) NAL_unit_type; // 32 (VPS)
      0xa0,
      // unsigned int(16) numNalus;
      0x00, 0x01,
      // for (i=0; i< numNalus; i++) {
      // unsigned int(16) nalUnitLength;
      0x00, 0x18,
      // offset: 28
      // VPS-start
      0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60,
      0x00, 0x00, 0x03, 0x00, 0x80, 0x00, 0x00, 0x03,
      0x00, 0x00, 0x03, 0x00, 0x78, 0x9d, 0xc0, 0x90,
      // VPS-end
      // offset: 52
      // for (j=0; j < numOfArrays; j++) {
      // bit(1) array_completeness;
      // unsigned int(1) reserved = 0;
      // unsigned int(6) NAL_unit_type; // 33 (SPS)
      0xa1,
      // unsigned int(16) numNalus;
      0x00, 0x01,
      // for (i=0; i< numNalus; i++) {
      // unsigned int(16) nalUnitLength;
      0x00, 0x27,
      // offset: 57
      // SPS-start
      0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03,
      0x00, 0x80, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03,
      0x00, 0x78, 0xa0, 0x03, 0xc0, 0x80, 0x32, 0x16,
      0x59, 0xde, 0x49, 0x1b, 0x6b, 0x80, 0x40, 0x00,
      0x00, 0xfa, 0x00, 0x00, 0x17, 0x70, 0x02,
      // SPS-end
      // for (j=0; j < numOfArrays; j++) {
      // bit(1) array_completeness;
      // unsigned int(1) reserved = 0;
      // unsigned int(6) NAL_unit_type; // 34 (PPS)
      0xa2,
      // unsigned int(16) numNalus;
      0x00, 0x01,
      // for (i=0; i< numNalus; i++) {
      // unsigned int(16) nalUnitLength;
      0x00, 0x06,
      // offset: 100
      // PPS-start
      0x44, 0x01, 0xc1, 0x73, 0xd1, 0x89
      // PPS-end
  };

  // fuzzer::conv: begin
  H265BitstreamParserState bitstream_parser_state;
  ParsingOptions parsing_options;
  auto configuration_box = H265ConfigurationBoxParser::ParseConfigurationBox(
      buffer, arraysize(buffer), &bitstream_parser_state, parsing_options);
  // fuzzer::conv: end

  EXPECT_TRUE(configuration_box != nullptr);

  // hvcC header
  EXPECT_EQ(1, configuration_box->configurationVersion);
  EXPECT_EQ(0, configuration_box->general_profile_space);
  EXPECT_EQ(0, configuration_box->general_tier_flag);
  EXPECT_EQ(1, configuration_box->general_profile_idc);
  EXPECT_THAT(configuration_box->general_profile_compatibility_flags,
              ::testing::ElementsAreArray({0, 1, 1, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0}));
  EXPECT_EQ(0x800000000000,
            configuration_box->general_constraint_indicator_flags);

  EXPECT_EQ(120, configuration_box->general_level_idc);
  EXPECT_EQ(0b1111, configuration_box->reserved1);
  EXPECT_EQ(0, configuration_box->min_spatial_segmentation_idc);
  EXPECT_EQ(0b111111, configuration_box->reserved2);
  EXPECT_EQ(0, configuration_box->parallelismType);
  EXPECT_EQ(0b111111, configuration_box->reserved3);
  EXPECT_EQ(1, configuration_box->chromaFormat);
  EXPECT_EQ(0b11111, configuration_box->reserved4);
  EXPECT_EQ(0, configuration_box->bitDepthLumaMinus8);
  EXPECT_EQ(0b11111, configuration_box->reserved5);
  EXPECT_EQ(0, configuration_box->bitDepthChromaMinus8);
  EXPECT_EQ(0, configuration_box->avgFrameRate);
  EXPECT_EQ(0, configuration_box->constantFrameRate);
  EXPECT_EQ(1, configuration_box->numTemporalLayers);
  EXPECT_EQ(1, configuration_box->temporalIdNested);
  EXPECT_EQ(3, configuration_box->lengthSizeMinusOne);
  EXPECT_EQ(3, configuration_box->numOfArrays);
  EXPECT_THAT(configuration_box->array_completeness,
              ::testing::ElementsAreArray({1, 1, 1}));
  EXPECT_THAT(configuration_box->reserved6,
              ::testing::ElementsAreArray({0, 0, 0}));
  EXPECT_THAT(configuration_box->NAL_unit_type,
              ::testing::ElementsAreArray({32, 33, 34}));
  EXPECT_THAT(configuration_box->numNalus,
              ::testing::ElementsAreArray({1, 1, 1}));
  EXPECT_THAT(configuration_box->nalUnitLength[0],
              ::testing::ElementsAreArray({24}));
  EXPECT_THAT(configuration_box->nalUnitLength[1],
              ::testing::ElementsAreArray({39}));
  EXPECT_THAT(configuration_box->nalUnitLength[2],
              ::testing::ElementsAreArray({6}));

  // VPS
  auto& nalUnit0 = configuration_box->nalUnit[0].front();
  EXPECT_EQ(0, nalUnit0->nal_unit_header->forbidden_zero_bit);
  EXPECT_EQ(32, nalUnit0->nal_unit_header->nal_unit_type);
  EXPECT_EQ(0, nalUnit0->nal_unit_header->nuh_layer_id);
  EXPECT_EQ(1, nalUnit0->nal_unit_header->nuh_temporal_id_plus1);

  auto& vps = nalUnit0->nal_unit_payload->vps;
  EXPECT_EQ(0, vps->vps_video_parameter_set_id);
  EXPECT_EQ(1, vps->vps_base_layer_internal_flag);
  EXPECT_EQ(1, vps->vps_base_layer_available_flag);
  EXPECT_EQ(0, vps->vps_max_layers_minus1);
  EXPECT_EQ(0, vps->vps_max_sub_layers_minus1);
  EXPECT_EQ(1, vps->vps_temporal_id_nesting_flag);
  EXPECT_EQ(0xffff, vps->vps_reserved_0xffff_16bits);
  // profile_tier_level start
  EXPECT_EQ(0, vps->profile_tier_level->general->profile_space);
  EXPECT_EQ(0, vps->profile_tier_level->general->tier_flag);
  EXPECT_EQ(1, vps->profile_tier_level->general->profile_idc);
  EXPECT_THAT(vps->profile_tier_level->general->profile_compatibility_flag,
              ::testing::ElementsAreArray({0, 1, 1, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0}));
  EXPECT_EQ(1, vps->profile_tier_level->general->progressive_source_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->interlaced_source_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->non_packed_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->frame_only_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_12bit_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_10bit_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_8bit_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_422chroma_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_420chroma_constraint_flag);
  EXPECT_EQ(0,
            vps->profile_tier_level->general->max_monochrome_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->intra_constraint_flag);
  EXPECT_EQ(0,
            vps->profile_tier_level->general->one_picture_only_constraint_flag);
  EXPECT_EQ(0,
            vps->profile_tier_level->general->lower_bit_rate_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->max_14bit_constraint_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->reserved_zero_33bits);
  EXPECT_EQ(0, vps->profile_tier_level->general->reserved_zero_34bits);
  EXPECT_EQ(0, vps->profile_tier_level->general->reserved_zero_43bits);
  EXPECT_EQ(0, vps->profile_tier_level->general->inbld_flag);
  EXPECT_EQ(0, vps->profile_tier_level->general->reserved_zero_bit);
  EXPECT_EQ(120, vps->profile_tier_level->general_level_idc);
  EXPECT_EQ(0, vps->profile_tier_level->sub_layer_profile_present_flag.size());
  EXPECT_EQ(0, vps->profile_tier_level->sub_layer_level_present_flag.size());
  EXPECT_EQ(0, vps->profile_tier_level->reserved_zero_2bits.size());
  EXPECT_EQ(0, vps->profile_tier_level->sub_layer.size());
  // profile_tier_level end
  EXPECT_EQ(1, vps->vps_sub_layer_ordering_info_present_flag);
  EXPECT_THAT(vps->vps_max_dec_pic_buffering_minus1,
              ::testing::ElementsAreArray({6}));
  EXPECT_THAT(vps->vps_max_num_reorder_pics, ::testing::ElementsAreArray({2}));
  EXPECT_THAT(vps->vps_max_latency_increase_plus1,
              ::testing::ElementsAreArray({0}));
  EXPECT_EQ(0, vps->vps_max_layer_id);
  EXPECT_EQ(0, vps->vps_num_layer_sets_minus1);
  EXPECT_EQ(0, vps->layer_id_included_flag.size());
  EXPECT_EQ(0, vps->vps_timing_info_present_flag);
  EXPECT_EQ(0, vps->vps_num_units_in_tick);
  EXPECT_EQ(0, vps->vps_time_scale);
  EXPECT_EQ(0, vps->vps_poc_proportional_to_timing_flag);
  EXPECT_EQ(0, vps->vps_num_ticks_poc_diff_one_minus1);
  EXPECT_EQ(0, vps->vps_num_hrd_parameters);
  EXPECT_EQ(0, vps->hrd_layer_set_idx.size());
  EXPECT_EQ(0, vps->cprms_present_flag.size());
  EXPECT_EQ(0, vps->vps_extension_flag);
  EXPECT_EQ(0, vps->vps_extension_data_flag);

  // SPS
  auto& nalUnit1 = configuration_box->nalUnit[1].front();
  EXPECT_EQ(0, nalUnit1->nal_unit_header->forbidden_zero_bit);
  EXPECT_EQ(33, nalUnit1->nal_unit_header->nal_unit_type);
  EXPECT_EQ(0, nalUnit1->nal_unit_header->nuh_layer_id);
  EXPECT_EQ(1, nalUnit1->nal_unit_header->nuh_temporal_id_plus1);

  auto& sps = nalUnit1->nal_unit_payload->sps;
  EXPECT_EQ(0, sps->sps_video_parameter_set_id);
  EXPECT_EQ(0, sps->sps_max_sub_layers_minus1);
  EXPECT_EQ(1, sps->sps_temporal_id_nesting_flag);
  // profile_tier_level start
  EXPECT_EQ(0, sps->profile_tier_level->general->profile_space);
  EXPECT_EQ(0, sps->profile_tier_level->general->tier_flag);
  EXPECT_EQ(1, sps->profile_tier_level->general->profile_idc);
  EXPECT_THAT(sps->profile_tier_level->general->profile_compatibility_flag,
              ::testing::ElementsAreArray({0, 1, 1, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 0}));
  EXPECT_EQ(1, sps->profile_tier_level->general->progressive_source_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->interlaced_source_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->non_packed_constraint_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->frame_only_constraint_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->max_12bit_constraint_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->max_10bit_constraint_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->max_8bit_constraint_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->max_422chroma_constraint_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->max_420chroma_constraint_flag);
  EXPECT_EQ(0,
            sps->profile_tier_level->general->max_monochrome_constraint_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->intra_constraint_flag);
  EXPECT_EQ(0,
            sps->profile_tier_level->general->one_picture_only_constraint_flag);
  EXPECT_EQ(0,
            sps->profile_tier_level->general->lower_bit_rate_constraint_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->max_14bit_constraint_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->reserved_zero_33bits);
  EXPECT_EQ(0, sps->profile_tier_level->general->reserved_zero_34bits);
  EXPECT_EQ(0, sps->profile_tier_level->general->reserved_zero_43bits);
  EXPECT_EQ(0, sps->profile_tier_level->general->inbld_flag);
  EXPECT_EQ(0, sps->profile_tier_level->general->reserved_zero_bit);
  EXPECT_EQ(120, sps->profile_tier_level->general_level_idc);
  EXPECT_EQ(0, sps->profile_tier_level->sub_layer_profile_present_flag.size());
  EXPECT_EQ(0, sps->profile_tier_level->sub_layer_level_present_flag.size());
  EXPECT_EQ(0, sps->profile_tier_level->reserved_zero_2bits.size());
  EXPECT_EQ(0, sps->profile_tier_level->sub_layer.size());
  // profile_tier_level end
  EXPECT_EQ(0, sps->sps_seq_parameter_set_id);
  EXPECT_EQ(1, sps->chroma_format_idc);
  EXPECT_EQ(1920, sps->pic_width_in_luma_samples);
  EXPECT_EQ(800, sps->pic_height_in_luma_samples);
  EXPECT_EQ(0, sps->conformance_window_flag);
  EXPECT_EQ(0, sps->conf_win_left_offset);
  EXPECT_EQ(0, sps->conf_win_right_offset);
  EXPECT_EQ(0, sps->conf_win_top_offset);
  EXPECT_EQ(0, sps->conf_win_bottom_offset);
  EXPECT_EQ(0, sps->bit_depth_luma_minus8);
  EXPECT_EQ(0, sps->bit_depth_chroma_minus8);
  EXPECT_EQ(4, sps->log2_max_pic_order_cnt_lsb_minus4);
  EXPECT_EQ(1, sps->sps_sub_layer_ordering_info_present_flag);
  EXPECT_THAT(sps->sps_max_dec_pic_buffering_minus1,
              ::testing::ElementsAreArray({6}));
  EXPECT_THAT(sps->sps_max_num_reorder_pics, ::testing::ElementsAreArray({2}));
  EXPECT_THAT(sps->sps_max_latency_increase_plus1,
              ::testing::ElementsAreArray({0}));
  EXPECT_EQ(0, sps->log2_min_luma_coding_block_size_minus3);
  EXPECT_EQ(3, sps->log2_diff_max_min_luma_coding_block_size);
  EXPECT_EQ(0, sps->log2_min_luma_transform_block_size_minus2);
  EXPECT_EQ(3, sps->log2_diff_max_min_luma_transform_block_size);
  EXPECT_EQ(2, sps->max_transform_hierarchy_depth_inter);
  EXPECT_EQ(2, sps->max_transform_hierarchy_depth_intra);
  EXPECT_EQ(0, sps->scaling_list_enabled_flag);
  EXPECT_EQ(1, sps->amp_enabled_flag);
  EXPECT_EQ(1, sps->sample_adaptive_offset_enabled_flag);
  EXPECT_EQ(0, sps->pcm_enabled_flag);
  EXPECT_EQ(0, sps->num_short_term_ref_pic_sets);
  // vui_parameters()
  EXPECT_EQ(0, sps->vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->video_format);
  EXPECT_EQ(0, sps->vui_parameters->video_full_range_flag);
  EXPECT_EQ(0, sps->vui_parameters->colour_description_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->colour_primaries);
  EXPECT_EQ(0, sps->vui_parameters->transfer_characteristics);
  EXPECT_EQ(0, sps->vui_parameters->matrix_coeffs);
  EXPECT_EQ(0, sps->vui_parameters->chroma_loc_info_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->neutral_chroma_indication_flag);
  EXPECT_EQ(0, sps->vui_parameters->field_seq_flag);
  EXPECT_EQ(0, sps->vui_parameters->frame_field_info_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->default_display_window_flag);
  EXPECT_EQ(1, sps->vui_parameters->vui_timing_info_present_flag);
  EXPECT_EQ(1000, sps->vui_parameters->vui_num_units_in_tick);
  EXPECT_EQ(24000, sps->vui_parameters->vui_time_scale);
  EXPECT_EQ(0, sps->vui_parameters->vui_poc_proportional_to_timing_flag);
  EXPECT_EQ(0, sps->vui_parameters->vui_num_ticks_poc_diff_one_minus1);
  EXPECT_EQ(0, sps->vui_parameters->bitstream_restriction_flag);
  EXPECT_EQ(0, sps->vui_parameters->tiles_fixed_structure_flag);
  EXPECT_EQ(0, sps->vui_parameters->motion_vectors_over_pic_boundaries_flag);
  EXPECT_EQ(0, sps->vui_parameters->restricted_ref_pic_lists_flag);
  EXPECT_EQ(0, sps->vui_parameters->min_spatial_segmentation_idc);
  EXPECT_EQ(0, sps->vui_parameters->max_bytes_per_pic_denom);
  EXPECT_EQ(0, sps->vui_parameters->max_bits_per_min_cu_denom);
  EXPECT_EQ(0, sps->vui_parameters->log2_max_mv_length_horizontal);
  EXPECT_EQ(0, sps->vui_parameters->log2_max_mv_length_vertical);
  EXPECT_EQ(0, sps->sps_extension_present_flag);
  EXPECT_EQ(0, sps->sps_range_extension_flag);
  EXPECT_EQ(0, sps->sps_multilayer_extension_flag);
  EXPECT_EQ(0, sps->sps_3d_extension_flag);
  EXPECT_EQ(0, sps->sps_scc_extension_flag);
  EXPECT_EQ(0, sps->sps_extension_4bits);

  // PPS
  auto& nalUnit2 = configuration_box->nalUnit[2].front();
  EXPECT_EQ(0, nalUnit2->nal_unit_header->forbidden_zero_bit);
  EXPECT_EQ(34, nalUnit2->nal_unit_header->nal_unit_type);
  EXPECT_EQ(0, nalUnit2->nal_unit_header->nuh_layer_id);
  EXPECT_EQ(1, nalUnit2->nal_unit_header->nuh_temporal_id_plus1);

  auto& pps = nalUnit2->nal_unit_payload->pps;
  EXPECT_EQ(0, pps->pps_pic_parameter_set_id);
  EXPECT_EQ(0, pps->pps_seq_parameter_set_id);
  EXPECT_EQ(0, pps->dependent_slice_segments_enabled_flag);
  EXPECT_EQ(0, pps->output_flag_present_flag);
  EXPECT_EQ(0, pps->num_extra_slice_header_bits);
  EXPECT_EQ(1, pps->sign_data_hiding_enabled_flag);
  EXPECT_EQ(0, pps->cabac_init_present_flag);
  EXPECT_EQ(0, pps->num_ref_idx_l0_default_active_minus1);
  EXPECT_EQ(0, pps->num_ref_idx_l1_default_active_minus1);
  EXPECT_EQ(0, pps->init_qp_minus26);
  EXPECT_EQ(0, pps->constrained_intra_pred_flag);
  EXPECT_EQ(0, pps->transform_skip_enabled_flag);
  EXPECT_EQ(1, pps->cu_qp_delta_enabled_flag);
  EXPECT_EQ(0, pps->diff_cu_qp_delta_depth);
  EXPECT_EQ(0, pps->pps_cb_qp_offset);
  EXPECT_EQ(0, pps->pps_cr_qp_offset);
  EXPECT_EQ(0, pps->pps_slice_chroma_qp_offsets_present_flag);
  EXPECT_EQ(1, pps->weighted_pred_flag);
  EXPECT_EQ(0, pps->weighted_bipred_flag);
  EXPECT_EQ(0, pps->transquant_bypass_enabled_flag);
  EXPECT_EQ(0, pps->tiles_enabled_flag);
  EXPECT_EQ(1, pps->entropy_coding_sync_enabled_flag);
  EXPECT_EQ(1, pps->pps_loop_filter_across_slices_enabled_flag);
  EXPECT_EQ(0, pps->deblocking_filter_control_present_flag);
  EXPECT_EQ(0, pps->pps_scaling_list_data_present_flag);
  EXPECT_EQ(0, pps->lists_modification_present_flag);
  EXPECT_EQ(0, pps->log2_parallel_merge_level_minus2);
  EXPECT_EQ(0, pps->slice_segment_header_extension_present_flag);
  EXPECT_EQ(0, pps->pps_extension_present_flag);
}

}  // namespace h265nal
