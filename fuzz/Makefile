



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
    h265_pps_multilayer_extension_parser_fuzzer.cc \
    h265_pps_scc_extension_parser_fuzzer.cc \
    h265_pps_parser_fuzzer.cc \
    h265_aud_parser_fuzzer.cc \
    h265_sei_parser_fuzzer.cc \
    h265_rtp_single_parser_fuzzer.cc \
    h265_rtp_parser_fuzzer.cc \
    h265_rtp_ap_parser_fuzzer.cc \
    h265_rtp_fu_parser_fuzzer.cc \
    h265_slice_parser_fuzzer.cc \
    h265_bitstream_parser_fuzzer.cc \
    h265_nal_unit_parser_fuzzer.cc \
    h265_configuration_box_parser_fuzzer.cc
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

h265_pps_multilayer_extension_parser_fuzzer.cc: ../test/h265_pps_multilayer_extension_parser_unittest.cc
	./converter.py ../test/h265_pps_multilayer_extension_parser_unittest.cc ./

h265_pps_scc_extension_parser_fuzzer.cc: ../test/h265_pps_scc_extension_parser_unittest.cc
	./converter.py ../test/h265_pps_scc_extension_parser_unittest.cc ./

h265_pps_parser_fuzzer.cc: ../test/h265_pps_parser_unittest.cc
	./converter.py ../test/h265_pps_parser_unittest.cc ./

h265_aud_parser_fuzzer.cc: ../test/h265_aud_parser_unittest.cc
	./converter.py ../test/h265_aud_parser_unittest.cc ./

h265_sei_parser_fuzzer.cc: ../test/h265_sei_parser_unittest.cc
	./converter.py ../test/h265_sei_parser_unittest.cc ./

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

h265_configuration_box_parser_fuzzer.cc: ../test/h265_configuration_box_parser_unittest.cc
	./converter.py ../test/h265_configuration_box_parser_unittest.cc ./


RUNS ?= 1000000

# Extra libFuzzer flags. Fork mode by default: without it libFuzzer exits
# on the first finding, so one bug ends that target's run. The cost is that
# -runs stops being exact, because the parent only checks the total at job
# boundaries; expect it to overshoot by 3-30x. When the run count has to be
# exact, turn it off with FUZZ_FLAGS= on the command line.
FUZZ_FLAGS ?= -fork=1 -ignore_crashes=1

FUZZERS = \
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
    h265_sei_parser_fuzzer \
    h265_rtp_single_parser_fuzzer \
    h265_rtp_parser_fuzzer \
    h265_rtp_ap_parser_fuzzer \
    h265_rtp_fu_parser_fuzzer \
    h265_slice_parser_fuzzer \
    h265_bitstream_parser_fuzzer \
    h265_nal_unit_parser_fuzzer \
    h265_configuration_box_parser_fuzzer

.PHONY: fuzz $(FUZZERS)

fuzz: $(FUZZERS)

# Failing inputs go to artifacts/, not into corpus/. libFuzzer executes
# every file in the corpus directory at startup, and separately writes new
# coverage-increasing inputs back into it, so pointing -artifact_prefix at
# that same directory means a target that crashes once replays its own
# crash as a seed and never starts again. Keeping them apart leaves corpus/
# as the growing input set and artifacts/ as the list of things to act on.
# The mkdir deliberately has no - prefix: libFuzzer refuses to start when
# the artifact directory is missing, so failing here is better.
$(FUZZERS):
	mkdir -p artifacts/$@
	-../build/fuzz/$@ -artifact_prefix=artifacts/$@/ corpus/$@/ -runs=$(RUNS) $(FUZZ_FLAGS)
