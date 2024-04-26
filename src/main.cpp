#include <iostream>
#include <iomanip>
#include <filesystem>
#include <sstream>
#include <vector>

#include <cstdint> // std::uint8_t

#include "gbemu/cartridge.hpp"
#include "gbemu/helpers.hpp"
#include "gbemu/pokemon_red.hpp"

#include "gbimg/decoder.hpp"
#include "gbimg/palette.hpp"
#include "gbimg/sprite.hpp"
#include "util/image.hpp"
#include "util/io.hpp"
#include "util/options.hpp"

void decompress_raw(
  const std::filesystem::path &path_to_data,
  const std::filesystem::path &output_directory, bool create_dirs
);

void render_tile_data(
  const std::filesystem::path &path_to_data,
  const std::filesystem::path &output_directory, bool create_dirs
);

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

  // decompress_raw(
  //   options.image_path, options.output_path, options.create_dirs
  // );

  // render_tile_data(
  //   options.image_path, options.output_path, options.create_dirs
  // );

  return 0;
}

void decompress_raw(
  const std::filesystem::path &path_to_data,
  const std::filesystem::path &output_directory, bool create_dirs
) {
  // decompress and decode raw data
  std::vector<std::uint8_t> raw_data = loadFromFile(path_to_data);
  Decoder dec(raw_data, output_directory, create_dirs, true);

  // NOTE: data is not in the correct format
  // TODO: rewrite renderer to use a more accurate memory layout

  auto interlaced = Sprite::interlace(dec.data);
  auto expanded = Sprite::expand(interlaced);
  std::vector<std::uint8_t> output_data;
  for (unsigned char b : expanded) {
    output_data.push_back(0b00000011 - b);
  }
  std::filesystem::path filepath = output_directory / "output.pgm";
  save_pgm(output_data, dec.width * 8, dec.height * 8, filepath, true);
}

void render_tile_data(
  const std::filesystem::path &path_to_data,
  const std::filesystem::path &output_directory, bool create_dirs
) {
  // render sprite from decoded tile data
  std::vector<std::uint8_t> data = loadFromFile(path_to_data);
  Sprite sprite(data);
  sprite.save(output_directory, "grey", IMAGE_FORMAT::PGM, create_dirs);
  sprite.set_palette(palettes::gbc::red);
  sprite.save(output_directory, "pkmn_red", IMAGE_FORMAT::PPM, create_dirs);
}
