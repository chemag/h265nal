/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h265_common.h"

#if defined WIN32 || defined _WIN32 || defined __CYGWIN__
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace h265nal {

bool IsSliceSegment(uint32_t nal_unit_type) {
  // payload (Table 7-1, Section 7.4.2.2)
  switch (nal_unit_type) {
    case TRAIL_N:
    case TRAIL_R:
    case TSA_N:
    case TSA_R:
    case STSA_N:
    case STSA_R:
    case RADL_N:
    case RADL_R:
    case RASL_N:
    case RASL_R:
    case BLA_W_LP:
    case BLA_W_RADL:
    case BLA_N_LP:
    case IDR_W_RADL:
    case IDR_N_LP:
    case CRA_NUT:
      // slice_segment_layer_rbsp()
      return true;
    default:
      break;
  }
  return false;
}

bool IsNalUnitTypeVcl(uint32_t nal_unit_type) {
  // payload (Table 7-1, Section 7.4.2.2)
  switch (nal_unit_type) {
    case TRAIL_N:
    case TRAIL_R:
    case TSA_N:
    case TSA_R:
    case STSA_N:
    case STSA_R:
    case RADL_N:
    case RADL_R:
    case RASL_N:
    case RASL_R:
    case RSV_VCL_N10:
    case RSV_VCL_R11:
    case RSV_VCL_N12:
    case RSV_VCL_R13:
    case RSV_VCL_N14:
    case RSV_VCL_R15:
    case BLA_W_LP:
    case BLA_W_RADL:
    case BLA_N_LP:
    case IDR_W_RADL:
    case IDR_N_LP:
    case CRA_NUT:
    case RSV_IRAP_VCL22:
    case RSV_IRAP_VCL23:
    case RSV_VCL24:
    case RSV_VCL25:
    case RSV_VCL26:
    case RSV_VCL27:
    case RSV_VCL28:
    case RSV_VCL29:
    case RSV_VCL30:
    case RSV_VCL31:
      return true;
    default:
      break;
  }
  return false;
}

bool IsNalUnitTypeNonVcl(uint32_t nal_unit_type) {
  // payload (Table 7-1, Section 7.4.2.2)
  switch (nal_unit_type) {
    case VPS_NUT:
    case SPS_NUT:
    case PPS_NUT:
    case AUD_NUT:
    case EOS_NUT:
    case EOB_NUT:
    case FD_NUT:
    case PREFIX_SEI_NUT:
    case SUFFIX_SEI_NUT:
    case RSV_NVCL41:
    case RSV_NVCL42:
    case RSV_NVCL43:
    case RSV_NVCL44:
    case RSV_NVCL45:
    case RSV_NVCL46:
    case RSV_NVCL47:
    case AP:
    case FU:
    case UNSPEC50:
    case UNSPEC51:
    case UNSPEC52:
    case UNSPEC53:
    case UNSPEC54:
    case UNSPEC55:
    case UNSPEC56:
    case UNSPEC57:
    case UNSPEC58:
    case UNSPEC59:
    case UNSPEC60:
    case UNSPEC61:
    case UNSPEC62:
    case UNSPEC63:
      return true;
    default:
      break;
  }
  return false;
}

bool IsNalUnitTypeUnspecified(uint32_t nal_unit_type) {
  // payload (Table 7-1, Section 7.4.2.2)
  switch (nal_unit_type) {
    case AP:
    case FU:
    case UNSPEC50:
    case UNSPEC51:
    case UNSPEC52:
    case UNSPEC53:
    case UNSPEC54:
    case UNSPEC55:
    case UNSPEC56:
    case UNSPEC57:
    case UNSPEC58:
    case UNSPEC59:
    case UNSPEC60:
    case UNSPEC61:
    case UNSPEC62:
    case UNSPEC63:
      return true;
    default:
      break;
  }
  return false;
}

void profileTypeToString(enum ProfileType profile, std::string &str) {
  switch (profile) {
    case UNSPECIFIED:
      str = "Unspecified";
      break;

    case MAIN:
      str = "Main";
      break;

    case MAIN_10:
      str = "Main 10";
      break;

    case MAIN_STILL_PICTURE:
      str = "Main Still Picture";
      break;

    case FREXT:
      str = "FRExt";
      break;

    case MULTIVIEW_MAIN:
      str = "Multiview Main";
      break;

    case SCALABLE:
      str = "Scalable";
      break;

    case _3D_MAIN:
      str = "3D-Main";
      break;

    case SCREEN_EXTENDED:
      str = "Screen-Extended";
      break;

    case SCREEN_EXTENDED_HIGH_THROUGHPUT:
      str = "Screen-Extended High-Throughput";
      break;

    case MULTIVIEW_EXTENDED:
      str = "Multiview Extended";
      break;

    case MULTIVIEW_EXTENDED_10:
      str = "Multiview Extended 10";
      break;

    // Section A.3.3
    case MAIN_10_STILL_PICTURE:
      str = "Main 10 Still Picture";
      break;

    // Table A.2
    case MONOCHROME:
      str = "Monochrome (8-bit)";
      break;

    case MONOCHROME_10:
      str = "Monochrome (10-bit)";
      break;

    case MONOCHROME_12:
      str = "Monochrome (12-bit)";
      break;

    case MONOCHROME_16:
      str = "Monochrome (16-bit)";
      break;

    case MAIN_12:
      str = "Main 12";
      break;

    case MAIN_422_10:
      str = "Main 4:2:2 10";
      break;

    case MAIN_422_12:
      str = "Main 4:2:2 12";
      break;

    case MAIN_444:
      str = "Main 4:4:4";
      break;

    case MAIN_444_10:
      str = "Main 4:4:4 10";
      break;

    case MAIN_444_12:
      str = "Main 4:4:4 12";
      break;

    case MAIN_INTRA:
      str = "Main Intra";
      break;

    case MAIN_10_INTRA:
      str = "Main 10 Intra";
      break;

    case MAIN_12_INTRA:
      str = "Main 12 Intra";
      break;

    case MAIN_422_10_INTRA:
      str = "Main 4:2:2 10 Intra";
      break;

    case MAIN_422_12_INTRA:
      str = "Main 4:2:2 12 Intra";
      break;

    case MAIN_444_INTRA:
      str = "Main 4:4:4 Intra";
      break;

    case MAIN_444_10_INTRA:
      str = "Main 4:4:4 10 Intra";
      break;

    case MAIN_444_12_INTRA:
      str = "Main 4:4:4 12 Intra";
      break;

    case MAIN_444_16_INTRA:
      str = "Main 4:4:4 16 Intra";
      break;

    case MAIN_444_STILL_PICTURE:
      str = "Main 4:4:4 Still Picture";
      break;

    case MAIN_444_16_STILL_PICTURE:
      str = "Main 4:4:4 16 Still Picture";
      break;

    // Table A.3
    case HIGH_THROUGHPUT_444:
      str = "High Throughput 4:4:4";
      break;

    case HIGH_THROUGHPUT_444_10:
      str = "High Throughput 4:4:4 10";
      break;

    case HIGH_THROUGHPUT_444_14:
      str = "High Throughput 4:4:4 14";
      break;

    case HIGH_THROUGHPUT_444_16_INTRA:
      str = "High Throughput 4:4:4 16 Intra";
      break;

    // Appendix H.11.1.1
    case SCALABLE_MAIN:
      str = "Scalable Main";
      break;

    case SCALABLE_MAIN_10:
      str = "Scalable Main 10";
      break;

    default:
      str = "Unknown";
      break;
  }
}

// NALU packing uses a mechanism to identify the start of a new NALU
// based on a 3-byte start code sequence. The idea is that every NALU
// starts with the binary string "\x00\x00\x01" ("start code prefix").
// In order to avoid the start code prefix to appear by chance in the
// middle of a NALU, the NALU is checked, and every appearance of a
// start code prefix is replaced by a 4-byte escaped version. The
// escaped version consists of adding a "\x03" byte as the third
// byte of the string. This means that the "\x00\x00\x01" string,
// when it is not a start code prefix, is replaced with the
// "\x00\x00\x03\x01" string. Note that we also need to escape the
// "\x00\x00\x03" string (using "\x00\x00\x03\x03" instead). For
// completeness, "\x00\x00\x00" and "\x00\x00\x02" are also escaped.
//
// UnescapeRbsp() takes a escaped string (where any 3-byte string
// where the first 2x bytes are "\x00" and the third byte is "\x00",
// "\x01", "\x02", or "\x03" have been escaped (and extra "\x03"
// has been inserted as third byte), and returns the unescaped one.
std::vector<uint8_t> UnescapeRbsp(const uint8_t *data, size_t length) {
  std::vector<uint8_t> out;
  out.reserve(length);

  for (size_t i = 0; i < length;) {
    // Be careful about over/underflow here. byte_length_ - 3 can underflow, and
    // i + 3 can overflow, but byte_length_ - i can't, because i < byte_length_
    // above, and that expression will produce the number of bytes left in
    // the stream including the byte at i.
    if (length - i >= 3 && data[i] == 0x00 && data[i + 1] == 0x00 &&
        data[i + 2] == 0x03) {
      // Two RBSP bytes.
      out.push_back(data[i++]);
      out.push_back(data[i++]);
      // Skip the emulation byte.
      i++;
    } else {
      // Single rbsp byte.
      out.push_back(data[i++]);
    }
  }
  return out;
}

// Syntax functions and descriptors) (Section 7.2)
bool byte_aligned(BitBuffer *bit_buffer) {
  // If the current position in the bitstream is on a byte boundary, i.e.,
  // the next bit in the bitstream is the first bit in a byte, the return
  // value of byte_aligned() is equal to TRUE.
  // Otherwise, the return value of byte_aligned() is equal to FALSE.
  size_t out_byte_offset, out_bit_offset;
  bit_buffer->GetCurrentOffset(&out_byte_offset, &out_bit_offset);

  return (out_bit_offset == 0);
}

int get_current_offset(BitBuffer *bit_buffer) {
  size_t out_byte_offset, out_bit_offset;
  bit_buffer->GetCurrentOffset(&out_byte_offset, &out_bit_offset);

  return out_byte_offset + ((out_bit_offset == 0) ? 0 : 1);
}

bool more_rbsp_data(BitBuffer *bit_buffer) {
  // > If there is no more data in the raw byte sequence payload (RBSP), the
  // > return value of more_rbsp_data() is equal to FALSE.
  uint64_t remaining_bitcount = bit_buffer->RemainingBitCount();
  if (remaining_bitcount == 0) {
    return false;
  }

  // > Otherwise, the RBSP data is searched for the last (least significant,
  // > right-most) bit equal to 1 that is present in the RBSP. Given the
  // > position of this bit, which is the first bit (rbsp_stop_one_bit) of
  // > the rbsp_trailing_bits() syntax structure, the following applies:
  // > - If there is more data in an RBSP before the rbsp_trailing_bits()
  // >   syntax structure, the return value of more_rbsp_data() is equal to
  // >   TRUE.
  // > - Otherwise, the return value of more_rbsp_data() is equal to FALSE.
  // > The method for enabling determination of whether there is more data
  // > in the RBSP is specified by the application (or in Annex B for
  // > applications that use the byte stream format).

  // Here we do the following simplification:
  // (1) We know that rbsp_trailing_bits() is limited to at most 1 byte. Its
  // definition is:
  // > rbsp_trailing_bits() {
  // >   rbsp_stop_one_bit // equal to 1
  // >   while( !byte_aligned() )
  // >     rbsp_alignment_zero_bit // equal to 0
  // >   }
  // where byte_aligned() is a Bool stating whether the position of the
  // bitstream is in a byte boundary. So, if there is more than 1 byte
  // left (8 bits left), clearly "there is more data in the RBSP before the
  // rbsp_trailing_bits()"
  if (remaining_bitcount > 8) {
    return true;
  }

  // (2) if we are indeed in the last byte, we just need to know whether the
  // rest of the byte is [1, 0, ..., 0]. For that, we want to peek in the
  // bit buffer (not read).
  // So we first read (peek) the remaining bits.
  uint32_t remaining_bits;
  if (!bit_buffer->PeekBits(remaining_bitcount, remaining_bits)) {
    // this should not happen: we do not have remaining_bits bits left.
    return false;
  }
  // and then check for the actual values to be 100..000
  bool is_rbsp_trailing_bits =
      (remaining_bits == (unsigned int)(1 << (remaining_bitcount - 1)));

  // if the actual values to be 100..000, we are already at the
  // rbsp_trailing_bits, which means there is no more RBSP data
  return !is_rbsp_trailing_bits;
}

bool rbsp_trailing_bits(BitBuffer *bit_buffer) {
  uint32_t bits_tmp;

  // rbsp_stop_one_bit  f(1) // equal to 1
  if (!bit_buffer->ReadBits(1, bits_tmp)) {
    return false;
  }
  if (bits_tmp != 1) {
    return false;
  }

  while (!byte_aligned(bit_buffer)) {
    // rbsp_alignment_zero_bit  f(1) // equal to 0
    if (!bit_buffer->ReadBits(1, bits_tmp)) {
      return false;
    }
    if (bits_tmp != 0) {
      return false;
    }
  }
  return true;
}

#if defined(FDUMP_DEFINE)
int indent_level_incr(int indent_level) {
  return (indent_level == -1) ? -1 : (indent_level + 1);
}

int indent_level_decr(int indent_level) {
  return (indent_level == -1) ? -1 : (indent_level - 1);
}

void fdump_indent_level(FILE *outfp, int indent_level) {
  if (indent_level == -1) {
    // no indent
    fprintf(outfp, " ");
    return;
  }
  fprintf(outfp, "\n");
  fprintf(outfp, "%*s", 2 * indent_level, "");
}
#endif  // FDUMP_DEFINE

std::shared_ptr<NaluChecksum> NaluChecksum::GetNaluChecksum(
    BitBuffer *bit_buffer) noexcept {
  // save the bit buffer current state
  size_t byte_offset = 0;
  size_t bit_offset = 0;
  bit_buffer->GetCurrentOffset(&byte_offset, &bit_offset);

  auto checksum = std::make_shared<NaluChecksum>();
  // implement simple IP-like checksum (extended from 16/32 to 32/64 bits)
  // Inspired in https://stackoverflow.com/questions/26774761

  // Our algorithm is simple, using a 64 bit accumulator (sum), we add
  // sequential 32 bit words to it, and at the end, fold back all the
  // carry bits from the top 32 bits into the lower 32 bits.

  uint64_t sum = 0;

  uint32_t val = 0;
  while (bit_buffer->ReadUInt32(val)) {
    sum += val;
  }

  // check if there are unread bytes
  int i = 0;
  uint8_t val8 = 0;
  val = 0;
  while (bit_buffer->RemainingBitCount() > 0) {
    (void)bit_buffer->ReadUInt8(val8);
    val |= (val8 << (8 * (3 - i)));
    i += 1;
  }
  if (i > 0) {
    sum += val;
  }

  // add back carry outs from top 32 bits to low 32 bits
  // add hi 32 to low 32
  sum = (sum >> 32) + (sum & 0xffffffff);
  // add carry
  sum += (sum >> 32);
  // truncate to 32 bits and get one's complement
  uint32_t answer = ~sum;

  // write sum into (generic) checksum buffer (network order)
  *(reinterpret_cast<uint32_t *>(checksum->checksum)) = htonl(answer);
  checksum->length = 4;

  // return the bit buffer to the original state
  bit_buffer->Seek(byte_offset, bit_offset);

  return checksum;
}

void NaluChecksum::fdump(char *output, int output_len) const {
  int i = 0;
  int oi = 0;
  while (i < length) {
    // make sure there is space in the output buffer
    if (oi + 2 >= output_len) {
      output[output_len - 1] = '\0';
      break;
    }
    oi += snprintf(output + oi, output_len - oi, "%02x",
                   static_cast<unsigned char>(checksum[i++]));
  }
}

const char *NaluChecksum::GetPrintableChecksum() const {
#define BUFFER_LEN ((kMaxLength * 2) + 1)
  static char buffer[BUFFER_LEN];
  int i = 0;
  int oi = 0;
  while (i < length) {
    oi += snprintf(buffer + oi, BUFFER_LEN - oi, "%02x",
                   static_cast<unsigned char>(checksum[i++]));
  }
  buffer[i] = '\0';
  return buffer;
}

}  // namespace h265nal
