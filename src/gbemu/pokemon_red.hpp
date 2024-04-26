#ifndef __POKEMON_RED__
#define __POKEMON_RED__

#include <map>
#include <string>
#include <vector>

#include <cstdint> // std::uint8_t, std::uint16_t

#include "cartridge.hpp"

namespace pkmnred {
  // general header
  constexpr std::uint16_t entry_point      = 0x0100;
  constexpr std::uint16_t rom_size_pointer = 0x0148;
  constexpr std::uint16_t ram_size_pointer = 0x0149;

  // pokemon red specific header
  const std::string name_string = "Pokemon Red Version";

  // 16 bytes, padded with 0's
  const std::vector<std::uint8_t> name = {
    'P', 'O', 'K', 'E', 'M', 'O', 'N', ' ', 'R', 'E', 'D', 0, 0, 0, 0, 0
  };

  constexpr std::uint8_t eos_char = 0x50;

  /////////////////////////////////////////////////////////////////////////////
  //
  constexpr std::uint8_t minimum_index = 0x01;
  constexpr std::uint8_t maximum_index = 0xbe;
  const std::vector<std::uint8_t> missingno = {
    0x1f, 0x20, 0x32, 0x34, 0x38, 0x3d, 0x3e, 0x3f, 0x43, 0x44, 0x45, 0x4f,
    0x50, 0x51, 0x56, 0x57, 0x5e, 0x5f, 0x73, 0x79, 0x7a, 0x7f, 0x86, 0x87,
    0x89, 0x8c, 0x92, 0x9c, 0x9f, 0xa0, 0xa1, 0xa2, 0xac, 0xae, 0xaf, 0xb5,
    0xb6, 0xb7, 0xb8
  };

  const std::vector<std::uint8_t> dex_to_index = {
    0x99, 0x09, 0x9A, 0xB0, 0xB2, 0xB4, 0xB1, 0xBC, 0x1C, 0x7B, 0x7C, 0x7D,
    0x70, 0x71, 0x72, 0x24, 0x96, 0x97, 0xA5, 0xA6, 0x05, 0x23, 0x6C, 0x2D,
    0x54, 0x55, 0x60, 0x61, 0x0F, 0xA8, 0x10, 0x03, 0xA7, 0x07, 0x04, 0x8E,
    0x52, 0x53, 0x64, 0x65, 0x6B, 0x82, 0xB9, 0xBA, 0xBB, 0x6D, 0x2E, 0x41,
    0x77, 0x3B, 0x76, 0x4D, 0x90, 0x2F, 0x80, 0x39, 0x75, 0x21, 0x14, 0x47,
    0x6E, 0x6F, 0x94, 0x26, 0x95, 0x6A, 0x29, 0x7E, 0xBC, 0xBD, 0xBE, 0x18,
    0x9B, 0xA9, 0x27, 0x31, 0xA3, 0xA4, 0x25, 0x08, 0xAD, 0x36, 0x40, 0x46,
    0x74, 0x3A, 0x78, 0x0D, 0x88, 0x17, 0x8B, 0x19, 0x93, 0x0E, 0x22, 0x30,
    0x81, 0x4E, 0x8A, 0x06, 0x8D, 0x0C, 0x0A, 0x11, 0x91, 0x2B, 0x2C, 0x0B,
    0x37, 0x8F, 0x12, 0x01, 0x28, 0x1E, 0x02, 0x5C, 0x5D, 0x9D, 0x9E, 0x1B,
    0x98, 0x2A, 0x1A, 0x48, 0x35, 0x33, 0x1D, 0x3C, 0x85, 0x16, 0x13, 0x4C,
    0x66, 0x69, 0x68, 0x67, 0xAA, 0x62, 0x63, 0x5A, 0x5B, 0xAB, 0x84, 0x4A,
    0x4B, 0x49, 0x58, 0x59, 0x42, 0x83, 0x15
  };

  /////////////////////////////////////////////////////////////////////////////
  // pointer tables (indexable list of 16-bit offsets)
  // basically a data table with a width of 2, but byteswapped

  // pointers to null-terminated strings (variable width)
  constexpr std::uint8_t  type_names_pointer_bank     = 0x09;
  constexpr std::uint16_t type_names_pointer_offset   = 0x7dae;

  // pointers to pokedex entries (null-terminated string + 9 bytes)
  // indexed by (id - 1) -> id 1 (Rhydon) == index 0
  constexpr std::uint8_t  pokedex_data_pointer_bank   = 0x10;
  constexpr std::uint16_t pokedex_data_pointer_offset = 0x447e;

