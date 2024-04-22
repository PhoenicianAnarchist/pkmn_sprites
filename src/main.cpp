#include <iostream>
#include <iomanip>
#include <filesystem>
#include <vector>

#include <cstdint> // std::uint8_t

#include "gbimg/decoder.hpp"
#include "gbimg/palette.hpp"
#include "gbimg/sprite.hpp"
#include "util/io.hpp"
#include "util/options.hpp"

int main(int argc, char *argv[]) {
  OPTIONS options = parse_command_line(argc, argv);

  if (options.err) {
    return options.err;
  } else if (options.called_for_help) {
    return 0;
  }

  // decompress and decode raw data
  std::vector<std::uint8_t> raw_data = loadFromFile(options.image_path);
  Decoder dec(raw_data, options.output_path, options.create_dirs, true);

  // render sprite from decoded tile data
  // std::vector<std::uint8_t> raw_data = loadFromFile(options.image_path);
  // Sprite sprite(data);
  // sprite.save(output_path, "grey", IMAGE_FORMAT::PGM, create_dirs);
  // sprite.set_palette(palettes::gbc::red);
  // sprite.save(output_path, "pkmn_red", IMAGE_FORMAT::PPM, create_dirs);

  return 0;
}
