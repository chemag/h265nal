# add executable
#add_executable(main main.cc)

option(H265NAL_SMALL_FOOTPRINT, "small footprint build")

if(H265NAL_SMALL_FOOTPRINT)
  message(STATUS "src: small footprint selected")
  add_compile_definitions(SMALL_FOOTPRINT)
else()
  message(STATUS "src: normal footprint selected")
  add_compile_definitions(FDUMP_DEFINE)
  add_compile_definitions(RTP_DEFINE)
  add_compile_definitions(FPRINT_ERRORS)
endif()

add_executable(h265nal-bin h265nal.cc)
target_include_directories(h265nal-bin PUBLIC ../src)
target_link_libraries(h265nal-bin PUBLIC h265nal)
# rename executable using target properties
set_target_properties(h265nal-bin PROPERTIES OUTPUT_NAME h265nal)

# getopt port needed on windows
if(WIN32)
  target_include_directories(h265nal-bin PRIVATE ../ya_getopt)
  target_sources(h265nal-bin PRIVATE ../ya_getopt/ya_getopt.c)
endif()
