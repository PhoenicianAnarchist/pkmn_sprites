#include <bitset>
#include <algorithm>
#include <iostream>
#include "../util/image.hpp"

#include "spriterenderer.hpp"

gbemu::Renderer::Renderer(
  const std::vector<std::uint8_t> &data, std::uint8_t width,
  std::uint8_t height
) : data(data), width(width), height(height) {
  colour_palette = {
    {0xff, 0xff, 0xff},
    {0xaa, 0xaa, 0xaa},
    {0x55, 0x55, 0x55},
    {0x00, 0x00, 0x00}
  };
}

void gbemu::Renderer::set_palette(
  const std::vector<std::array<std::uint8_t, 3>> &palette
) {
  colour_palette = palette;
}

void gbemu::Renderer::interlace() {
  // each two bytes are the low and high bits for 4 pixels

  std::vector<std::uint8_t> zipped_data;
  zipped_data.reserve(data.size());

  for (std::size_t i = 0; i < data.size(); i += 2) {
    std::uint8_t a = data[i];
    std::uint8_t b = data[i + 1];

    std::uint16_t s;
    for (std::size_t i = 0; i <= 7; ++i) {
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

  data = zipped_data;
}

void gbemu::Renderer::expand() {
  // each byte is expanded into four pixels
  std::vector<std::uint8_t> expanded_data;
  expanded_data.reserve(data.size() * 4);

  for (auto b : data) {
    for (std::size_t i = 0; i < 4; ++i) {
      std::uint8_t a = (b >> ((3 - i) * 2)) & 0b00000011;

      expanded_data.push_back(a);
    }
  }

  data = expanded_data;
}

void gbemu::Renderer::transpose() {
  std::vector<std::uint8_t> transposed_data;
  transposed_data.resize(data.size());

  // for each tile (8x8 pixels, 64 bytes) stored in column order

  std::uint8_t num_tiles = width * height;

  for (std::size_t tile_index = 0; tile_index < num_tiles; ++tile_index) {
    // get tile data from stream
    std::vector<std::uint8_t> tile_data;
    std::copy(
      &data[tile_index * 64],
      &data[(tile_index + 1) * 64],
      std::back_inserter(tile_data)
    );

    // put tile data into new array
    // get tile position
    std::size_t top = tile_index % height;
    std::size_t left = tile_index / height;

    // top left pixel of current tile.
    std::size_t row_pixel_count = width * 8;
    std::size_t offset = (top * 8) * row_pixel_count;
    std::size_t index = (left * 8) + offset;

    for (std::size_t row = 0; row < 8; ++row) {
      std::size_t src_offset = row * 8;
      std::size_t dst_offset = row * row_pixel_count;
      for (std::size_t col = 0; col < 8; ++col) {
        std::size_t src_index = col + src_offset;
        std::size_t dst_index = col + dst_offset;

        transposed_data[index + dst_index] = tile_data[src_index];
      }
    }
  }

  data = transposed_data;
}

void gbemu::Renderer::save(
  const std::filesystem::path &directory, const std::string &name,
  IMAGE_FORMAT format, bool create_dirs
) {
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
