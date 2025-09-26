/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 *
 * An HEVC NALU (Annex-B) Parser. It can operate in 2x modes, depending on
 * the nalu_length_bytes parameter.
 * (1) If the parameter is negative (default), it assumes a full h265 (HEVC)
 *   Annex-B file with explicit NALU separators. In that case, it parses the
 *   content using a single function (`H265BitstreamParser::ParseBitstream()`).
 * (2) If the parameter is zero, it assumes a single NALU. It parses it.
 * (3) If the parameter is greater than zero, it assumes a full h265 (HEVC)
 *   Annex-B file with explicit NALU length bytes.
 * In all cases, it returns a vector of NALUs read. The file dumpts the
 * contents of each NALU read.
 */

#if defined WIN32 || defined _WIN32 || defined __CYGWIN__
#include "ya_getopt.h"
#else
#include <getopt.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "config.h"
#include "h265_bitstream_parser.h"
#include "h265_common.h"
#include "h265_configuration_box_parser.h"
#include "h265_utils.h"
#include "rtc_common.h"

extern int optind;

enum Dumpmode { dump_all, dump_length };

typedef struct arg_options {
  int debug;
  Dumpmode dumpmode;
  bool as_one_line;
  bool add_offset;
  bool add_length;
  bool add_parsed_length;
  bool add_checksum;
  bool add_resolution;
  bool add_contents;
  int nalu_length_bytes;
  int frames_per_second;
  char *hvcc_file;
  char *infile;
  char *outfile;
} arg_options;

// default option values
arg_options DEFAULT_OPTIONS{
    .debug = 0,
    .dumpmode = dump_all,
    .as_one_line = true,
    .add_offset = false,
    .add_length = false,
    .add_parsed_length = false,
    .add_checksum = false,
    .add_resolution = false,
    .add_contents = false,
    .nalu_length_bytes = -1,
    .frames_per_second = 30,
    .hvcc_file = nullptr,
    .infile = nullptr,
    .outfile = nullptr,
};

void usage(char *name) {
  fprintf(stderr, "usage: %s [options]\n", name);
  fprintf(stderr, "where options are:\n");
  fprintf(stderr, "\t-d:\t\tIncrease debug verbosity [default: %i]\n",
          DEFAULT_OPTIONS.debug);
  fprintf(stderr, "\t-q:\t\tZero debug verbosity\n");
  fprintf(stderr, "\t-i <infile>:\t\tH265 file to parse [default: stdin]\n");
  fprintf(stderr,
          "\t--hvcc-file <infile>:\t\thvcC file to parse bitstream state from "
          "[default: none]\n");
  fprintf(stderr, "\t-o <output>:\t\tH265 parsing output [default: stdout]\n");
  fprintf(stderr, "\t--dump-all\t\tDump all the parsed contents\n");
  fprintf(stderr, "\t--dump-length\t\tDump only the length information\n");
  fprintf(stderr, "\t--as-one-line:\tSet as_one_line flag%s\n",
          DEFAULT_OPTIONS.as_one_line ? " [default]" : "");
  fprintf(stderr, "\t--no-as-one-line:\tReset as_one_line flag%s\n",
          !DEFAULT_OPTIONS.as_one_line ? " [default]" : "");
  fprintf(stderr, "\t--add-offset:\tSet add_offset flag%s\n",
          DEFAULT_OPTIONS.add_offset ? " [default]" : "");
  fprintf(stderr, "\t--no-add-offset:\tReset add_offset flag%s\n",
          !DEFAULT_OPTIONS.add_offset ? " [default]" : "");
  fprintf(stderr, "\t--add-length:\tSet add_length flag%s\n",
          DEFAULT_OPTIONS.add_length ? " [default]" : "");
  fprintf(stderr, "\t--no-add-length:\tReset add_length flag%s\n",
          !DEFAULT_OPTIONS.add_length ? " [default]" : "");
  fprintf(stderr, "\t--add-parsed-length:\tSet add_parsed_length flag%s\n",
          DEFAULT_OPTIONS.add_parsed_length ? " [default]" : "");
  fprintf(stderr, "\t--no-add-parsed-length:\tReset add_parsed_length flag%s\n",
          !DEFAULT_OPTIONS.add_parsed_length ? " [default]" : "");
  fprintf(stderr, "\t--add-checksum:\tSet add_checksum flag%s\n",
          DEFAULT_OPTIONS.add_checksum ? " [default]" : "");
  fprintf(stderr, "\t--no-add-checksum:\tReset add_checksum flag%s\n",
          !DEFAULT_OPTIONS.add_checksum ? " [default]" : "");
  fprintf(stderr, "\t--add-resolution:\tSet add_resolution flag%s\n",
          DEFAULT_OPTIONS.add_resolution ? " [default]" : "");
  fprintf(stderr, "\t--no-add-resolution:\tReset add_resolution flag%s\n",
          !DEFAULT_OPTIONS.add_resolution ? " [default]" : "");
  fprintf(stderr, "\t--add-contents:\tSet add_contents flag%s\n",
          DEFAULT_OPTIONS.add_contents ? " [default]" : "");
  fprintf(stderr, "\t--no-add-contents:\tReset add_contents flag%s\n",
          !DEFAULT_OPTIONS.add_contents ? " [default]" : "");
  fprintf(stderr,
          "\t--nalu-length-bytes:\tSet the number of NALU length bytes: use -1 "
          "for explicit NALU separators, 0 for a single NALU, and >1 for "
          "explicit NALU length bytes [default: %i]\n",
          DEFAULT_OPTIONS.nalu_length_bytes);
  fprintf(
      stderr,
      "\t--frames-per-second:\tSet the fps for dumplength mode [default: %i]\n",
      DEFAULT_OPTIONS.frames_per_second);
  fprintf(stderr, "\t--version:\t\tDump version number\n");
  fprintf(stderr, "\t-h:\t\tHelp\n");
  exit(-1);
}

