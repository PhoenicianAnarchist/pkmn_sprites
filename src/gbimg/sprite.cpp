#include <iomanip>

#include "../util/image.hpp"
#include "sprite.hpp"

Sprite::Sprite(const std::vector<std::uint8_t> &raw_data) : data(raw_data) {
  std::size_t len = data.size();

  switch (len) {
    case 400: width = 40; height = 40; break;
    case 576: width = 48; height = 48; break;
    case 784: width = 56; height = 56; break;
    default: width = 0; height = 0;
  }

  auto interlaced = interlace(data);
  auto expanded = expand(interlaced);
  tiles = stream_to_tiles(expanded);
  data = tiles_to_stream(tiles, width / 8, height / 8);
}

void Sprite::save(const std::filesystem::path &filepath) const {
  save_pgm(data, width, height, filepath);
}

void Sprite::save_tiles(const std::filesystem::path &directory) const {
  // for debugging
  for (std::size_t t = 0; t < tiles.size(); ++t) {
    std::stringstream ss;
    ss << "tile" << std::setw(3) << std::setfill('0') << t << ".pgm";

    auto filepath = directory / ss.str();
    save_pgm(tiles[t], 8, 8, filepath);
  }
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
      s |= (a & (0b10000000 >> i)) >> (7 - i);

      s <<= 1;
      s |= (b & (0b10000000 >> i)) >> (7 - i);
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

      // swap 0b00 and 0b11
      if (a == 0b00000000) {
        a = 0b00000011;
      } else if (a == 0b00000011) {
        a = 0b00000000;
      }

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
