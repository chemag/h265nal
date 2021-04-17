



all: \
    h265_profile_tier_level_parser_fuzzer.cc \
    h265_vps_parser_fuzzer.cc \
    h265_vui_parameters_parser_fuzzer.cc \
    h265_pred_weight_table_parser_fuzzer.cc \
    h265_st_ref_pic_set_parser_fuzzer.cc \
    h265_sps_scc_extension_parser_fuzzer.cc \
    h265_sps_multilayer_extension_parser_fuzzer.cc \
    h265_sps_range_extension_parser_fuzzer.cc \
    h265_sps_parser_fuzzer.cc \
    h265_pps_parser_fuzzer.cc \
    h265_aud_parser_fuzzer.cc \
    h265_rtp_single_parser_fuzzer.cc \
    h265_rtp_parser_fuzzer.cc \
    h265_rtp_ap_parser_fuzzer.cc \
    h265_rtp_fu_parser_fuzzer.cc \
    h265_slice_parser_fuzzer.cc \
    h265_bitstream_parser_fuzzer.cc \
    h265_nal_unit_parser_fuzzer.cc


h265_profile_tier_level_parser_fuzzer.cc: ../test/h265_profile_tier_level_parser_unittest.cc
	./converter.py ../test/h265_profile_tier_level_parser_unittest.cc ./

h265_vps_parser_fuzzer.cc: ../test/h265_vps_parser_unittest.cc
	./converter.py ../test/h265_vps_parser_unittest.cc ./

h265_vui_parameters_parser_fuzzer.cc: ../test/h265_vui_parameters_parser_unittest.cc
	./converter.py ../test/h265_vui_parameters_parser_unittest.cc ./

h265_pred_weight_table_parser_fuzzer.cc: ../test/h265_pred_weight_table_parser_unittest.cc
	./converter.py ../test/h265_pred_weight_table_parser_unittest.cc ./

h265_st_ref_pic_set_parser_fuzzer.cc: ../test/h265_st_ref_pic_set_parser_unittest.cc
	./converter.py ../test/h265_st_ref_pic_set_parser_unittest.cc ./

h265_sps_scc_extension_parser_fuzzer.cc: ../test/h265_sps_scc_extension_parser_unittest.cc
	./converter.py ../test/h265_sps_scc_extension_parser_unittest.cc ./

h265_sps_multilayer_extension_parser_fuzzer.cc: ../test/h265_sps_multilayer_extension_parser_unittest.cc
	./converter.py ../test/h265_sps_multilayer_extension_parser_unittest.cc ./

h265_sps_range_extension_parser_fuzzer.cc: ../test/h265_sps_range_extension_parser_unittest.cc
	./converter.py ../test/h265_sps_range_extension_parser_unittest.cc ./

h265_sps_parser_fuzzer.cc: ../test/h265_sps_parser_unittest.cc
	./converter.py ../test/h265_sps_parser_unittest.cc ./

h265_pps_parser_fuzzer.cc: ../test/h265_pps_parser_unittest.cc
	./converter.py ../test/h265_pps_parser_unittest.cc ./

h265_aud_parser_fuzzer.cc: ../test/h265_aud_parser_unittest.cc
	./converter.py ../test/h265_aud_parser_unittest.cc ./

h265_rtp_single_parser_fuzzer.cc: ../test/h265_rtp_single_parser_unittest.cc
	./converter.py ../test/h265_rtp_single_parser_unittest.cc ./

h265_rtp_parser_fuzzer.cc: ../test/h265_rtp_parser_unittest.cc
	./converter.py ../test/h265_rtp_parser_unittest.cc ./

h265_rtp_ap_parser_fuzzer.cc: ../test/h265_rtp_ap_parser_unittest.cc
	./converter.py ../test/h265_rtp_ap_parser_unittest.cc ./

h265_rtp_fu_parser_fuzzer.cc: ../test/h265_rtp_fu_parser_unittest.cc
	./converter.py ../test/h265_rtp_fu_parser_unittest.cc ./

h265_slice_parser_fuzzer.cc: ../test/h265_slice_parser_unittest.cc
	./converter.py ../test/h265_slice_parser_unittest.cc ./

h265_bitstream_parser_fuzzer.cc: ../test/h265_bitstream_parser_unittest.cc
	./converter.py ../test/h265_bitstream_parser_unittest.cc ./

h265_nal_unit_parser_fuzzer.cc: ../test/h265_nal_unit_parser_unittest.cc
	./converter.py ../test/h265_nal_unit_parser_unittest.cc ./


RUNS=1000000

fuzz:
	-../build/fuzz/h265_profile_tier_level_parser_fuzzer -artifact_prefix=corpus/h265_profile_tier_level_parser_fuzzer/ corpus/h265_profile_tier_level_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_vps_parser_fuzzer -artifact_prefix=corpus/h265_vps_parser_fuzzer/ corpus/h265_vps_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_vui_parameters_parser_fuzzer -artifact_prefix=corpus/h265_vui_parameters_parser_fuzzer/ corpus/h265_vui_parameters_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_pred_weight_table_parser_fuzzer -artifact_prefix=corpus/h265_pred_weight_table_parser_fuzzer/ corpus/h265_pred_weight_table_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_st_ref_pic_set_parser_fuzzer -artifact_prefix=corpus/h265_st_ref_pic_set_parser_fuzzer/ corpus/h265_st_ref_pic_set_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_sps_scc_extension_parser_fuzzer -artifact_prefix=corpus/h265_sps_scc_extension_parser_fuzzer/ corpus/h265_sps_scc_extension_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_sps_multilayer_extension_parser_fuzzer -artifact_prefix=corpus/h265_sps_multilayer_extension_parser_fuzzer/ corpus/h265_sps_multilayer_extension_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_sps_range_extension_parser_fuzzer -artifact_prefix=corpus/h265_sps_range_extension_parser_fuzzer/ corpus/h265_sps_range_extension_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_sps_parser_fuzzer -artifact_prefix=corpus/h265_sps_parser_fuzzer/ corpus/h265_sps_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_pps_parser_fuzzer -artifact_prefix=corpus/h265_pps_parser_fuzzer/ corpus/h265_pps_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_aud_parser_fuzzer -artifact_prefix=corpus/h265_aud_parser_fuzzer/ corpus/h265_aud_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_rtp_single_parser_fuzzer -artifact_prefix=corpus/h265_rtp_single_parser_fuzzer/ corpus/h265_rtp_single_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_rtp_parser_fuzzer -artifact_prefix=corpus/h265_rtp_parser_fuzzer/ corpus/h265_rtp_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_rtp_ap_parser_fuzzer -artifact_prefix=corpus/h265_rtp_ap_parser_fuzzer/ corpus/h265_rtp_ap_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_rtp_fu_parser_fuzzer -artifact_prefix=corpus/h265_rtp_fu_parser_fuzzer/ corpus/h265_rtp_fu_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_slice_parser_fuzzer -artifact_prefix=corpus/h265_slice_parser_fuzzer/ corpus/h265_slice_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_bitstream_parser_fuzzer -artifact_prefix=corpus/h265_bitstream_parser_fuzzer/ corpus/h265_bitstream_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h265_nal_unit_parser_fuzzer -artifact_prefix=corpus/h265_nal_unit_parser_fuzzer/ corpus/h265_nal_unit_parser_fuzzer/ -runs=$(RUNS)

