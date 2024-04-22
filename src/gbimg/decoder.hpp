#ifndef __DECODER_HPP__
#define __DECODER_HPP__

#include <array>
#include <bitset>
#include <filesystem>
#include <vector>

#include <cstdint> // std::uint8_t, std::size_t

#include "binaryreader.hpp"

// max number of bits in an rle packet (check!)
constexpr std::size_t RLE_PACKET_MAX_BITS = 512;

class Decoder {
public:
  Decoder(
    const std::vector<std::uint8_t> &rd,

    const std::filesystem::path &output_path=".",
    bool create_dirs=false, bool debug=false
  );

  static std::vector<std::uint8_t> rle_decode(
    BinaryReader &reader, std::uint8_t width, std::uint8_t height
  );
  static std::vector<std::bitset<2>> decode_rle_packet(
    BinaryReader &reader
  );
  static std::vector<std::bitset<2>> decode_data_packet(
    BinaryReader &reader
  );
  static std::vector<std::uint8_t> delta_decode(
    std::vector<std::uint8_t> &encoded_data, std::uint8_t width,
    std::uint8_t height
  );

  // byte pairs are in column order (two columns wide)
  static std::vector<std::uint8_t>transpose_pairs(
    const std::vector<std::bitset<2>> &pairs, std::uint8_t width,
    std::uint8_t height
  );

// private:
  std::vector<std::uint8_t> data;
  std::vector<std::uint8_t> buffer;
  std::vector<std::uint8_t> plane_0;
  std::vector<std::uint8_t> plane_1;

  std::uint8_t width;
  std::uint8_t height;

  bool swap_buffers;
  std::uint8_t encoding_mode;
};

#endif // __DECODER_HPP__
