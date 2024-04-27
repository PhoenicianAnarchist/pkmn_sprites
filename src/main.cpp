#include <bitset>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <sstream>
#include <vector>

#include <cstdint> // std::uint8_t

#include "gbemu/cartridge.hpp"
#include "gbemu/helpers.hpp"
#include "gbemu/pokemon_red.hpp"
#include "gbemu/spritedecoder.hpp"

#include "util/options.hpp"

void test_ram(Cartridge &cart);

// All pointers and verification info is stored in an external file for ease
// of adding new rom info.
namespace rominfo = pkmnred;

int main(int argc, char *argv[]) {
  OPTIONS options = parse_command_line(argc, argv);

  if (options.err) {
    return options.err;
  } else if (options.called_for_help) {
    return 0;
  }

  Cartridge cart;
  cart.load_rom(options.rom_path);

  // Check rom loaded successfully
  std::vector<std::uint8_t> name = cart.read(0x134, 16);

  if (name != rominfo::name) {
    std::cerr << "File appears to be incorrect\nPlease supply path to ";
    std::cerr << rominfo::name_string << " ROM" << std::endl;
    return 1;
  }

  std::cout << "Successfully loaded " << rominfo::name_string << std::endl;
  std::cout << "------------------------------------------------" << std::endl;

  if (options.dexno != 0) {
    options.index = rominfo::dex_to_index[(options.dexno - 1)];
  }

  std::uint8_t pokemon_id = options.index;

  rominfo::PokemonStats pokemon_stats;
  try {
    pokemon_stats = rominfo::get_stats(pokemon_id, cart);
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
  std::cout << pokemon_stats << std::endl;

  gbemu::Decoder decoder(cart);
  decoder.set_bank(rominfo::sprite_banks[pokemon_stats.id]);
  decoder.set_offset(pokemon_stats.front_sprite_offset);
  decoder.read_header();
  decoder.rle_decode(0);
  decoder.read_encoding_mode();
  decoder.rle_decode(1);

  gbhelp::dump_ram(cart, "", false);

  return 0;
}

void test_ram(Cartridge &cart) {
  gbemu::BinaryInterface bi(cart.ram);
  for (std::size_t col = 0; col < 168; ++col) {
    std::size_t offset = col * 56;
    for (std::size_t row = 0; row < 56; ++row) {
      std::size_t index = row + offset;

      // std::size_t new_index = col + (row * 168);
      bi.seek(index  *8);
      for (int i = 8; i >= 0; --i) {
        bi.put((index >> (i - 1)) & 0b1);
      }
    }
  }

  gbhelp::dump_ram(cart, "", false);
}
