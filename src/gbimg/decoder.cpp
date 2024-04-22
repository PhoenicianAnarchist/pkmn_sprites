#include <iostream>
#include <iomanip>
#include <bitset>
#include <utility> // std::swap

#include "binaryreader.hpp"
#include "decoder.hpp"
#include "sprite.hpp"

Decoder::Decoder(
  const std::vector<std::uint8_t> &rd,
  const std::filesystem::path &output_path, bool create_dirs, bool debug
)
: data(rd), width(0), height(0), swap_buffers(false), encoding_mode(0) {
  // each bitplane is 56*56 pixels @ 1bpp = 3136 bits (392 bytes)
  // buffer can hold 3 bitplanes (1176 bytes)
  buffer.resize(1176);

  BinaryReader reader(data);

  width = reader.get_nibble();
  height = reader.get_nibble();
  swap_buffers = reader.get();

  if (debug) {
    std::cout << "Decoding Sprite Data" << std::endl;
    std::cout << "  Width  " << int(width) << std::endl;
    std::cout << "  Height " << int(height) << std::endl;
    std::cout << "  Swap?  " << std::boolalpha << swap_buffers << std::endl;
  }

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

  if (debug) {
    std::cout << "  Mode   " << int(encoding_mode) << std::endl;

    render_bitplane(
      plane_0, width, height, output_path, "plane_0_rle", create_dirs
    );
    render_bitplane(
      plane_1, width, height, output_path, "plane_1_rle", create_dirs
    );
  }

  plane_0 = delta_decode(plane_0, width, height);

  if (debug) {
    render_bitplane(
      plane_0, width, height, output_path, "plane_0_delta", create_dirs
    );
  }

  if (encoding_mode != 2) {
    plane_1 = delta_decode(plane_1, width, height);

    if (debug) {
      render_bitplane(
        plane_1, width, height, output_path, "plane_1_delta", create_dirs
      );
    }
  }



  // if (swap_buffers) {
  //   std::swap(plane_0, plane_1);
  // }

  // if (encoding_mode != 1) {
  //   plane_1 = xor(plane_0, plane_1);
  //
  //   if (debug) {
  //     render_bitplane(
  //       plane_1, width, height, output_path, "plane_1_delta", create_dirs
  //     );
  //   }
  // }
}

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

std::vector<std::uint8_t> Decoder::delta_decode(
  std::vector<std::uint8_t> &encoded_data, std::uint8_t width,
  std::uint8_t height
) {
  std::vector<std::uint8_t> data;
  data.resize(encoded_data.size());

  BinaryReader reader(encoded_data);
  BinaryReader writer(data);

  std::cout << std::noboolalpha;

  for (std::size_t i = 0; i < (height * 8); ++i) {
    std::size_t offset = i * (width * 8);

    // previous_bit is reset to 0 for each line
    // NOTE: previous_bit is the last bit that was **written** not read!
    bool previous_bit = 0;
    for (std::size_t j = 0; j < (width * 8); ++j) {
      std::size_t index = j + offset;

      reader.seek(index);
      bool r = reader.get();
      bool n = (r == previous_bit) ? 0 : 1;

      writer.seek(index);
      writer.put(n);
      previous_bit = n;
    }
  }

  return data;
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
