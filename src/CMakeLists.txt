# add executable
#add_executable(main main.cc)

# link executable to webrtc library
# Since the webrtc library has public include directories
# we will use those link directories when building `main`
#target_link_libraries(main LINK_PUBLIC webrtc)

find_library(GTEST_LIBRARY gtest)
find_library(GMOCK_LIBRARY gmock)

add_library(h265nal
    h265_common.h h265_common.cc
    h265_profile_tier_level_parser.h h265_profile_tier_level_parser.cc
    h265_vps_parser.h h265_vps_parser.cc
    h265_vui_parameters_parser.h h265_vui_parameters_parser.cc
    h265_st_ref_pic_set_parser.h h265_st_ref_pic_set_parser.cc
    h265_sps_parser.h h265_sps_parser.cc
    h265_pps_parser.h h265_pps_parser.cc
    h265_aud_parser.h h265_aud_parser.cc
    h265_slice_parser.h h265_slice_parser.cc
    h265_bitstream_parser_state.h
    h265_bitstream_parser.h h265_bitstream_parser.cc
    h265_nal_unit_parser.h h265_nal_unit_parser.cc
)
target_include_directories(h265nal PUBLIC ../webrtc)
target_link_libraries(h265nal PUBLIC webrtc)

# https://cmake.org/cmake/help/latest/guide/tutorial/index.html#adding-a-version-number-and-configured-header-file
configure_file(config.h.in config.h)

# this allows the 'config.h' file to be used throughout the program
target_include_directories(h265nal PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")


add_executable(h265_profile_tier_level_parser_unittest h265_profile_tier_level_parser_unittest.cc)
add_test(h265_profile_tier_level_parser_unittest h265_profile_tier_level_parser_unittest)
#target_include_directories(h265_profile_tier_level_parser_unittest PUBLIC /usr/local/include ${CMAKE_CURRENT_SOURCE_DIR} webrtc)

target_link_libraries(h265_profile_tier_level_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_profile_tier_level_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_profile_tier_level_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265_vps_parser_unittest h265_vps_parser_unittest.cc)
add_test(h265_vps_parser_unittest h265_vps_parser_unittest)
target_link_libraries(h265_vps_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_vps_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_vps_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265_vui_parameters_parser_unittest h265_vui_parameters_parser_unittest.cc)
add_test(h265_vui_parameters_parser_unittest h265_vui_parameters_parser_unittest)
target_link_libraries(h265_vui_parameters_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_vui_parameters_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_vui_parameters_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265_st_ref_pic_set_parser_unittest h265_st_ref_pic_set_parser_unittest.cc)
add_test(h265_st_ref_pic_set_parser_unittest h265_st_ref_pic_set_parser_unittest)
target_link_libraries(h265_st_ref_pic_set_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_st_ref_pic_set_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_st_ref_pic_set_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265_sps_parser_unittest h265_sps_parser_unittest.cc)
add_test(h265_sps_parser_unittest h265_sps_parser_unittest)
target_link_libraries(h265_sps_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_sps_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_sps_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265_pps_parser_unittest h265_pps_parser_unittest.cc)
add_test(h265_pps_parser_unittest h265_pps_parser_unittest)
target_link_libraries(h265_pps_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_pps_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_pps_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265_aud_parser_unittest h265_aud_parser_unittest.cc)
add_test(h265_aud_parser_unittest h265_aud_parser_unittest)
target_link_libraries(h265_aud_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_aud_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_aud_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265_slice_parser_unittest h265_slice_parser_unittest.cc)
add_test(h265_slice_parser_unittest h265_slice_parser_unittest)
target_link_libraries(h265_slice_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_slice_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_slice_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265_bitstream_parser_unittest h265_bitstream_parser_unittest.cc)
add_test(h265_bitstream_parser_unittest h265_bitstream_parser_unittest)
target_link_libraries(h265_bitstream_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_bitstream_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_bitstream_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265_nal_unit_parser_unittest h265_nal_unit_parser_unittest.cc)
add_test(h265_nal_unit_parser_unittest h265_nal_unit_parser_unittest)
target_link_libraries(h265_nal_unit_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_nal_unit_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_nal_unit_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)

add_executable(h265nal-bin h265nal.cc)
target_link_libraries(h265nal-bin PUBLIC h265nal)
# rename executable using target properties
set_target_properties(h265nal-bin PROPERTIES OUTPUT_NAME h265nal)