// long options with no equivalent short option
enum {
  QUIET_OPTION = CHAR_MAX + 1,
  DUMP_ALL_OPTION,
  DUMP_LENGTH_OPTION,
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
  ADD_RESOLUTION_FLAG_OPTION,
  NO_ADD_RESOLUTION_FLAG_OPTION,
  ADD_CONTENTS_FLAG_OPTION,
  NO_ADD_CONTENTS_FLAG_OPTION,
  HVCC_FILE_OPTION,
  NALU_LENGTH_BYTES_OPTION,
  FRAMES_PER_SECOND_OPTION,
  VERSION_OPTION,
  HELP_OPTION
};

arg_options *parse_args(int argc, char **argv) {
  int c;
  static arg_options options;

  // set default options
  options = DEFAULT_OPTIONS;

  // getopt_long stores the option index here
  int optindex = 0;

  // long options
  static struct option longopts[] = {
      // matching options to short options
      {"debug", no_argument, NULL, 'd'},
      {"infile", required_argument, NULL, 'i'},
      {"outfile", required_argument, NULL, 'o'},
      // options without a short option
      {"quiet", no_argument, NULL, QUIET_OPTION},
      {"dump-all", no_argument, NULL, DUMP_ALL_OPTION},
      {"dump-length", no_argument, NULL, DUMP_LENGTH_OPTION},
      {"as-one-line", no_argument, NULL, AS_ONE_LINE_FLAG_OPTION},
      {"no-as-one-line", no_argument, NULL, NO_AS_ONE_LINE_FLAG_OPTION},
      {"add-offset", no_argument, NULL, ADD_OFFSET_FLAG_OPTION},
      {"no-add-offset", no_argument, NULL, NO_ADD_OFFSET_FLAG_OPTION},
      {"add-length", no_argument, NULL, ADD_LENGTH_FLAG_OPTION},
      {"no-add-length", no_argument, NULL, NO_ADD_LENGTH_FLAG_OPTION},
      {"add-parsed-length", no_argument, NULL, ADD_PARSED_LENGTH_FLAG_OPTION},
      {"no-add-parsed-length", no_argument, NULL,
       NO_ADD_PARSED_LENGTH_FLAG_OPTION},
      {"add-checksum", no_argument, NULL, ADD_CHECKSUM_FLAG_OPTION},
      {"no-add-checksum", no_argument, NULL, NO_ADD_CHECKSUM_FLAG_OPTION},
      {"add-resolution", no_argument, NULL, ADD_RESOLUTION_FLAG_OPTION},
      {"no-add-resolution", no_argument, NULL, NO_ADD_RESOLUTION_FLAG_OPTION},
      {"add-contents", no_argument, NULL, ADD_CONTENTS_FLAG_OPTION},
      {"no-add-contents", no_argument, NULL, NO_ADD_CONTENTS_FLAG_OPTION},
      {"hvcc-file", required_argument, NULL, HVCC_FILE_OPTION},
      {"nalu-length-bytes", required_argument, NULL, NALU_LENGTH_BYTES_OPTION},
      {"frames-per-second", required_argument, NULL, FRAMES_PER_SECOND_OPTION},
      {"version", no_argument, NULL, VERSION_OPTION},
      {"help", no_argument, NULL, HELP_OPTION},
      {NULL, 0, NULL, 0}};

  // parse arguments
  while ((c = getopt_long(argc, argv, "di:o:h", longopts, &optindex)) != -1) {
    switch (c) {
      case 0:
        // long options that define flag
        // if this option set a flag, do nothing else now
        if (longopts[optindex].flag != NULL) {
          break;
        }
        fprintf(stdout, "option %s", longopts[optindex].name);
        if (optarg) {
          fprintf(stdout, " with arg %s", optarg);
        }
        break;

      case 'd':
        options.debug += 1;
        break;

      case QUIET_OPTION:
        options.debug = 0;
        break;

      case 'i':
        options.infile = optarg;
        break;

      case 'o':
        options.outfile = optarg;
        break;

      case DUMP_ALL_OPTION:
        options.dumpmode = dump_all;
        break;

      case DUMP_LENGTH_OPTION:
        options.dumpmode = dump_length;
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

      case ADD_RESOLUTION_FLAG_OPTION:
        options.add_resolution = true;
        break;

      case NO_ADD_RESOLUTION_FLAG_OPTION:
        options.add_resolution = false;
        break;

      case ADD_CONTENTS_FLAG_OPTION:
        options.add_contents = true;
        break;

      case NO_ADD_CONTENTS_FLAG_OPTION:
        options.add_contents = false;
        break;

      case HVCC_FILE_OPTION:
        options.hvcc_file = optarg;
        break;

      case NALU_LENGTH_BYTES_OPTION: {
        std::string optarg_str(optarg);
        options.nalu_length_bytes = std::stoi(optarg_str);
      } break;

      case FRAMES_PER_SECOND_OPTION: {
        std::string optarg_str(optarg);
        options.frames_per_second = std::stoi(optarg_str);
      } break;

      case VERSION_OPTION:
        fprintf(stdout, "version: %s\n", PROJECT_VERSION);
        exit(0);
        break;

      case HELP_OPTION:
      case 'h':
        usage(argv[0]);
        break;

      default:
        fprintf(stderr, "Unsupported option: %c\n", c);
        usage(argv[0]);
    }
  }

  // check there is at least a valid input file to parser
  if (options.infile == nullptr && options.hvcc_file == nullptr) {
    fprintf(stderr, "error: need at least one input file to parse\n");
    usage(argv[0]);
  }

  return &options;
}

