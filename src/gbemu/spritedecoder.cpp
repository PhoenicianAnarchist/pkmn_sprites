#include <bitset>

#include "spritedecoder.hpp"

gbemu::Decoder::Decoder(Cartridge &cart)
: cart(cart), rom_interface(cart.bank1), offset(0), bank(0), width(0),
  height(0), encoding_mode(0), swap_buffers(false)
{}

void gbemu::Decoder::set_bank(std::uint8_t value) {
  bank = value;
  cart.switch_bank(bank);
  rom_interface.attach_buffer(cart.bank1);
}

void gbemu::Decoder::set_offset(std::uint16_t value) {
  offset = value;
  rom_interface.seek((offset - 0x4000) * 8);
}

void gbemu::Decoder::read_header() {
  width = rom_interface.get_nibble();
  height = rom_interface.get_nibble();
  swap_buffers = rom_interface.get();
}

void gbemu::Decoder::read_encoding_mode() {
  encoding_mode = rom_interface.get();
  if (encoding_mode == 1) {
    encoding_mode <<= 1;
    encoding_mode |= rom_interface.get();
  }
}

void gbemu::Decoder::rle_decode(bool is_first_plane) {
  std::size_t pairs_to_read = width * height * 8 * 4;

  // each column is only 2 pixels wide
  std::size_t current_column = 0;
  std::size_t current_row = 0;
  std::size_t offset = 0;
  std::size_t buffer_offset = 392;

  if (is_first_plane ^ swap_buffers) {
    buffer_offset = 784;
  }

  std::size_t num_rows = (height * 8);

  bool packet_is_data = rom_interface.get();
  while (true) {
    std::vector<std::bitset<2>> pairs;
    try {
      if (packet_is_data) {
        pairs = decode_data_packet();
      } else {
        pairs = decode_rle_packet();
      }
    } catch (std::ios_base::failure &e) { break; }

    for (std::bitset<2> p : pairs) {
      std::size_t byte_index = buffer_offset + offset + current_row;

      std::uint8_t new_byte = (p.to_ulong() & 0xff);
      new_byte <<= (3 - (current_column % 4)) * 2;
      cart.ram[byte_index] |= new_byte;
      ++current_row;
      if (current_row >= (num_rows)) {
        current_row = 0;
        ++current_column;
        offset = ((current_column / 4) * num_rows);
      }
    }

    pairs_to_read -= pairs.size();

    if (pairs_to_read <= 0) {
      break;
    }

    packet_is_data = !packet_is_data;
  }
}

std::vector<std::bitset<2>> gbemu::Decoder::decode_rle_packet() {
  bool bit = 0;
  std::size_t bits_read = 0;

  std::bitset<RLE_PACKET_MAX_BITS> l = 0;
  do {
    bit = rom_interface.get();
    l <<= 1;
    l.set(0, bit);

    ++bits_read;
  } while (bit != 0);

  std::bitset<RLE_PACKET_MAX_BITS> v = 0;
  for (std::size_t i = 0; i < bits_read; ++i) {
    bit = rom_interface.get();
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

std::vector<std::bitset<2>> gbemu::Decoder::decode_data_packet() {
  std::vector<std::bitset<2>> pairs {};

  std::bitset<2> pair = 0;
  while (1) {
    pair = rom_interface.get_pair();

    if (pair == 0) {
      break;
    }

    pairs.push_back(pair);
  };

  return pairs;
}
