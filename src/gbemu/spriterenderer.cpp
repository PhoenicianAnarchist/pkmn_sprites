#include <exception>
#include <bitset>
#include <algorithm>
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

void gbemu::Renderer::add_padding() {
  std::vector<std::uint8_t> padded_data;
  std::uint8_t pad_left = int(((7 - width) / 2.0) + 0.5);
  std::uint8_t pad_right  = int((7 - width) / 2.0);
  std::uint8_t pad_top = 7 - height;

  std::array<std::uint8_t, 64> blank;
  blank.fill(0x00);
  std::array<std::uint8_t, 64> filled;
  filled.fill(0x03);

  while (pad_left > 0) {
    for (std::size_t b = 0; b < 7; ++b) {
      std::copy(blank.begin(), blank.end(), std::back_inserter(padded_data));
    }
    --pad_left;
  }

  for (std::size_t i = 0; i < width; ++i) {
    if (pad_top != 0) {
      for (std::size_t b = 0; b < pad_top; ++b) {
        std::copy(blank.begin(), blank.end(), std::back_inserter(padded_data));
      }
    }
    std::size_t tile_offset = i * height;
    for (std::size_t d = 0; d < height; ++d) {
      std::size_t tile_index = (d + tile_offset);
      std::size_t pixel_offset = tile_index * 64;

      std::copy(
        data.begin() + pixel_offset,
        data.begin() + pixel_offset + 64,
        std::back_inserter(padded_data)
      );
    }
  }

  while (pad_right > 0) {
    for (std::size_t b = 0; b < 7; ++b) {
      std::copy(blank.begin(), blank.end(), std::back_inserter(padded_data));
    }
    --pad_right;
  }

  width = 7;
  height = 7;
  data = padded_data;
}

void gbemu::Renderer::transpose() {
  std::vector<std::uint8_t> transposed_data;
  transposed_data.resize(data.size());

  // for each tile (8x8 pixels, 64 bytes) stored in column order

  std::uint8_t num_tiles = width * height;

  for (std::size_t tile_index = 0; tile_index < num_tiles; ++tile_index) {
    // data for each tile (64 bytes) is stored sequntially
    std::size_t tile_offset = tile_index * 64;

    // need to advance a whole image row for each row in the tile data
    std::size_t row_pixel_count = width * 8;

    // convert tile_index into pixel x and y coords
    std::size_t x = (tile_index / height) * 8;
    std::size_t y = (tile_index % height) * 8;

    // calculate offset of top-left pixel in tile data
    std::size_t dst_offset = x + (y * row_pixel_count);
    // std::cout << dst_offset << std::endl;

    for (std::size_t row = 0; row < 8; ++row) {
      std::size_t pixel_offset = row * 8;

      // count of pixels in the number of full rows needed to advance
      std::size_t row_offset = row * row_pixel_count;
      for (std::size_t col = 0; col < 8; ++col) {
        std::size_t src_index = tile_offset + pixel_offset + col;
        std::size_t dst_index = dst_offset + row_offset + col;

        transposed_data[dst_index] = data[src_index];
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
