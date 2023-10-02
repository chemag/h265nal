



all: \
    h265_utils_fuzzer.cc \
    h265_profile_tier_level_parser_fuzzer.cc \
    h265_sub_layer_hrd_parameters_parser_fuzzer.cc \
    h265_hrd_parameters_parser_fuzzer.cc \
    h265_vps_parser_fuzzer.cc \
    h265_vui_parameters_parser_fuzzer.cc \
    h265_pred_weight_table_parser_fuzzer.cc \
    h265_st_ref_pic_set_parser_fuzzer.cc \
    h265_sps_scc_extension_parser_fuzzer.cc \
    h265_sps_multilayer_extension_parser_fuzzer.cc \
    h265_sps_3d_extension_parser_fuzzer.cc \
    h265_sps_range_extension_parser_fuzzer.cc \
    h265_scaling_list_data_parser_fuzzer.cc \
    h265_sps_parser_fuzzer.cc \
    h265_pps_scc_extension_parser_fuzzer.cc \
    h265_pps_parser_fuzzer.cc \
    h265_aud_parser_fuzzer.cc \
    h265_rtp_single_parser_fuzzer.cc \
    h265_rtp_parser_fuzzer.cc \
    h265_rtp_ap_parser_fuzzer.cc \
    h265_rtp_fu_parser_fuzzer.cc \
    h265_slice_parser_fuzzer.cc \
    h265_bitstream_parser_fuzzer.cc \
    h265_nal_unit_parser_fuzzer.cc
	@echo "run: RUNS=1000000 make -j -Oline fuzz"


h265_utils_fuzzer.cc: ../test/h265_utils_unittest.cc
	./converter.py ../test/h265_utils_unittest.cc ./

h265_profile_tier_level_parser_fuzzer.cc: ../test/h265_profile_tier_level_parser_unittest.cc
	./converter.py ../test/h265_profile_tier_level_parser_unittest.cc ./

h265_sub_layer_hrd_parameters_parser_fuzzer.cc: ../test/h265_sub_layer_hrd_parameters_parser_unittest.cc
	./converter.py ../test/h265_sub_layer_hrd_parameters_parser_unittest.cc ./

h265_hrd_parameters_parser_fuzzer.cc: ../test/h265_hrd_parameters_parser_unittest.cc
	./converter.py ../test/h265_hrd_parameters_parser_unittest.cc ./

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

h265_sps_3d_extension_parser_fuzzer.cc: ../test/h265_sps_3d_extension_parser_unittest.cc
	./converter.py ../test/h265_sps_3d_extension_parser_unittest.cc ./

h265_sps_range_extension_parser_fuzzer.cc: ../test/h265_sps_range_extension_parser_unittest.cc
	./converter.py ../test/h265_sps_range_extension_parser_unittest.cc ./

h265_scaling_list_data_parser_fuzzer.cc: ../test/h265_scaling_list_data_parser_unittest.cc
	./converter.py ../test/h265_scaling_list_data_parser_unittest.cc ./

h265_sps_parser_fuzzer.cc: ../test/h265_sps_parser_unittest.cc
	./converter.py ../test/h265_sps_parser_unittest.cc ./

h265_pps_scc_extension_parser_fuzzer.cc: ../test/h265_pps_scc_extension_parser_unittest.cc
	./converter.py ../test/h265_pps_scc_extension_parser_unittest.cc ./

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


RUNS?=1000000

