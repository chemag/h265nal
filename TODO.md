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
the 101 failing files: many need more than one.

| structure                  | files | where                             |
|----------------------------|-------|-----------------------------------|
| multi-layer sps extension  |   101 | shvc, 3d-hevc, mv-hevc            |
| `pps_3d_extension()`       |    26 | 3d-hevc                           |
| `colour_mapping_table()`   |     9 | shvc, in pps_multilayer_extension |

Notes on each:

* multi-layer sps extension is Section F.7.3.2.2.1, selected when
  `nuh_layer_id > 0` and `sps_ext_or_max_sub_layers_minus1 == 7`. The
  sps then carries no `profile_tier_level()`, and an
  `update_rep_format_flag` in place of the format fields, which it
  inherits from the vps. Every remaining file needs it, and it is the
  largest piece of work left: a second sps syntax path plus vps
  `rep_format` plumbing.
* the other two are larger structures serving one suite each, and both
  sit behind the multi-layer sps extension in every file that needs
  them, so neither unlocks a file on its own.

Per suite, as of the last sweep.

| suite   | files | clean |
|---------|-------|-------|
| hevc_v1 |   147 |   147 |
| rext    |    49 |    49 |
| scc     |    15 |    15 |
| mv-hevc |     9 |     2 |
| 3d-hevc |    27 |     0 |
| shvc    |    70 |     3 |

# 2. other source TODOs

Everything else marked TODO in `src/`, by count:

| n | item                                                             |
|---|------------------------------------------------------------------|
| 2 | `filler_data()`, `end_of_seq()`, `end_of_bitstream()` payloads     |
| 1 | `byte_alignment()`                                                |
| 1 | num_entry_point_offsets, outside the three cases the spec lists    |
| 1 | read the infile incrementally, rather than all of it at once       |
| 1 | move the nal unit type dispatcher into its own function            |
| 1 | ref_loc offsets should be a dictionary, not parallel vectors       |
| 1 | enforce the nal_unit_type check                                    |

Two more that are not marked TODO in the source, because neither is a
defect in one place:

* `RTC_DCHECK` and `RTC_CHECK` are both `assert()`, so every check in the
  bit buffer disappears under `NDEBUG`. A malformed stream aborts a debug
  build and is carried on with in a release one. Two bugs came out of
  this: a rewind that only ran in debug builds, and a shift by a negative
  count that a hang reached first. Both are fixed, the property is not.
* `isValidNumEntryPointOffsets()` takes its sps and pps as `shared_ptr`
  by value and only reads them. A const reference is the right signature
  and clang-tidy asks for it, but the declaration is in a public header,
  so narrowing it changes the mangled name and breaks the ABI. Suppressed
  with the reasoning inline. Worth folding into the next change that
  breaks the ABI for another reason.

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

One trap when comparing parameter sets rather than slices: ffmpeg prints
an `Extradata` block before the first `Packet:` line, holding a copy of
the leading VPS/SPS/PPS that the demuxer lifted out of the stream. That
copy is not a second parameter set in the file, so a naive diff reports
every single-pps stream as a mismatch. Skip everything before the first
`Packet:`.

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
* the sanitizers reach inlined libstdc++, and report it. Every run of the
  h265nal tool emits eight lines from `ext/atomicity.h`, where
  `__exchange_and_add` decrements a `shared_ptr` refcount by adding -1 as
  unsigned, which `implicit-conversion` and `unsigned-integer-overflow`
  both object to. It is correct code and not ours. This blocks the
  conformance sweep, since `tools/h265nal-conformance.py` reads any
  stderr as a failure and would mark all 317 files. Either build the tool
  without the sanitizers or exclude system headers, e.g. with
  `-fsanitize-ignorelist` on `src:*/include/c++/*`. The fuzz targets do
  not report it, so whatever the tool does to trigger it is worth
  understanding before the next campaign.
* the two bounds below are looser than the standard, because the value
  the standard bounds them against lives in the sps and the parser that
  reads them does not resolve it. Both are safe, in that neither can
  overflow the arithmetic downstream, but both accept parameter sets a
  decoder would reject:
  * `num_tile_columns_minus1` and `num_tile_rows_minus1` are bounded by
    the largest picture rather than by `PicWidthInCtbsY` and
    `PicHeightInCtbsY` (Section 7.4.3.3).
  * `luma_bit_depth_entry_minus8` and `chroma_bit_depth_entry_minus8`
    are range checked rather than compared against the sps bit depths
    they are required to equal (Section 7.4.3.3.3). The exact test is
    available here: `ParsePpsSccExtension()` is given those values.
  * `diff_cu_chroma_qp_offset_depth` is bounded by 3 rather than by
    `log2_diff_max_min_luma_coding_block_size`, and
    `log2_sao_offset_scale_luma` and `log2_sao_offset_scale_chroma` by 6
    rather than by `Max(0, BitDepthY - 10)` and `Max(0, BitDepthC - 10)`
    (Section 7.4.3.3.2). The three bounds are what those sps values can
    reach at their most permissive.
