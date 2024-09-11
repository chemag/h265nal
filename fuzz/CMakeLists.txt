
include_directories(PUBLIC ../include)

include_directories(PUBLIC ../src)

# MacOS requirements
include_directories(PUBLIC /usr/local/include)
link_directories(/usr/local/lib)

add_compile_options(-fsanitize=address,fuzzer,undefined)
add_link_options(-fsanitize=address,fuzzer,undefined)

function(add_fuzzer TARGET SOURCE)
  if(NOT WIN32)
    add_executable(${TARGET} ${SOURCE})
    target_link_libraries(${TARGET} PUBLIC h265nal)
  endif()
endfunction()

add_fuzzer(h265_utils_fuzzer h265_utils_fuzzer.cc)
add_fuzzer(h265_profile_tier_level_parser_fuzzer h265_profile_tier_level_parser_fuzzer.cc)
add_fuzzer(h265_sub_layer_hrd_parameters_parser_fuzzer h265_sub_layer_hrd_parameters_parser_fuzzer.cc)
add_fuzzer(h265_hrd_parameters_parser_fuzzer h265_hrd_parameters_parser_fuzzer.cc)
add_fuzzer(h265_vps_parser_fuzzer h265_vps_parser_fuzzer.cc)
add_fuzzer(h265_vui_parameters_parser_fuzzer h265_vui_parameters_parser_fuzzer.cc)
add_fuzzer(h265_pred_weight_table_parser_fuzzer h265_pred_weight_table_parser_fuzzer.cc)
add_fuzzer(h265_st_ref_pic_set_parser_fuzzer h265_st_ref_pic_set_parser_fuzzer.cc)
add_fuzzer(h265_sps_scc_extension_parser_fuzzer h265_sps_scc_extension_parser_fuzzer.cc)
add_fuzzer(h265_sps_multilayer_extension_parser_fuzzer h265_sps_multilayer_extension_parser_fuzzer.cc)
add_fuzzer(h265_sps_3d_extension_parser_fuzzer h265_sps_3d_extension_parser_fuzzer.cc)
add_fuzzer(h265_sps_range_extension_parser_fuzzer h265_sps_range_extension_parser_fuzzer.cc)
add_fuzzer(h265_scaling_list_data_parser_fuzzer h265_scaling_list_data_parser_fuzzer.cc)
add_fuzzer(h265_sps_parser_fuzzer h265_sps_parser_fuzzer.cc)
add_fuzzer(h265_pps_scc_extension_parser_fuzzer h265_pps_scc_extension_parser_fuzzer.cc)
add_fuzzer(h265_pps_parser_fuzzer h265_pps_parser_fuzzer.cc)
add_fuzzer(h265_aud_parser_fuzzer h265_aud_parser_fuzzer.cc)
add_fuzzer(h265_sei_parser_fuzzer h265_sei_parser_fuzzer.cc)

if(H265NAL_SMALL_FOOTPRINT)
  message(STATUS "fuzz: small footprint selected")

else()
  message(STATUS "fuzz: normal footprint selected")
  add_compile_definitions(RTP_DEFINE)
  add_fuzzer(h265_rtp_single_parser_fuzzer h265_rtp_single_parser_fuzzer.cc)
  add_fuzzer(h265_rtp_parser_fuzzer h265_rtp_parser_fuzzer.cc)
  add_fuzzer(h265_rtp_ap_parser_fuzzer h265_rtp_ap_parser_fuzzer.cc)
  add_fuzzer(h265_rtp_fu_parser_fuzzer h265_rtp_fu_parser_fuzzer.cc)
endif()

add_fuzzer(h265_slice_parser_fuzzer h265_slice_parser_fuzzer.cc)
add_fuzzer(h265_bitstream_parser_fuzzer h265_bitstream_parser_fuzzer.cc)
add_fuzzer(h265_nal_unit_parser_fuzzer h265_nal_unit_parser_fuzzer.cc)
add_fuzzer(h265_configuration_box_parser_fuzzer h265_configuration_box_parser_fuzzer.cc)
