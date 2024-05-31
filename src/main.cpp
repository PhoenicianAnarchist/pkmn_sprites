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
#include "gbemu/spriterenderer.hpp"

#include "util/options.hpp"

void test_ram(Cartridge &cart);

// All pointers and verification info is stored in an external file for ease
// of adding new rom info.
namespace rominfo = pkmnred;

int extract_sprite(
  Cartridge& cart, std::uint8_t pokemon_id, int verbose_level=0
);

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

  if (options.extract_all) {
    for (std::uint8_t i = 1; i <= 151; ++i) {
      // cart.ram.fill(0);

      std::uint8_t index = rominfo::dex_to_index[i - 1];
      std::uint8_t err = extract_sprite(cart, index, options.verbose_level);
      if (err) {
        return err;
      }

    }
  } else {
    std::uint8_t index = options.index;

    if (options.dexno != 0) {
      index = rominfo::dex_to_index[(options.dexno - 1)];
    }

    std::uint8_t err = extract_sprite(cart, index, options.verbose_level);

    if (err) {
      return err;
    }
  }

  return 0;
}

void test_ram(Cartridge &cart) {
  gbemu::BinaryInterface bi(cart.ram);
  for (std::size_t col = 0; col < 168; ++col) {
    std::size_t offset = col * 56;
    for (std::size_t row = 0; row < 56; ++row) {
      std::size_t index = row + offset;

      bi.seek(index * 8);
      for (int i = 8; i >= 0; --i) {
        bi.put((index >> (i - 1)) & 0b1);
      }
    }
  }

  gbhelp::dump_ram(cart, "debug", "ram", true);
}

int extract_sprite(
  Cartridge& cart, std::uint8_t pokemon_id, int verbose_level
) {
  /////////////////////////////////////////////////////////////////////////////
  // Fetch pokemon information
  rominfo::PokemonStats pokemon_stats;
  try {
    pokemon_stats = rominfo::get_stats(pokemon_id, cart);
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    return 1;
  }

  if (verbose_level >= 1) {
    std::cout << "Information for " << pokemon_stats.name << ", ";
    std::cout << " ID (" << int(pokemon_stats.id) << ")";
    std::cout << " DexNo (" << int(pokemon_stats.dexno) << ")\n";
    std::cout << "Sprite data location\n";
    std::cout << "  BANK  " << gbhelp::hex_str(rominfo::sprite_banks[(pokemon_stats.id - 1)], 1) << '\n';
    std::cout << "  Front " << gbhelp::hex_str(pokemon_stats.front_sprite_offset, 2) << '\n';
    std::cout << "  Back  " << gbhelp::hex_str(pokemon_stats.back_sprite_offset, 2) << '\n';
    std::cout << "------------------------------------------------" << std::endl;
  }
  /////////////////////////////////////////////////////////////////////////////
  // Fetch and decode sprite data
  gbemu::Decoder decoder(cart, verbose_level);
  decoder.clear(0);
  decoder.clear(1);
  decoder.clear(2);
  decoder.set_bank(rominfo::sprite_banks[pokemon_stats.id - 1]);
  decoder.set_offset(pokemon_stats.front_sprite_offset);
  decoder.read_header();
  // std::cout << "width == " << int(decoder.width) << std::endl;
  // std::cout << "height == " << int(decoder.height) << std::endl;
  // std::cout << "swap_buffers == " << std::boolalpha << decoder.swap_buffers << std::endl;
  decoder.rle_decode(1);
  decoder.read_encoding_mode();
  // std::cout << "encoding_mode == " << int(decoder.encoding_mode) << std::endl;
  decoder.rle_decode(2);
  gbhelp::dump_ram(cart, "debug", "rle", true);

  decoder.delta_decode(decoder.primary_buffer);
  if (decoder.encoding_mode != 2) {
    decoder.delta_decode(decoder.secondary_buffer);
  }
  gbhelp::dump_ram(cart, "debug", "delta", true);

  if (decoder.encoding_mode != 1) {
    decoder.xor_planes();
  }
  gbhelp::dump_ram(cart, "debug", "xor", true);

  decoder.clear(0);
  decoder.copy(1, 0);
  decoder.clear(1);
  decoder.copy(2, 1);
  decoder.zip_planes();
  gbhelp::dump_ram(cart, "debug", "final", true);

  /////////////////////////////////////////////////////////////////////////////
  // Convert tile data and export image
  std::vector<std::uint8_t> sprite_data;
  std::copy(
    &cart.ram[392],
    &cart.ram[1176],
    std::back_inserter(sprite_data)
  );

  gbemu::Renderer renderer(sprite_data, decoder.width, decoder.height);
  renderer.interlace();
  renderer.expand();
  renderer.add_padding();
  renderer.transpose();

  std::stringstream ss;
  ss << std::setw(3) << std::setfill('0') << int(pokemon_stats.dexno) << ".";
  ss << pokemon_stats.name;
  renderer.save("output", ss.str(), gbemu::IMAGE_FORMAT::PGM, true);

  return 0;
}
