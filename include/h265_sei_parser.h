/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "rtc_base/bit_buffer.h"

namespace h265nal {

enum class SeiType {
    buffering_period = 0,
    pic_timing = 1,
    pan_scan_rect = 2,
    filler_payload = 3,
    user_data_registered_itu_t_t35 = 4,
    user_data_unregistered = 5,
    recovery_point = 6,
    scene_info = 9,
    picture_snapshot = 15,
    progressive_refinement_segment_start = 16,
    progressive_refinement_segment_end = 17,
    film_grain_characteristics = 19,
    post_filter_hint = 22,
    tone_mapping_info = 23, 
    frame_packing_arrangement = 45, 
    display_orientation = 47,
    green_metadata = 56,
    structure_of_pictures_info = 128,
    active_parameter_sets = 129,
    decoding_unit_info = 130,
    temporal_sub_layer_zero_idx = 131, 
    scalable_nesting = 133,
    region_refresh_info = 134, 
    no_display = 135,
    time_code = 136, 
    mastering_display_colour_volume = 137, 
    segmented_rect_frame_packing_arrangement = 138, 
    temporal_motion_constrained_tile_sets = 139, 
    chroma_resampling_filter_hint = 140, 
    knee_function_info = 141,
    colour_remapping_info = 142,
    deinterlaced_field_identification = 143,
    content_light_level_info = 144,
    dependent_rap_indication = 145, 
    coded_region_completion = 146,
    alternative_transfer_characteristics = 147,
    ambient_viewing_environment = 148,
    content_colour_volume = 149,
    equirectangular_projection = 150,
    cubemap_projection = 151,
    fisheye_video_info = 152,
    sphere_rotation = 154,
    regionwise_packing = 155,
    omni_viewport = 156,
    regional_nesting = 157,
    mcts_extraction_info_sets = 158,
    mcts_extraction_info_nesting = 159,
    layers_not_present = 160,
    inter_layer_constrained_tile_sets = 161,
    bsp_nesting = 162,
    bsp_initial_arrival_time = 163,
    sub_bitstream_property = 164,
    alpha_channel_info = 165,
    overlay_info = 166,
    temporal_mv_prediction_constraints = 167,
    frame_field_info = 168,
    three_dimensional_reference_displays_info = 176,
    depth_representation_info = 177,
    multiview_scene_info = 178,
    multiview_acquisition_info = 179,
    multiview_view_position = 180,
    alternative_depth_info = 181,
    sei_manifest = 200,
    sei_prefix_indication = 201,
    annotated_regions = 202,
    shutter_interval_info = 205,
    not_set = 999
};

// A class for parsing out a supplemental enhancement information (SEI) data from
// an H265 NALU.
class H265SeiParser {
 public:
  // The parsed state of the SEI.
  struct SeiState {
    SeiState() = default;
    virtual ~SeiState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    SeiState(const SeiState&) = delete;
    SeiState(SeiState&&) = delete;
    SeiState& operator=(const SeiState&) = delete;
    SeiState& operator=(SeiState&&) = delete;
    virtual void parse_payload() = 0;

#ifdef FDUMP_DEFINE
    virtual void fdump(FILE* outfp, int indent_level) const = 0;
#endif  // FDUMP_DEFINE

    SeiType payload_type = SeiType::not_set;
    uint32_t payload_size = 0;
    std::vector<uint8_t> payload;
  };

  // There are many SEI types.  All have payloads.
  // With the SeiStateNotImplemented class,
  // the specifics of the SEI parsing are not yet
  // implemented the data is stored in the payload
  // and no further action is taken.
  struct SeiStateNotImplemented: public SeiState {
#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE
    void parse_payload();
  };

  // D.2.6 User data registered by Recommendation ITU-T T.35 SEI message syntax
  struct SeiStateUserData: public SeiState {
  public:
#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE
    void parse_payload();
    const uint8_t* get_userdata_payload(size_t& payload_size) const;
    uint8_t itu_t_t35_country_code  = 0;
    uint8_t itu_t_t35_country_code_extension_byte = 0;
  private:
    size_t  userdata_payload_start_index = 0;
  };

  // Unpack RBSP and parse SEI state from the supplied buffer.
  static std::unique_ptr<SeiState> ParseSei(const uint8_t* data,
                                            size_t length) noexcept;

  static std::unique_ptr<SeiState> ParseSei(
      rtc::BitBuffer* bit_buffer) noexcept;
};

}  // namespace h265nal
