# The recipes below pipe into tee, and a pipeline reports the status of
# its last command, so without pipefail a crashing fuzzer would look like
# a success. pipefail is not in POSIX sh, hence bash. This applies to
# every recipe in this file.
SHELL := /bin/bash
.SHELLFLAGS := -o pipefail -c





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
# on the first finding, so one bug ends that target's run. Note that either
# way a crash only ends that one target, never the campaign, because the
# recipe below is prefixed with -.
#
# The cost is that -runs stops being exact, because the parent only checks
# the total at job boundaries; expect it to overshoot by 3-30x. When the run
# count has to be exact, turn it off with FUZZ_FLAGS= on the command line.
#
# Use FUZZ_FLAGS= as well whenever a target is new, or already known to be
# crashing, and the point of the run is to get a reproducer rather than
# coverage:
#
#   * Fork mode buys the rest of the budget on a target that has already
#     failed, so it finds more.
#   * It pays for that in artifacts: one bug can produce many, all replaying
#     to one site, so triage means replaying each and grouping on the
#     failure location.
#   * Worse for a reproducer, a crash in the fork parent, before any child
#     runs, prints its AddressSanitizer report and exits without writing an
#     artifact at all. The input is then gone: the seed libFuzzer prints is
#     the parent's, and replaying it does not reproduce a child's work.
#
# So: fork for soaks, FUZZ_FLAGS= when chasing something.
FUZZ_FLAGS ?= -fork=1 -ignore_crashes=1

# libFuzzer picks a random seed per run and prints it. A finding that does
# not become an artifact is only reproducible if that seed is known, so it
# is worth reading out of the log and replaying with SEED=<n>. Empty by
# default, which leaves libFuzzer's own randomness in place.
SEED ?=
SEED_FLAG = $(if $(SEED),-seed=$(SEED),)

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

.PHONY: fuzz merge $(FUZZERS)

fuzz: $(FUZZERS)

# Failing inputs go to artifacts/, not into corpus/. libFuzzer executes
# every file in the corpus directory at startup, and separately writes new
# coverage-increasing inputs back into it, so pointing -artifact_prefix at
# that same directory means a target that crashes once replays its own
# crash as a seed and never starts again. Keeping them apart leaves corpus/
# as the growing input set and artifacts/ as the list of things to act on.
# Neither mkdir has a - prefix: libFuzzer refuses to start when either the
# artifact directory or the corpus directory is missing, reporting 'ERROR:
# The required directory "..." does not exist', so failing here is better.
# corpus/$@ is created rather than assumed because a target added to
# FUZZERS has no seeds until its first run.
# Everything is teed to logs/$@.log. artifacts/ is not sufficient on its
# own: in fork mode a crash in the parent, before any child runs, prints
# an AddressSanitizer report and exits without writing an artifact, so the
# run looks clean by every other measure. The log is the only place it
# appears.
$(FUZZERS):
	mkdir -p artifacts/$@ corpus/$@ logs
	-../build/fuzz/$@ -artifact_prefix=artifacts/$@/ corpus/$@/ \
	    -runs=$(RUNS) $(SEED_FLAG) $(FUZZ_FLAGS) 2>&1 | tee logs/$@.log

# What did the last campaign find? Reads the logs, not just artifacts/,
# for the reason above.
.PHONY: fuzz-report
fuzz-report:
	@grep -lE '^==[0-9]+==ERROR|runtime error:|ERROR: libFuzzer' logs/*.log 2>/dev/null \
	    || echo 'logs: no findings'
	@ls artifacts/*/* 2>/dev/null || echo 'artifacts: none'

# Distill each corpus in place. libFuzzer's -merge=1 writes survivors into a
# destination directory under their content hash, so swapping directories
# would rename the unittest.*.bin seeds and any crash-* kept as a regression
# input. Merge to a scratch directory instead and delete only the
# content-addressed blobs that did not survive, leaving named files alone.
# The result is a pure set of deletions, reviewable with git diff in the
# corpus submodule before committing.
merge: $(FUZZERS:%=merge-%)

# The survivorship test reads "absent from the scratch directory" as "did
# not survive", so anything that stops the merge early looks like a corpus
# with no survivors and takes the whole thing with it. A crashing input is
# enough to cause that, and UBSan findings abort by design since a1a029ab.
# So check that the merge succeeded and produced something before deleting
# anything, and leave the corpus alone otherwise.
# -artifact_prefix matters here too. A merge replays every input in the
# corpus, so it finds anything already in there that the current build
# aborts on, and without a prefix libFuzzer writes those crash-<sha> files
# into the working directory, i.e. loose in fuzz/. Send them to the same
# artifacts/<target>/ the fuzz runs use.
merge-%:
	@mkdir -p artifacts/$*
	@tmp=$$(mktemp -d); before=$$(ls -1 corpus/$* | wc -l); \
	if ! ../build/fuzz/$* -merge=1 -merge_control_file=$$tmp/.ctl \
	        -artifact_prefix=artifacts/$*/ $$tmp corpus/$* \
	        >$$tmp/.log 2>&1; then \
		echo "$*: merge failed, corpus untouched"; \
		tail -3 $$tmp/.log | sed 's/^/    /'; \
		rm -rf $$tmp; exit 0; \
	fi; \
	if [ "$$(ls -1 $$tmp | wc -l)" -eq 0 ]; then \
		echo "$*: merge produced no survivors, corpus untouched"; \
		rm -rf $$tmp; exit 0; \
	fi; \
	for f in corpus/$*/*; do \
		b=$$(basename $$f); \
		echo "$$b" | grep -qE '^[0-9a-f]{40}$$' || continue; \
		[ -e "$$tmp/$$b" ] || rm -f "$$f"; \
	done; \
	rm -rf $$tmp; \
	echo "$*: $$before -> $$(ls -1 corpus/$* | wc -l) files"
