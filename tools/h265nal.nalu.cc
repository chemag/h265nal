/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 *
 * An h265 (HEVC) NALU Parser. It gets a series of NALU units, and sends them
 * one by one to a parser (`H265NalUnitParser::ParseNalUnitUnescaped`).
 * Note that the original file read is a full Annex-B file, which we read
 * into a buffer. We also get the NALU indices of that buffer using an
 * h265nal-provided function.
 * It then dumps the contents of each NALU read.
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "config.h"
#include "h265_bitstream_parser.h"
#include "h265_common.h"
#include "rtc_base/bit_buffer.h"

extern int optind;

typedef struct arg_options {
  int debug;
  bool as_one_line;
  bool add_offset;
  bool add_length;
  bool add_parsed_length;
  bool add_checksum;
  bool add_contents;
  char *infile;
  char *outfile;
} arg_options;

// default option values
arg_options DEFAULTS{
    .debug = 0,
    .as_one_line = true,
    .add_offset = false,
    .add_length = false,
    .add_parsed_length = false,
    .add_checksum = false,
    .add_contents = false,
    .infile = nullptr,
    .outfile = nullptr,
};

void usage(char *name) {
  fprintf(stderr, "usage: %s [options]\n", name);
  fprintf(stderr, "where options are:\n");
  fprintf(stderr, "\t-d:\t\tIncrease debug verbosity [default: %i]\n",
          DEFAULTS.debug);
  fprintf(stderr, "\t-q:\t\tZero debug verbosity\n");
  fprintf(stderr, "\t--as-one-line:\tSet as_one_line flag%s\n",
          DEFAULTS.as_one_line ? " [default]" : "");
  fprintf(stderr, "\t--noas-one-line:\tReset as_one_line flag%s\n",
          !DEFAULTS.as_one_line ? " [default]" : "");
  fprintf(stderr, "\t--add-offset:\tSet add_offset flag%s\n",
          DEFAULTS.add_offset ? " [default]" : "");
  fprintf(stderr, "\t--noadd-offset:\tReset add_offset flag%s\n",
          !DEFAULTS.add_offset ? " [default]" : "");
  fprintf(stderr, "\t--add-length:\tSet add_length flag%s\n",
          DEFAULTS.add_length ? " [default]" : "");
  fprintf(stderr, "\t--noadd-length:\tReset add_length flag%s\n",
          !DEFAULTS.add_length ? " [default]" : "");
  fprintf(stderr, "\t--add-parsed-length:\tSet add_parsed_length flag%s\n",
          DEFAULTS.add_parsed_length ? " [default]" : "");
  fprintf(stderr, "\t--noadd-parsed-length:\tReset add_parsed_length flag%s\n",
          !DEFAULTS.add_parsed_length ? " [default]" : "");
  fprintf(stderr, "\t--add-checksum:\tSet add_checksum flag%s\n",
          DEFAULTS.add_checksum ? " [default]" : "");
  fprintf(stderr, "\t--noadd-checksum:\tReset add_checksum flag%s\n",
          !DEFAULTS.add_checksum ? " [default]" : "");
  fprintf(stderr, "\t--add-contents:\tSet add_contents flag%s\n",
          DEFAULTS.add_contents ? " [default]" : "");
  fprintf(stderr, "\t--noadd-contents:\tReset add_contents flag%s\n",
          !DEFAULTS.add_contents ? " [default]" : "");
  fprintf(stderr, "\t--version:\t\tDump version number\n");
  fprintf(stderr, "\t-h:\t\tHelp\n");
  exit(-1);
}

// long options with no equivalent short option
enum {
  QUIET_OPTION = CHAR_MAX + 1,
  AS_ONE_LINE_FLAG_OPTION,
  NO_AS_ONE_LINE_FLAG_OPTION,
  ADD_OFFSET_FLAG_OPTION,
  NO_ADD_OFFSET_FLAG_OPTION,
  ADD_LENGTH_FLAG_OPTION,
  NO_ADD_LENGTH_FLAG_OPTION,
  ADD_PARSED_LENGTH_FLAG_OPTION,
  NO_ADD_PARSED_LENGTH_FLAG_OPTION,
  ADD_CHECKSUM_FLAG_OPTION,
  NO_ADD_CHECKSUM_FLAG_OPTION,
  ADD_CONTENTS_FLAG_OPTION,
  NO_ADD_CONTENTS_FLAG_OPTION,
  VERSION_OPTION,
  HELP_OPTION
};

