#ifndef __GBEMU_SPRITE_DECODER_HPP__
#define __GBEMU_SPRITE_DECODER_HPP__

#include <cstdint>

#include "binaryinterface.hpp"
#include "cartridge.hpp"

namespace gbemu {
  constexpr std::size_t RLE_PACKET_MAX_BITS = 16;

  class Decoder {
  public:
    Decoder(Cartridge &card);
    void set_bank(std::uint8_t value);
    void set_offset(std::uint16_t value);

    void read_header();
    void read_encoding_mode();
    void rle_decode(bool is_first_plane=false);
    // void delta_decode();
    // void xor_planes();

  private:
    std::vector<std::bitset<2>> decode_rle_packet();
    std::vector<std::bitset<2>> decode_data_packet();

    Cartridge &cart;
    BinaryInterface rom_interface;
    std::uint16_t offset;
    std::uint8_t bank;

    std::uint8_t width;
    std::uint8_t height;
    std::uint8_t encoding_mode;
    bool swap_buffers;
  };
}

#endif // __GBEMU_SPRITE_DECODER_HPP__
