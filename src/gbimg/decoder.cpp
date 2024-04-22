// #include <iostream>
#include <iomanip>
#include <bitset>
#include <utility> // std::swap

#include "binaryreader.hpp"
#include "decoder.hpp"
#include "sprite.hpp"

Decoder::Decoder(const std::vector<std::uint8_t> &rd)
: data(rd), width(0), height(0), swap_buffers(false), encoding_mode(0) {
  // each bitplane is 56*56 pixels @ 1bpp = 3136 bits (392 bytes)
  // buffer can hold 3 bitplanes (1176 bytes)
  buffer.resize(1176);

  BinaryReader reader(data);

  width = reader.get_nibble();
  height = reader.get_nibble();
  swap_buffers = reader.get();

  // number of bytes to read for each plane
  // width * height (num tiles) * 64 1bpp pixels / 8 bits per byte
  std::size_t buffer_size = width * height * 8;

  // std::cout << "rle decoding plane 0" << std::endl;
  plane_0 = rle_decode(reader, width, height);

  encoding_mode = reader.get();
  if (encoding_mode == 1) {
    encoding_mode <<= 1;
    encoding_mode |= reader.get();
  }

  plane_1 = rle_decode(reader, width, height);



  // if (swap_buffers) {
  //   std::swap(plane_0, plane_1);
  // }
  //
  // if (encoding_mode != 1) {
  //   plane_1 = xor(plane_0, plane_1);
  // }
  //
  // plane_0 = delta_decode(plane_0);
  //
  // if (encoding_mode != 2) {
  //   plane_1 = delta_decode(plane_1);
  // }
}
//
std::vector<std::uint8_t> Decoder::rle_decode(
  BinaryReader &reader, std::uint8_t width, std::uint8_t height
) {
  std::size_t buffer_size = width * height * 8;

  std::vector<std::bitset<2>> decoded;
  std::size_t pairs_read = 0;

  bool packet_is_data = reader.get();

  // data will be decoded until enough bytes have been read into the buffer
  while (true) {
    std::vector<std::bitset<2>> pairs;
    try {
      if (packet_is_data) {
        pairs = decode_data_packet(reader);
        std::copy(pairs.begin(), pairs.end(), std::back_inserter(decoded));
      } else {
        pairs = decode_rle_packet(reader);
        std::copy(pairs.begin(), pairs.end(), std::back_inserter(decoded));
      }
    } catch (std::ios_base::failure &e) { break; }

    pairs_read += pairs.size();

    // std::cout << "Packet type: ";
    // std::cout << (packet_is_data ? "data" : "rle") << std::endl;
    // for (auto p : pairs) {
    //   std::cout << p << " ";
    // }
    // std::cout << std::endl;

    if ((pairs_read / 4) >= buffer_size) {
      break;
    }

    packet_is_data = !packet_is_data;
  }

  return transpose_pairs(decoded, width, height);
}

std::vector<std::bitset<2>> Decoder::decode_rle_packet(BinaryReader &reader) {
  bool bit = 0;
  std::size_t bits_read = 0;

  std::bitset<RLE_PACKET_MAX_BITS> l = 0;
  do {
    bit = reader.get();
    l <<= 1;
    l.set(0, bit);

    ++bits_read;
  } while (bit != 0);

  std::bitset<RLE_PACKET_MAX_BITS> v = 0;
  for (std::size_t i = 0; i < bits_read; ++i) {
    bit = reader.get();
    v <<= 1;
    v.set(0, bit);
  }

  std::size_t num_pairs = l.to_ulong() + v.to_ulong() + 1;

  std::vector<std::bitset<2>> pairs;

  for (std::size_t i = 0; i < num_pairs; ++i) {
    pairs.push_back({0b00});
  }

  return pairs;
}

std::vector<std::bitset<2>> Decoder::decode_data_packet(BinaryReader &reader) {
  std::vector<std::bitset<2>> pairs {};

  std::bitset<2> pair = 0;
  while (1) {
    pair = reader.get_pair();

    if (pair == 0) {
      break;
    }

    pairs.push_back(pair);
  };

  return pairs;
}

std::vector<std::uint8_t> Decoder::transpose_pairs(
  const std::vector<std::bitset<2>> &pairs, std::uint8_t width,
  std::uint8_t height
) {
  // byte pairs are in column order (two columns wide)
  //

  std::vector<std::uint8_t> packed_data;
  packed_data.resize(pairs.size() / 4);

  BinaryReader reader(packed_data);

  std::size_t num_cols = width * 8 / 2;
  std::size_t num_rows = height * 8;

  for (std::size_t col = 0; col < num_cols; ++col) {
    std::size_t src_column_offset = col * num_rows;
    std::size_t dst_column_offset = col * 2;

    for (std::size_t row = 0; row < num_rows; ++row) {
      std::size_t src_index = row + src_column_offset;

      std::size_t dst_row_offset = row * (num_cols * 2);
      std::size_t dst_index = dst_column_offset + dst_row_offset;

      std::bitset<2> pair = pairs[src_index];
      reader.seek(dst_index);
      reader.put(pair.test(1));
      reader.put(pair.test(0));
    }
  }

  return packed_data;
}
