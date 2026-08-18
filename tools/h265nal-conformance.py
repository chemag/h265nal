#!/usr/bin/env python3
"""Summarize a set of H265 bitstreams into a CSV file.

Runs h265nal on each input file and writes one CSV row per file, with
resolution, profile, tier, level, NAL unit counts, and the parser status.

Example:
    $ ./h265nal-conformance.py -o summary.csv /path/to/dataset/*
"""

import argparse
import collections
import csv
import os
import re
import shutil
import signal
import subprocess
import sys


CSV_FIELDS = [
    "input_dir",
    "input_file",
    "resolution",
    "profile",
    "tier",
    "level",
    "number_of_nalus",
    "number_of_slice_nalus",
    "number_of_slice_segment_headers",
    "h265nal_status",
    "slice_header_status",
]

STATUS_OK = "0"

# NAL unit types h265nal is expected to turn into a slice segment header:
# the non-IRAP pictures (0 to 9) and the IRAP ones (16 to 21). The
# reserved VCL types (10 to 15, 22 to 31) are deliberately left out:
# h265nal does not parse them, so counting them would report a shortfall
# that is a missing feature rather than a failure.
SLICE_NAL_UNIT_TYPES = tuple(range(0, 10)) + tuple(range(16, 22))

DEFAULT_TIMEOUT = 60


def get_default_h265nal_path():
    """Find h265nal in $PATH, else fall back to the in-tree build."""
    path = shutil.which("h265nal")
    if path is not None:
        return path
    tools_dir = os.path.dirname(os.path.abspath(__file__))
    path = os.path.join(tools_dir, os.pardir, "build", "tools", "h265nal")
    path = os.path.normpath(path)
    return path if os.path.isfile(path) else None


def run_h265nal(input_path, h265nal_path, timeout):
    """Run h265nal on a file.

    Returns a (stdout, status) tuple. status is STATUS_OK when h265nal ran
    to completion without complaining, else a human-readable description.
    """
    try:
        result = subprocess.run(
            [
                h265nal_path,
                "-i",
                input_path,
                "--as-one-line",
                "--add-resolution",
            ],
            capture_output=True,
            text=True,
            timeout=timeout,
        )
    except subprocess.TimeoutExpired:
        return "", f"timeout after {timeout}s"
    except OSError as error:
        return "", f"could not run h265nal: {error}"

    status = summarize_errors(result.stderr)
    if result.returncode != 0:
        rc_status = format_returncode(result.returncode)
        status = rc_status if status == STATUS_OK else f"{rc_status}; {status}"
    return result.stdout, status


# h265nal tells apart a bitstream it does not support from a broken one
EXIT_CODES = {
    1: "invalid bitstream",
    2: "unimplemented syntax",
}


def format_returncode(returncode):
    """Describe a non-zero h265nal exit, including deaths by signal."""
    if returncode < 0:
        name = signal.Signals(-returncode).name
        return f"h265nal killed by {name}"
    reason = EXIT_CODES.get(returncode)
    if reason is not None:
        return f"h265nal: {reason}"
    return f"h265nal exited with {returncode}"


def summarize_errors(stderr):
    """Collapse h265nal's stderr into a single CSV-friendly status string.

    Parse errors repeat once per NAL unit (a rejected SPS yields one
    "non-existent SPS id" line per following slice), so identical messages
    are deduplicated and counted instead of being copied verbatim.
    """
    messages = [line.strip() for line in stderr.splitlines() if line.strip()]
    if not messages:
        return STATUS_OK
    counts = collections.Counter(messages)
    return "; ".join(
        message if count == 1 else f"{message} (x{count})"
        for message, count in counts.items()
    )


def get_int_field(text, name):
    match = re.search(rf"\b{name}: (-?\d+)", text)
    return int(match.group(1)) if match else None


def format_level(general_level_idc):
    """Convert general_level_idc into a level number (Section A.4.1).

    general_level_idc is thirty times the level number, so level 4.1 is
    signalled as 123.
    """
    if general_level_idc is None:
        return None
    major, remainder = divmod(general_level_idc, 30)
    if remainder % 3 != 0:
        # not one of the level numbers in Table A.8: keep the raw value
        return f"level_idc:{general_level_idc}"
    return f"{major}.{remainder // 3}"


def format_tier(general_tier_flag):
    """Convert the general profile_tier_level tier_flag into a tier name."""
    if general_tier_flag is None:
        return None
    return "High" if general_tier_flag == 1 else "Main"


