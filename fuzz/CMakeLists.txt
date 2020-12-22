# link executable to webrtc library
# Since the webrtc library has public include directories
# we will use those link directories when building `main`
#target_link_libraries(main LINK_PUBLIC webrtc)

include_directories(PUBLIC ../src)
include_directories(PUBLIC ../webrtc)

# MacOS requirements
include_directories(PUBLIC /usr/local/include)
link_directories(/usr/local/lib)

add_compile_options(-fsanitize=address,fuzzer)
add_link_options(-fsanitize=address,fuzzer)

#target_include_directories(h265_profile_tier_level_parser_fuzzer PUBLIC /usr/local/include ${CMAKE_CURRENT_SOURCE_DIR} webrtc)

add_executable(h265_profile_tier_level_parser_fuzzer h265_profile_tier_level_parser_fuzzer.cc)
target_link_libraries(h265_profile_tier_level_parser_fuzzer PUBLIC h265nal)

add_executable(h265_vps_parser_fuzzer h265_vps_parser_fuzzer.cc)
target_link_libraries(h265_vps_parser_fuzzer PUBLIC h265nal)

add_executable(h265_vui_parameters_parser_fuzzer h265_vui_parameters_parser_fuzzer.cc)
target_link_libraries(h265_vui_parameters_parser_fuzzer PUBLIC h265nal)

add_executable(h265_st_ref_pic_set_parser_fuzzer h265_st_ref_pic_set_parser_fuzzer.cc)
target_link_libraries(h265_st_ref_pic_set_parser_fuzzer PUBLIC h265nal)

add_executable(h265_sps_parser_fuzzer h265_sps_parser_fuzzer.cc)
target_link_libraries(h265_sps_parser_fuzzer PUBLIC h265nal)

add_executable(h265_pps_parser_fuzzer h265_pps_parser_fuzzer.cc)
target_link_libraries(h265_pps_parser_fuzzer PUBLIC h265nal)

add_executable(h265_aud_parser_fuzzer h265_aud_parser_fuzzer.cc)
target_link_libraries(h265_aud_parser_fuzzer PUBLIC h265nal)

if(H265NAL_SMALL_FOOTPRINT)
  message(STATUS "fuzz: small footprint selected")

else()
  message(STATUS "fuzz: normal footprint selected")
  add_compile_definitions(RTP_DEFINE)
  add_executable(h265_rtp_single_parser_fuzzer h265_rtp_single_parser_fuzzer.cc)
  target_link_libraries(h265_rtp_single_parser_fuzzer PUBLIC h265nal)

  add_executable(h265_rtp_parser_fuzzer h265_rtp_parser_fuzzer.cc)
  target_link_libraries(h265_rtp_parser_fuzzer PUBLIC h265nal)

  add_executable(h265_rtp_ap_parser_fuzzer h265_rtp_ap_parser_fuzzer.cc)
  target_link_libraries(h265_rtp_ap_parser_fuzzer PUBLIC h265nal)

  add_executable(h265_rtp_fu_parser_fuzzer h265_rtp_fu_parser_fuzzer.cc)
  target_link_libraries(h265_rtp_fu_parser_fuzzer PUBLIC h265nal)
endif()

add_executable(h265_slice_parser_fuzzer h265_slice_parser_fuzzer.cc)
target_link_libraries(h265_slice_parser_fuzzer PUBLIC h265nal)

add_executable(h265_bitstream_parser_fuzzer h265_bitstream_parser_fuzzer.cc)
target_link_libraries(h265_bitstream_parser_fuzzer PUBLIC h265nal)

add_executable(h265_nal_unit_parser_fuzzer h265_nal_unit_parser_fuzzer.cc)
target_link_libraries(h265_nal_unit_parser_fuzzer PUBLIC h265nal)