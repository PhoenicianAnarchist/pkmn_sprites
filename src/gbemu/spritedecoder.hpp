#ifndef __GBEMU_SPRITE_DECODER_HPP__
#define __GBEMU_SPRITE_DECODER_HPP__

#include <cstdint>

#include "binaryinterface.hpp"
#include "cartridge.hpp"

namespace gbemu {
  constexpr std::size_t RLE_PACKET_MAX_BITS = 16;

  class Decoder {
  public:
    Decoder(Cartridge &card, int verbose_level=0);
    void set_bank(std::uint8_t value);
    void set_offset(std::uint16_t value);

    void read_header();
    void read_encoding_mode();
    void rle_decode(std::size_t plane_index);
    void delta_decode(std::size_t plane_index);
    void xor_planes();
    void clear(std::size_t plane_index);
    void copy(std::size_t src_plane, std::size_t dst_plane);
    void zip_planes();

  // private:
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

    std::uint8_t primary_buffer;
    std::uint8_t secondary_buffer;

    int verbose_level;
  };
}

#endif // __GBEMU_SPRITE_DECODER_HPP__