arg_options *parse_args(int argc, char **argv) {
  int c;
  static arg_options options;

  // set default options
  options = DEFAULTS;

  // getopt_long stores the option index here
  int optindex = 0;

  // long options
  static struct option longopts[] = {
      // matching options to short options
      {"debug", no_argument, NULL, 'd'},
      // options without a short option
      {"quiet", no_argument, NULL, QUIET_OPTION},
      {"as-one-line", no_argument, NULL, AS_ONE_LINE_FLAG_OPTION},
      {"noas-one-line", no_argument, NULL, NO_AS_ONE_LINE_FLAG_OPTION},
      {"add-offset", no_argument, NULL, ADD_OFFSET_FLAG_OPTION},
      {"noadd-offset", no_argument, NULL, NO_ADD_OFFSET_FLAG_OPTION},
      {"add-length", no_argument, NULL, ADD_LENGTH_FLAG_OPTION},
      {"noadd-length", no_argument, NULL, NO_ADD_LENGTH_FLAG_OPTION},
      {"add-parsed-length", no_argument, NULL, ADD_PARSED_LENGTH_FLAG_OPTION},
      {"noadd-parsed-length", no_argument, NULL,
       NO_ADD_PARSED_LENGTH_FLAG_OPTION},
      {"add-checksum", no_argument, NULL, ADD_CHECKSUM_FLAG_OPTION},
      {"noadd-checksum", no_argument, NULL, NO_ADD_CHECKSUM_FLAG_OPTION},
      {"add-contents", no_argument, NULL, ADD_CONTENTS_FLAG_OPTION},
      {"noadd-contents", no_argument, NULL, NO_ADD_CONTENTS_FLAG_OPTION},
      {"version", no_argument, NULL, VERSION_OPTION},
      {"help", no_argument, NULL, HELP_OPTION},
      {NULL, 0, NULL, 0}};

  // parse arguments
  while ((c = getopt_long(argc, argv, "dh", longopts, &optindex)) != -1) {
    switch (c) {
      case 0:
        // long options that define flag
        // if this option set a flag, do nothing else now
        if (longopts[optindex].flag != NULL) {
          break;
        }
        printf("option %s", longopts[optindex].name);
        if (optarg) {
          printf(" with arg %s", optarg);
        }
        break;

      case 'd':
        options.debug += 1;
        break;

      case QUIET_OPTION:
        options.debug = 0;
        break;

      case AS_ONE_LINE_FLAG_OPTION:
        options.as_one_line = true;
        break;

      case NO_AS_ONE_LINE_FLAG_OPTION:
        options.as_one_line = false;
        break;

      case ADD_OFFSET_FLAG_OPTION:
        options.add_offset = true;
        break;

      case NO_ADD_OFFSET_FLAG_OPTION:
        options.add_offset = false;
        break;

      case ADD_LENGTH_FLAG_OPTION:
        options.add_length = true;
        break;

      case NO_ADD_LENGTH_FLAG_OPTION:
        options.add_length = false;
        break;

      case ADD_PARSED_LENGTH_FLAG_OPTION:
        options.add_parsed_length = true;
        break;

      case NO_ADD_PARSED_LENGTH_FLAG_OPTION:
        options.add_parsed_length = false;
        break;

      case ADD_CHECKSUM_FLAG_OPTION:
        options.add_checksum = true;
        break;

      case NO_ADD_CHECKSUM_FLAG_OPTION:
        options.add_checksum = false;
        break;

      case ADD_CONTENTS_FLAG_OPTION:
        options.add_contents = true;
        break;

      case NO_ADD_CONTENTS_FLAG_OPTION:
        options.add_contents = false;
        break;

      case VERSION_OPTION:
        printf("version: %s\n", PROJECT_VER);
        exit(0);
        break;

      case HELP_OPTION:
      case 'h':
        usage(argv[0]);

      default:
        printf("Unsupported option: %c\n", c);
        usage(argv[0]);
    }
  }

  // require 2 extra parameters
  if ((argc - optind != 1) && (argc - optind != 2)) {
    fprintf(stderr, "need infile (outfile is optional)\n");
    usage(argv[0]);
    return nullptr;
  }

  options.infile = argv[optind];
  if (argc - optind == 2) {
    options.outfile = argv[optind + 1];
  }
  return &options;
}