  /////////////////////////////////////////////////////////////////////////////
  // data tables (indexable list of data with a set width)

  // 0x00038000
  constexpr std::uint8_t  moves_data_table_bank       = 0x0e;
  constexpr std::uint16_t moves_data_table_offset     = 0x4000;
  constexpr std::uint8_t  moves_data_table_width      = 6;

  // indexed by pokedex number -> pokedex 1 (Bulbasaur) == index 1
  // 0x000383de
  constexpr std::uint8_t  pokemon_stats_table_bank    = 0x0e;
  constexpr std::uint16_t pokemon_stats_table_offset  = 0x43de;
  constexpr std::uint8_t  pokemon_stats_table_width   = 28;

  // mew's stats are stored in their own table
  // 0x0004425b
  constexpr std::uint8_t  mew_stats_table_bank        = 0x01;
  constexpr std::uint16_t mew_stats_table_offset      = 0x425b;
  constexpr std::uint8_t  mew_stats_table_width       = 28;

  // indexed by (id - 1) -> id 1 (Rhydon) == index 0 == pokedex 112
  // 0x00041024
  constexpr std::uint8_t  pokedex_order_table_bank    = 0x10;
  constexpr std::uint16_t pokedex_order_table_offset  = 0x5024;
  constexpr std::uint8_t  pokedex_order_table_width   = 1;

  // indexed by (id - 1) -> id 1 (Rhydon) == index 0
  // 0x0001c21e
  constexpr std::uint8_t  pokemon_names_table_bank    = 0x07;
  constexpr std::uint16_t pokemon_names_table_offset  = 0x421e;
  constexpr std::uint8_t  pokemon_names_table_width   = 10;

  /////////////////////////////////////////////////////////////////////////////
  // hacks

  // can't find any table for the move names, so they will be automatically
  // loaded and stored in the ram, located directly after

  // pointers to null-terminated strings (variable width)
  constexpr std::uint16_t move_names_pointer_bank   = 0x2c;
  constexpr std::uint16_t move_names_pointer_offset = 0xa498;

  /////////////////////////////////////////////////////////////////////////////
  // ram addresses
  constexpr std::uint16_t sprite_buffer_0 = 0xa000;
  constexpr std::uint16_t sprite_buffer_1 = 0xa188;
  constexpr std::uint16_t sprite_buffer_2 = 0xa310;

  struct PokemonStats {
    std::uint8_t id;
    std::uint8_t hp;
    std::uint8_t attack;
    std::uint8_t defence;
    std::uint8_t speed;
    std::uint8_t special;
    std::uint8_t type_1_index;
    std::uint8_t type_2_index;
    std::uint8_t catch_rate;
    std::uint8_t exp_yield;
    std::uint8_t sprite_size;
    std::uint16_t front_sprite_offset;
    std::uint16_t back_sprite_offset;
    std::uint8_t move_1_index;
    std::uint8_t move_2_index;
    std::uint8_t move_3_index;
    std::uint8_t move_4_index;
    std::uint64_t hm_and_tm_bits; // maybe bitset?
    std::uint8_t null_byte;

    std::uint8_t dexno;
    std::string name;
    std::string type_1;
    std::string type_2;
    std::string move_1;
    std::string move_2;
    std::string move_3;
    std::string move_4;

    std::string pokemon_type_name;
    std::uint8_t height_ft;
    std::uint8_t height_in;
    std::uint16_t weight;
    std::uint8_t unknown_dex_byte_5;
    std::uint16_t pokedex_entry_offset;
    std::uint8_t unknown_dex_byte_8; // maybe description bank?

    std::string height_string;
    std::string weight_string;
    std::string pokedex_entry;
  };

  struct MoveData {
    std::uint8_t index;
    std::uint8_t effects;
    std::uint8_t power;
    std::uint8_t type;
    std::uint8_t accuracy;
    std::uint8_t pp;

    std::string name;
  };

  PokemonStats get_stats(std::uint8_t pokemon_id, Cartridge &cart);

  void load_move_names(Cartridge &cart);
  std::map<std::uint8_t, std::string> get_charmap();
  std::uint8_t get_sprite_bank(std::uint8_t index);

  std::ostream &operator<<(std::ostream &os, const PokemonStats &stats);
}


#endif // __POKEMON_RED__