inline std::string opt_value(int value, bool has_value) {
  return has_value ? std::to_string(value) : std::string{};
}

int main(int argc, char **argv) {
  arg_options *options;

  // parse args
  options = parse_args(argc, argv);
  if (options == nullptr) {
    usage(argv[0]);
    exit(-1);
  }

  if (options->debug > 1) {
#ifdef SMALL_FOOTPRINT
    printf("h265nal: small footprint version\n");
#else
    printf("h265nal: original version\n");
#endif
  }

  // print args
  if (options->debug > 1) {
    printf("options->debug = %i\n", options->debug);
    printf("options->infile = %s\n",
           (options->infile == nullptr) ? "null" : options->infile);
    printf("options->outfile = %s\n",
           (options->outfile == nullptr) ? "null" : options->outfile);
  }

  // add_contents requires add_length and add_offset
  if (options->add_contents) {
    options->add_offset = true;
    options->add_length = true;
  }

  // dump_length requires add_length
  if (options->dumpmode == dump_length) {
    options->add_length = true;
  }

  // 1. prepare bitstream parsing
  h265nal::ParsingOptions parsing_options;
  parsing_options.add_offset = options->add_offset;
  parsing_options.add_length = options->add_length;
  parsing_options.add_parsed_length = options->add_parsed_length;
  parsing_options.add_checksum = options->add_checksum;
  parsing_options.add_resolution = options->add_resolution;

  // 2. parse hvcC
  h265nal::H265BitstreamParserState bitstream_parser_state;
  std::shared_ptr<h265nal::H265ConfigurationBoxParser::ConfigurationBoxState>
      configuration_box;
  if (options->hvcc_file != nullptr) {
    // use bitstream parser state to keep the SPS/PPS/SubsetSPS NALUs
    // 2.1. read hvcc_file into buffer
    std::vector<uint8_t> hvcc_buffer;
    if (h265nal::H265Utils::ReadFile(options->hvcc_file, hvcc_buffer) < 0) {
      return -1;
    }
    uint8_t *hvcc_data = hvcc_buffer.data();
    size_t hvcc_length = hvcc_buffer.size();

    // 2.2. parse the hvcC structure
    configuration_box =
        h265nal::H265ConfigurationBoxParser::ParseConfigurationBox(
            hvcc_data, hvcc_length, &bitstream_parser_state, parsing_options);
    if (configuration_box == nullptr) {
      // cannot parse the NalUnit
#ifdef FPRINT_ERRORS
      fprintf(stderr, "error: cannot parse buffer into H265ConfigurationBox\n");
#endif  // FPRINT_ERRORS
      return -1;
    }
  }

  // 3. parse bitstream
  std::vector<uint8_t> buffer;
  std::unique_ptr<h265nal::H265BitstreamParser::BitstreamState> bitstream;
  if (options->infile != nullptr) {
    // 3.1. read infile into buffer
    if (h265nal::H265Utils::ReadFile(options->infile, buffer) < 0) {
      return -1;
    }
    // 3.2. parse buffer
    if (options->nalu_length_bytes < 0) {
      bitstream = h265nal::H265BitstreamParser::ParseBitstream(
          buffer.data(), buffer.size(), &bitstream_parser_state,
          parsing_options);
    } else {
      bitstream = h265nal::H265BitstreamParser::ParseBitstreamNALULength(
          buffer.data(), buffer.size(),
          static_cast<size_t>(options->nalu_length_bytes),
          &bitstream_parser_state, parsing_options);
    }
  }

#ifdef FDUMP_DEFINE
  // 4. dump parsed output

  // 4.1. get outfile file descriptor
  FILE *outfp;
  bool must_close_fp = false;
  if (options->outfile == nullptr ||
      (strlen(options->outfile) == 1 && options->outfile[0] == '-')) {
    // use stdout
    outfp = stdout;
  } else {
    must_close_fp = true;
    outfp = fopen(options->outfile, "wb");
    if (outfp == nullptr) {
      // did not work
      fprintf(stderr, "Could not open output file: \"%s\"\n", options->outfile);
      return -1;
    }
  }

  int indent_level = (options->as_one_line) ? -1 : 0;
  if (options->hvcc_file != nullptr) {
    // 4.2. dump the contents of the configuration box
    configuration_box->fdump(outfp, indent_level, parsing_options);
    fprintf(outfp, "\n");
  }

  if (options->infile != nullptr) {
    if (options->dumpmode == dump_length) {
      // add a CSV header
      fprintf(outfp,
              "nal_num,frame_num,nal_unit_type,nal_unit_type_str,"
              "nal_length_bytes,bitrate_bps,first_slice_segment_in_pic_flag,"
              "slice_segment_address,slice_pic_order_cnt_lsb\n");
    }
    // 4.3. dump the contents of each NALU
    size_t total_bytes = 0;
    size_t nal_num = 0;
    size_t frame_num = 0;
    uint32_t last_slice_nal_unit_type = 0;
    for (auto &nal_unit : bitstream->nal_units) {
      if (options->dumpmode == dump_all) {
        nal_unit->fdump(outfp, indent_level, parsing_options);
        if (options->add_contents) {
          fprintf(outfp, " contents {");
          for (size_t i = 0; i < nal_unit->length; i++) {
            fprintf(outfp, " %02x", buffer[nal_unit->offset + i]);
            if ((i + 1) % 16 == 0) {
              fprintf(outfp, " ");
            }
          }
          fprintf(outfp, " }");
        }
        fprintf(outfp, "\n");
      } else if (options->dumpmode == dump_length) {
        uint32_t nal_unit_type = nal_unit->nal_unit_header->nal_unit_type;
        std::string nal_unit_type_str =
            h265nal::NalUnitTypeToString(nal_unit_type);
        size_t nal_length_bytes = nal_unit->length;
        size_t bitrate_bps = 0;
        uint32_t first_slice_segment_in_pic_flag = 0;
        uint32_t slice_segment_address = 0;
        uint32_t slice_pic_order_cnt_lsb = 0;
        bool is_slice_segment = h265nal::IsSliceSegment(nal_unit_type);
        if (is_slice_segment) {
          first_slice_segment_in_pic_flag =
              nal_unit->nal_unit_payload->slice_segment_layer
                  ->slice_segment_header->first_slice_segment_in_pic_flag;
          slice_segment_address =
              nal_unit->nal_unit_payload->slice_segment_layer
                  ->slice_segment_header->slice_segment_address;
          slice_pic_order_cnt_lsb =
              nal_unit->nal_unit_payload->slice_segment_layer
                  ->slice_segment_header->slice_pic_order_cnt_lsb;
          if (first_slice_segment_in_pic_flag == 1 && total_bytes > 0) {
            // dump last frame info
            bitrate_bps = total_bytes * 8 *
                          static_cast<size_t>(options->frames_per_second);
            fprintf(outfp, ",%zu,%u,frame,,%zu,,,\n", frame_num,
                    last_slice_nal_unit_type, bitrate_bps);
            frame_num += 1;
            total_bytes = 0;
          }
          last_slice_nal_unit_type = nal_unit_type;
          total_bytes += nal_length_bytes;
        }
        fprintf(
            outfp, "%zu,%zu,%u,%s,%zu,,%s,%s,%s\n", nal_num, frame_num,
            nal_unit_type, nal_unit_type_str.c_str(), nal_length_bytes,
            opt_value(static_cast<int>(first_slice_segment_in_pic_flag),
                      is_slice_segment)
                .c_str(),
            opt_value(static_cast<int>(slice_segment_address), is_slice_segment)
                .c_str(),
            opt_value(static_cast<int>(slice_pic_order_cnt_lsb),
                      is_slice_segment)
                .c_str());
        nal_num += 1;
      }
    }
    if (options->dumpmode == dump_length) {
      if (total_bytes > 0) {
        // dump last frame info
        size_t bitrate_bps =
            total_bytes * 8 * static_cast<size_t>(options->frames_per_second);
        fprintf(outfp, ",%zu,%u,frame,,%zu,,,\n", frame_num,
                last_slice_nal_unit_type, bitrate_bps);
      }
    }
  }
#endif  // FDUMP_DEFINE

  // clean-up FD
  if (must_close_fp) {
    fclose(outfp);
  }

  return 0;
}