int main(int argc, char **argv) {
  arg_options *options;

#ifdef SMALL_FOOTPRINT
  printf("h265nal: small footprint version\n");
#else
  printf("h265nal: original version\n");
#endif
  // parse args
  options = parse_args(argc, argv);
  if (options == nullptr) {
    usage(argv[0]);
    exit(-1);
  }

  // print args
  if (options->debug > 1) {
    printf("options->debug = %i\n", options->debug);
    printf("options->infile = %s\n",
           (options->infile == NULL) ? "null" : options->infile);
    printf("options->outfile = %s\n",
           (options->outfile == NULL) ? "null" : options->outfile);
  }

  // add_contents requires add_length and add_offset
  if (options->add_contents) {
    options->add_offset = true;
    options->add_length = true;
  }

  // 1. read infile into buffer
  // TODO(chemag): read the infile incrementally
  FILE *infp = fopen(options->infile, "rb");
  if (infp == nullptr) {
    // did not work
    fprintf(stderr, "Could not open input file: \"%s\"\n", options->infile);
    return -1;
  }
  fseek(infp, 0, SEEK_END);
  int64_t size = ftell(infp);
  fseek(infp, 0, SEEK_SET);
  // read file into buffer
  std::vector<uint8_t> buffer(size);
  fread(reinterpret_cast<char *>(buffer.data()), 1, size, infp);
  uint8_t *data = buffer.data();
  size_t length = buffer.size();

  // 2. get the indices for the NALUs in the stream. This is needed
  // because we will read Annex-B files, i.e., a bunch of appended NALUs
  // with escape sequences used to separate them.
  auto nalu_indices =
      h265nal::H265BitstreamParser::FindNaluIndices(data, length);

  // 3. create state for parsing NALUs
  // bitstream parser state (to keep the SPS/PPS/SubsetSPS NALUs)
  h265nal::H265BitstreamParserState bitstream_parser_state;

  // 4. parse the NALUs one-by-one
  auto bitstream =
      std::make_unique<h265nal::H265BitstreamParser::BitstreamState>();
  for (const auto &nalu_index : nalu_indices) {
    // 4.1. parse 1 NAL unit
    // note: If the NALU comes from an unescaped bitstreams, i.e.,
    // one with an explicit NALU length mechanism (like mp4 mdat
    // boxes), the right function is `ParseNalUnitUnescaped()`.
    auto nal_unit = h265nal::H265NalUnitParser::ParseNalUnit(
        &data[nalu_index.payload_start_offset], nalu_index.payload_size,
        &bitstream_parser_state, true /* add_checksum */);
    if (nal_unit == nullptr) {
      // cannot parse the NalUnit
#ifdef FPRINT_ERRORS
      fprintf(stderr, "error: cannot parse buffer into NalUnit\n");
#endif  // FPRINT_ERRORS
      continue;
    }
    nal_unit->offset = nalu_index.payload_start_offset;
    nal_unit->length = nalu_index.payload_size;
    // 4.2. print a given value
    printf(
        "nal_unit { offset: %lu length: %lu parsed_length: %lu checksum: 0x%s "
        "} nal_unit_header { forbidden_zero_bit: %i nal_unit_type: %i "
        "nuh_layer_id: %i nuh_temporal_id_plus1: %i }\n",
        nal_unit->offset, nal_unit->length, nal_unit->parsed_length,
        nal_unit->checksum->GetPrintableChecksum(),
        nal_unit->nal_unit_header->forbidden_zero_bit,
        nal_unit->nal_unit_header->nal_unit_type,
        nal_unit->nal_unit_header->nuh_layer_id,
        nal_unit->nal_unit_header->nuh_temporal_id_plus1);

    // 4.3. store the parsed NAL unit
    bitstream->nal_units.push_back(std::move(nal_unit));
  }

  // 5. clean up
  // auto bitstream = std::make_unique<BitstreamState>();

  return 0;
}