def parse_sps_lines(stdout):
    """Extract (resolution, profile, tier, level) from every SPS.

    A bitstream may carry several SPSs (resolution changes, layered
    extensions), so each value is returned as an ordered list of unique
    entries.
    """
    resolutions = []
    profiles = []
    tiers = []
    levels = []

    for line in stdout.splitlines():
        if " sps {" not in line:
            continue

        # profile_tier_level() starts with the general_* fields, and the
        # optional sub_layer_* copies follow, so the first match is general
        profile_match = re.search(
            r"\bprofile: (.+?) progressive_source_flag:", line
        )
        profile = profile_match.group(1).strip() if profile_match else None
        tier_flag = get_int_field(line, "tier_flag")
        level_idc = get_int_field(line, "general_level_idc")
        # --add-resolution appends the conformance-window-cropped size
        width = get_int_field(line, "width")
        height = get_int_field(line, "height")

        if width is not None and height is not None:
            add_unique(resolutions, f"{width}x{height}")
        add_unique(profiles, profile)
        add_unique(tiers, format_tier(tier_flag))
        add_unique(levels, format_level(level_idc))

    return resolutions, profiles, tiers, levels


def add_unique(values, value):
    if value is not None and value not in values:
        values.append(value)


def count_slice_nalus(stdout):
    """Count the NAL units that should each yield one slice segment header."""
    types = "|".join(str(t) for t in SLICE_NAL_UNIT_TYPES)
    return len(re.findall(rf"\bnal_unit_type: (?:{types})\b", stdout))


def get_slice_header_status(slice_nalus, slice_headers):
    """Flag slice segment headers that went missing without h265nal saying so.

    A slice NAL unit that yields no slice segment header was rejected
    somewhere. The parser usually explains itself on stderr, but not
    always: a sub-parser can return nullptr silently and the NAL unit is
    then dumped with an empty payload. Comparing the two counts catches
    that, which stderr alone does not. Kept out of h265nal_status so that
    column stays a faithful record of what h265nal itself reported.
    """
    if slice_headers == slice_nalus:
        return STATUS_OK
    return f"unparsed slice segment headers: {slice_headers}/{slice_nalus}"


def analyze_file(input_path, h265nal_path, timeout):
    """Build the CSV row for a single input file."""
    stdout, status = run_h265nal(input_path, h265nal_path, timeout)
    resolutions, profiles, tiers, levels = parse_sps_lines(stdout)
    slice_nalus = count_slice_nalus(stdout)
    slice_headers = stdout.count("slice_segment_header {")

    return {
        # the containing directory only, not the whole path: the full path
        # is long, differs per machine, and is the same for every row of a
        # run. For the JCT-VC suite the directory is the suite name.
        "input_dir": os.path.basename(os.path.dirname(
            os.path.abspath(input_path))),
        "input_file": os.path.basename(input_path),
        "resolution": ";".join(resolutions),
        "profile": ";".join(profiles),
        "tier": ";".join(tiers),
        "level": ";".join(levels),
        # in one-line mode there is exactly one "nal_unit {" per NAL unit,
        # and one "slice_segment_header {" per parsed slice segment header
        "number_of_nalus": stdout.count("nal_unit {"),
        "number_of_slice_nalus": slice_nalus,
        "number_of_slice_segment_headers": slice_headers,
        "h265nal_status": status,
        "slice_header_status": get_slice_header_status(slice_nalus,
                                                       slice_headers),
    }


def get_options(argv):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "infiles",
        nargs="+",
        help="H265 files to parse",
    )
    parser.add_argument(
        "-o",
        "--output",
        dest="outfile",
        default=None,
        help="CSV output file [default: stdout]",
    )
    parser.add_argument(
        "--h265nal",
        dest="h265nal_path",
        default=get_default_h265nal_path(),
        help="path to the h265nal binary [default: %(default)s]",
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=DEFAULT_TIMEOUT,
        help="per-file timeout, in seconds [default: %(default)s]",
    )
    parser.add_argument(
        "-q",
        "--quiet",
        action="store_true",
        help="do not print per-file progress",
    )
    return parser.parse_args(argv[1:])


def main(argv):
    options = get_options(argv)

    if options.h265nal_path is None:
        print("error: cannot find h265nal, use --h265nal", file=sys.stderr)
        sys.exit(1)

    # a shell glob may hand us directories: parse regular files only
    infiles = [name for name in options.infiles if not os.path.isdir(name)]

    rows = []
    for index, input_path in enumerate(infiles, start=1):
        if not options.quiet:
            print(f"[{index}/{len(infiles)}] {input_path}", file=sys.stderr)
        rows.append(analyze_file(input_path, options.h265nal_path,
                                 options.timeout))

    if options.outfile is None:
        write_csv(sys.stdout, rows)
    else:
        with open(options.outfile, "w", newline="") as fout:
            write_csv(fout, rows)

    if not options.quiet:
        failed = sum(1 for row in rows
                     if row["h265nal_status"] != STATUS_OK)
        # files losing slice segment headers without h265nal reporting
        # anything: worth calling out separately, as they look clean
        # otherwise
        silent = sum(1 for row in rows
                     if row["h265nal_status"] == STATUS_OK
                     and row["slice_header_status"] != STATUS_OK)
        print(f"parsed {len(rows)} file(s), {failed} with issues, "
              f"{silent} silently missing slice segment headers",
              file=sys.stderr)


def write_csv(fout, rows):
    writer = csv.DictWriter(fout, fieldnames=CSV_FIELDS)
    writer.writeheader()
    writer.writerows(rows)


if __name__ == "__main__":
    main(sys.argv)
