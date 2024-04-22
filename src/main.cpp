#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <bitset>
#include <array>
#include <vector>

#include <cstdint> // std::uint8_t

#include "gbimg/decoder.hpp"
#include "gbimg/palette.hpp"
#include "gbimg/sprite.hpp"
#include "util/image.hpp"
#include "util/io.hpp"
#include "util/options.hpp"

void render_bitplane(
  const std::vector<std::uint8_t> &data, std::size_t width, std::size_t height,
  const std::filesystem::path &output_path, const std::string &name,
  bool create_dirs
);

void render_sprite(
  const std::vector<std::uint8_t> &data,
  const std::filesystem::path &output_path, bool create_dirs
);

int main(int argc, char *argv[]) {
  OPTIONS options = parse_command_line(argc, argv);

  if (options.err) {
    return options.err;
  } else if (options.called_for_help) {
    return 0;
  }

  std::vector<std::uint8_t> raw_data = loadFromFile(options.image_path);
  Decoder dec(raw_data);

  render_bitplane(
    dec.plane_0, dec.width, dec.height, options.output_path, "plane_0",
    options.create_dirs
  );
  render_bitplane(
    dec.plane_1, dec.width, dec.height, options.output_path, "plane_1",
    options.create_dirs
  );

  return 0;
}

void render_bitplane(
  const std::vector<std::uint8_t> &data, std::size_t width, std::size_t height,
  const std::filesystem::path &output_path, const std::string &name,
  bool create_dirs
) {
  // bitplane data is already in the correct orientation
  std::vector<std::uint8_t> sprite_data;

  for (auto b : data) {
    sprite_data.push_back(b);
    sprite_data.push_back(b);
  }

  auto interlaced = Sprite::interlace(sprite_data);
  auto expanded = Sprite::expand(interlaced);
  sprite_data = expanded;

  std::vector<std::uint8_t> output_data;
  for (unsigned char b : sprite_data) {
    output_data.push_back(0b00000011 - b);
  }

  std::stringstream ss;
  ss << name << ".pgm";
  std::filesystem::path filepath = output_path / ss.str();
  save_pgm(output_data, width * 8, height * 8, filepath, create_dirs);
}

void render_sprite(
  const std::vector<std::uint8_t> &data,
  const std::filesystem::path &output_path, bool create_dirs
) {
  // renders decoded tile data
  Sprite sprite(data);

  sprite.save(output_path, "grey", IMAGE_FORMAT::PGM, create_dirs);

  sprite.set_palette(palettes::gbc::red);
  sprite.save(output_path, "pkmn_red", IMAGE_FORMAT::PPM, create_dirs);
}
