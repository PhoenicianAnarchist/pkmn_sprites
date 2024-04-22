#include <iostream>
#include <iomanip>
#include <sstream>

#include "../util/image.hpp"
#include "sprite.hpp"

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

Sprite::Sprite(const std::vector<std::uint8_t> &raw_data) : data(raw_data) {
  std::size_t len = data.size();

  switch (len) {
    case 400: width = 5; height = 5; break;
    case 576: width = 6; height = 6; break;
    case 784: width = 7; height = 7; break;
    default: width = 0; height = 0;
  }

  colour_palette = {
    {0xff, 0xff, 0xff},
    {0xaa, 0xaa, 0xaa},
    {0x55, 0x55, 0x55},
    {0x00, 0x00, 0x00}
  };

  auto interlaced = interlace(data);
  auto expanded = expand(interlaced);
  tiles = stream_to_tiles(expanded);
  data = tiles_to_stream(tiles, width, height);
}

void Sprite::save(
  const std::filesystem::path &directory, const std::string &name,
  IMAGE_FORMAT format, bool create_dirs
) const {
  std::stringstream ss;
  std::vector<std::uint8_t> output_data;
  std::filesystem::path filepath;

  switch (format) {
    case IMAGE_FORMAT::PGM:
      for (unsigned char b : data) {
        output_data.push_back(0b00000011 - b);
      }

      ss << name << ".pgm";
      filepath = directory / ss.str();
      save_pgm(output_data, width * 8, height * 8, filepath, create_dirs);

      break;

    case IMAGE_FORMAT::PPM:
      for (auto b : data) {
        auto c = colour_palette[b];
        for (std::size_t i = 0; i < 3; ++i) {
          output_data.push_back(c[i]);
        }
      }

      ss << name << ".ppm";
      filepath = directory / ss.str();
      save_ppm(output_data, width * 8, height * 8, filepath, create_dirs);

      break;
  }
}

void Sprite::save_tiles(
  const std::filesystem::path &directory, bool create_dirs
) const {
  // for debugging
  for (std::size_t t = 0; t < tiles.size(); ++t) {
    std::stringstream ss;
    ss << "tile" << std::setw(3) << std::setfill('0') << t << ".pgm";

    auto filepath = directory / ss.str();
    save_pgm(tiles[t], 8, 8, filepath, create_dirs);
  }
}

void Sprite::set_palette(
  const std::vector<std::array<std::uint8_t, 3>> &palette
) {
  colour_palette = palette;
}

std::vector<std::uint8_t> Sprite::interlace(
  const std::vector<std::uint8_t> &data
) {
  std::vector<std::uint8_t> zipped_data;

  for (std::size_t i = 0; i < data.size(); i += 2) {
    std::uint8_t a = data[i];
    std::uint8_t b = data[i + 1];

    std::uint16_t s;
    for (int i = 0; i <= 7; ++i) {
      s <<= 1;
      s |= (b & (0b10000000 >> i)) >> (7 - i);

      s <<= 1;
      s |= (a & (0b10000000 >> i)) >> (7 - i);
    }

    std::uint8_t c = (s & 0xff00) >> 8;
    std::uint8_t d = (s & 0x00ff);

    zipped_data.push_back(c);
    zipped_data.push_back(d);
  }

  return zipped_data;
}

std::vector<std::uint8_t> Sprite::expand(
  const std::vector<std::uint8_t> &data
) {
  std::vector<std::uint8_t> expanded_data;

  for (auto b : data) {
    for (std::size_t i = 0; i < 4; ++i) {
      std::uint8_t a = (b >> ((3 - i) * 2)) & 0b00000011;

      expanded_data.push_back(a);
    }
  }

  return expanded_data;
}

std::vector<std::vector<std::uint8_t>> Sprite::stream_to_tiles(
  const std::vector<std::uint8_t> &data
) {
  std::vector<std::vector<std::uint8_t>> tiles;

  // each tile is 8x8 pixels for 64 pixels
  for (std::size_t i = 0; i < data.size(); i += 64) {
    std::vector<std::uint8_t> tile;

    std::copy(
      data.begin() + i,
      data.begin() + i + 64,
      std::back_inserter(tile)
    );

    tiles.push_back(tile);
  }

  return tiles;
}

std::vector<std::uint8_t> Sprite::tiles_to_stream(
  const std::vector<std::vector<std::uint8_t>> &data,
  std::size_t width, std::size_t height
) {
  std::vector<std::uint8_t> merged;

  // need to pre-allocate vector for index based access
  merged.resize(data.size() * 64);

  // tiles are stored in column order.
  for (std::size_t col = 0; col < width; ++col) {
    std::size_t tile_offset = col * height;
    std::size_t column_offset = col * 8;

    // for each tile in the column
    for (std::size_t row = 0; row < height; ++row) {
      std::size_t tile_index = row + tile_offset;
      std::size_t row_offset = row * 8;

      // calculate top-left pixel index of current tile in destination
      std::size_t topleft = column_offset + (row_offset * width * 8);

      // for each row in tile
      for (std::size_t y = 0; y < 8; ++y) {
        std::size_t dest_offset = topleft + (y * width * 8);
        std::size_t pixel_offset = y * 8;

        // copy each pixel of row over
        for (std::size_t x = 0; x < 8; ++x) {
          merged[dest_offset + x] = data[tile_index][pixel_offset + x];
        }
      }
    }
  }

  return merged;
}