fuzz: \
    h265_utils_fuzzer \
    h265_profile_tier_level_parser_fuzzer \
    h265_sub_layer_hrd_parameters_parser_fuzzer \
    h265_hrd_parameters_parser_fuzzer \
    h265_vps_parser_fuzzer \
    h265_vui_parameters_parser_fuzzer \
    h265_pred_weight_table_parser_fuzzer \
    h265_st_ref_pic_set_parser_fuzzer \
    h265_sps_scc_extension_parser_fuzzer \
    h265_sps_multilayer_extension_parser_fuzzer \
    h265_sps_3d_extension_parser_fuzzer \
    h265_sps_range_extension_parser_fuzzer \
    h265_scaling_list_data_parser_fuzzer \
    h265_sps_parser_fuzzer \
    h265_pps_scc_extension_parser_fuzzer \
    h265_pps_parser_fuzzer \
    h265_aud_parser_fuzzer \
    h265_rtp_single_parser_fuzzer \
    h265_rtp_parser_fuzzer \
    h265_rtp_ap_parser_fuzzer \
    h265_rtp_fu_parser_fuzzer \
    h265_slice_parser_fuzzer \
    h265_bitstream_parser_fuzzer \
    h265_nal_unit_parser_fuzzer

h265_utils_fuzzer:
	-../build/fuzz/h265_utils_fuzzer -artifact_prefix=corpus/h265_utils_fuzzer/ corpus/h265_utils_fuzzer/ -runs=$(RUNS)

h265_profile_tier_level_parser_fuzzer:
	-../build/fuzz/h265_profile_tier_level_parser_fuzzer -artifact_prefix=corpus/h265_profile_tier_level_parser_fuzzer/ corpus/h265_profile_tier_level_parser_fuzzer/ -runs=$(RUNS)

h265_sub_layer_hrd_parameters_parser_fuzzer:
	-../build/fuzz/h265_sub_layer_hrd_parameters_parser_fuzzer -artifact_prefix=corpus/h265_sub_layer_hrd_parameters_parser_fuzzer/ corpus/h265_sub_layer_hrd_parameters_parser_fuzzer/ -runs=$(RUNS)

h265_hrd_parameters_parser_fuzzer:
	-../build/fuzz/h265_hrd_parameters_parser_fuzzer -artifact_prefix=corpus/h265_hrd_parameters_parser_fuzzer/ corpus/h265_hrd_parameters_parser_fuzzer/ -runs=$(RUNS)

h265_vps_parser_fuzzer:
	-../build/fuzz/h265_vps_parser_fuzzer -artifact_prefix=corpus/h265_vps_parser_fuzzer/ corpus/h265_vps_parser_fuzzer/ -runs=$(RUNS)

h265_vui_parameters_parser_fuzzer:
	-../build/fuzz/h265_vui_parameters_parser_fuzzer -artifact_prefix=corpus/h265_vui_parameters_parser_fuzzer/ corpus/h265_vui_parameters_parser_fuzzer/ -runs=$(RUNS)

h265_pred_weight_table_parser_fuzzer:
	-../build/fuzz/h265_pred_weight_table_parser_fuzzer -artifact_prefix=corpus/h265_pred_weight_table_parser_fuzzer/ corpus/h265_pred_weight_table_parser_fuzzer/ -runs=$(RUNS)

h265_st_ref_pic_set_parser_fuzzer:
	-../build/fuzz/h265_st_ref_pic_set_parser_fuzzer -artifact_prefix=corpus/h265_st_ref_pic_set_parser_fuzzer/ corpus/h265_st_ref_pic_set_parser_fuzzer/ -runs=$(RUNS)

h265_sps_scc_extension_parser_fuzzer:
	-../build/fuzz/h265_sps_scc_extension_parser_fuzzer -artifact_prefix=corpus/h265_sps_scc_extension_parser_fuzzer/ corpus/h265_sps_scc_extension_parser_fuzzer/ -runs=$(RUNS)

h265_sps_multilayer_extension_parser_fuzzer:
	-../build/fuzz/h265_sps_multilayer_extension_parser_fuzzer -artifact_prefix=corpus/h265_sps_multilayer_extension_parser_fuzzer/ corpus/h265_sps_multilayer_extension_parser_fuzzer/ -runs=$(RUNS)

