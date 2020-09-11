# add executable
#add_executable (main main.cc)

# link executable to webrtc library
# Since the webrtc library has public include directories
# we will use those link directories when building `main`
#target_link_libraries (main LINK_PUBLIC webrtc)

find_library(GTEST_LIBRARY gtest)
find_library(GMOCK_LIBRARY gmock)

add_library (h265nal
    h265_common.h h265_common.cc
    h265_profile_tier_level_parser.h h265_profile_tier_level_parser.cc)
target_include_directories(h265nal PUBLIC ../webrtc)
target_link_libraries(h265nal PUBLIC webrtc)


add_executable(h265_profile_tier_level_parser_unittest h265_profile_tier_level_parser_unittest.cc)
add_test(h265_profile_tier_level_parser_unittest h265_profile_tier_level_parser_unittest)
#target_include_directories(h265_profile_tier_level_parser_unittest PUBLIC /usr/local/include ${CMAKE_CURRENT_SOURCE_DIR} webrtc)

target_link_libraries(h265_profile_tier_level_parser_unittest PUBLIC h265nal)
target_link_libraries(h265_profile_tier_level_parser_unittest PUBLIC ${GTEST_LIBRARY} gtest_main)
target_link_libraries(h265_profile_tier_level_parser_unittest PUBLIC ${GMOCK_LIBRARY} gmock_main)
