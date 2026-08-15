---
title: "h265nal TODO list"
---

# 1. unimplemented syntax structures

These are the parsers that bail out through `report_unimplemented()`, so
h265nal says what it does not support and exits with code 2
("unimplemented syntax") rather than pretending to have parsed the
stream.

The file counts come from the 201810 conformance suite, swept with
`tools/h265nal-conformance.py` (see `conformance/Makefile`). A file is
counted once per structure it needs, so the column does not add up to
the 132 failing files: many need more than one.

| structure                  | files | where                             |
|----------------------------|-------|-----------------------------------|
| multi-layer sps extension  |   101 | shvc, 3d-hevc, mv-hevc            |
| `pps_range_extension()`    |    31 | rext, scc                         |
| `pps_3d_extension()`       |    26 | 3d-hevc                           |
| `colour_mapping_table()`   |     9 | shvc, in pps_multilayer_extension |

Notes on each:

* multi-layer sps extension is Section F.7.3.2.2.1, selected when
  `nuh_layer_id > 0` and `sps_ext_or_max_sub_layers_minus1 == 7`. The
  sps then carries no `profile_tier_level()`, and an
  `update_rep_format_flag` in place of the format fields, which it
  inherits from the vps. Biggest gap by file count, and the largest
  piece of work: a second sps syntax path plus vps `rep_format`
  plumbing.
* `pps_range_extension()` is Section 7.3.2.3.2, a small flat structure
  comparable to the `pps_scc_extension()` that already exists. Best
  ratio of files unlocked to work. It also retires the hardcoded
  `chroma_qp_offset_list_enabled_flag = 0` in the slice segment header,
  which is a latent desync for any stream that sets it.
* the other two are larger structures serving one suite each.

Per suite, as of the last sweep:

| suite   | files | clean |
|---------|-------|-------|
| hevc_v1 |   147 |   147 |
| rext    |    49 |    30 |
| scc     |    15 |     3 |
| mv-hevc |     9 |     2 |
| 3d-hevc |    27 |     0 |
| shvc    |    70 |     3 |

# 2. other source TODOs

Everything else marked TODO in `src/`, by count:

| n | item                                                             |
|---|------------------------------------------------------------------|
| 2 | `ref_pic_lists_modification()`                                    |
| 2 | `filler_data()`, `end_of_seq()`, `end_of_bitstream()` payloads     |
| 1 | `byte_alignment()`                                                |
| 1 | `NumPicTotalCurr` (page 99), which the above is gated on           |
| 1 | num_entry_point_offsets, outside the three cases the spec lists    |
| 1 | read the infile incrementally, rather than all of it at once       |
| 1 | move the nal unit type dispatcher into its own function            |
| 1 | ref_loc offsets should be a dictionary, not parallel vectors       |
| 1 | enforce the nal_unit_type check                                    |

`ref_pic_lists_modification()` is worth calling out: it is gated behind
`NumPicTotalCurr`, which is hardcoded to 0, so the structure is never
reached and never read. Any stream carrying one desyncs from there.

# 3. comparison with ffmpeg BSF

Cross-checking against ffmpeg's bitstream filter is how the slice
segment header bugs of 2026-08 were found. It prints every syntax
element with its bit offset, so the first field where the two disagree
is the bug.

```
$ ffmpeg -v trace -bsf:v trace_headers -i in.265 -f null - 2> ffmpeg.txt
$ ./build/tools/h265nal -i in.265 --no-as-one-line > h265nal.txt
```

To make the ffmpeg side diffable against ours:

```
:%s/^\[[^\]]*\] [0-9]*  */    /
:%s/^\[[^\]]*\] [0-9]* *//
:%s/    *[0-1]* = /: /
```

then meld.

Worth automating: a script that aligns the two per slice segment header
and reports the first differing field turns this from an afternoon into
a command.

# 4. fuzzing

See `fuzz/Makefile`. Open items:

* the corpus is only distilled by hand, with `make merge`. Nothing runs
  it on a schedule, so it grows without bound between merges.
* the fuzz targets are not built by `make` nor exercised by `make test`,
  so a change that breaks one is only noticed when somebody builds with
  `-DBUILD_CLANG_FUZZER=ON`.
* `converter.py` emits a fuzz target for any unit test, including ones
  with no `fuzzer::conv` markers, which yields a target that ignores its
  input. `h265_common__unescape_rbsp_fuzzer.cc` is hand written for that
  reason.