h265_sps_3d_extension_parser_fuzzer:
	-../build/fuzz/h265_sps_3d_extension_parser_fuzzer -artifact_prefix=corpus/h265_sps_3d_extension_parser_fuzzer/ corpus/h265_sps_3d_extension_parser_fuzzer/ -runs=$(RUNS)

h265_sps_range_extension_parser_fuzzer:
	-../build/fuzz/h265_sps_range_extension_parser_fuzzer -artifact_prefix=corpus/h265_sps_range_extension_parser_fuzzer/ corpus/h265_sps_range_extension_parser_fuzzer/ -runs=$(RUNS)

h265_scaling_list_data_parser_fuzzer:
	-../build/fuzz/h265_scaling_list_data_parser_fuzzer -artifact_prefix=corpus/h265_scaling_list_data_parser_fuzzer/ corpus/h265_scaling_list_data_parser_fuzzer/ -runs=$(RUNS)

h265_sps_parser_fuzzer:
	-../build/fuzz/h265_sps_parser_fuzzer -artifact_prefix=corpus/h265_sps_parser_fuzzer/ corpus/h265_sps_parser_fuzzer/ -runs=$(RUNS)

h265_pps_scc_extension_parser_fuzzer:
	-../build/fuzz/h265_pps_scc_extension_parser_fuzzer -artifact_prefix=corpus/h265_pps_scc_extension_parser_fuzzer/ corpus/h265_pps_scc_extension_parser_fuzzer/ -runs=$(RUNS)

h265_pps_parser_fuzzer:
	-../build/fuzz/h265_pps_parser_fuzzer -artifact_prefix=corpus/h265_pps_parser_fuzzer/ corpus/h265_pps_parser_fuzzer/ -runs=$(RUNS)

h265_aud_parser_fuzzer:
	-../build/fuzz/h265_aud_parser_fuzzer -artifact_prefix=corpus/h265_aud_parser_fuzzer/ corpus/h265_aud_parser_fuzzer/ -runs=$(RUNS)

h265_rtp_single_parser_fuzzer:
	-../build/fuzz/h265_rtp_single_parser_fuzzer -artifact_prefix=corpus/h265_rtp_single_parser_fuzzer/ corpus/h265_rtp_single_parser_fuzzer/ -runs=$(RUNS)

h265_rtp_parser_fuzzer:
	-../build/fuzz/h265_rtp_parser_fuzzer -artifact_prefix=corpus/h265_rtp_parser_fuzzer/ corpus/h265_rtp_parser_fuzzer/ -runs=$(RUNS)

h265_rtp_ap_parser_fuzzer:
	-../build/fuzz/h265_rtp_ap_parser_fuzzer -artifact_prefix=corpus/h265_rtp_ap_parser_fuzzer/ corpus/h265_rtp_ap_parser_fuzzer/ -runs=$(RUNS)

h265_rtp_fu_parser_fuzzer:
	-../build/fuzz/h265_rtp_fu_parser_fuzzer -artifact_prefix=corpus/h265_rtp_fu_parser_fuzzer/ corpus/h265_rtp_fu_parser_fuzzer/ -runs=$(RUNS)

h265_slice_parser_fuzzer:
	-../build/fuzz/h265_slice_parser_fuzzer -artifact_prefix=corpus/h265_slice_parser_fuzzer/ corpus/h265_slice_parser_fuzzer/ -runs=$(RUNS)

h265_bitstream_parser_fuzzer:
	-../build/fuzz/h265_bitstream_parser_fuzzer -artifact_prefix=corpus/h265_bitstream_parser_fuzzer/ corpus/h265_bitstream_parser_fuzzer/ -runs=$(RUNS)

h265_nal_unit_parser_fuzzer:
	-../build/fuzz/h265_nal_unit_parser_fuzzer -artifact_prefix=corpus/h265_nal_unit_parser_fuzzer/ corpus/h265_nal_unit_parser_fuzzer/ -runs=$(RUNS)

