#ifndef __SPRITE_HPP__
#define __SPRITE_HPP__

#include <array>
#include <filesystem>
#include <vector>

#include <cstdint> // std::uint8_t

enum class IMAGE_FORMAT {
  PGM,
  PPM
};

class Sprite {
public:
  Sprite(const std::vector<std::uint8_t> &rd);
  void save(
    const std::filesystem::path &directory, const std::string &name,
    IMAGE_FORMAT format=IMAGE_FORMAT::PPM, bool create_dirs=false
  ) const;
  void save_tiles(
    const std::filesystem::path &directory, bool create_dirs=false
  ) const;
  void set_palette(const std::vector<std::array<std::uint8_t, 3>> &palette);

  // each two bytes are the "high" and "low" bits for eight 2bpp pixels
  static std::vector<std::uint8_t> interlace(
    const std::vector<std::uint8_t> &data
  );
  // expand each byte (of four byte-pairs) into four bytes
  static std::vector<std::uint8_t> expand(
    const std::vector<std::uint8_t> &data
  );
  // data is stored as a sequence of column-major 8x8 pixel tiles
  // brute force method to re-arrange binary data
  // seems to be good enough for now
  static std::vector<std::vector<std::uint8_t>> stream_to_tiles(
    const std::vector<std::uint8_t> &data
  );
  static std::vector<std::uint8_t> tiles_to_stream(
    const std::vector<std::vector<std::uint8_t>> &data,
    std::size_t width, std::size_t height
  );
private:
  std::uint8_t width;
  std::uint8_t height;

  std::vector<std::uint8_t> data;
  std::vector<std::vector<std::uint8_t>> tiles;
  std::vector<std::array<std::uint8_t, 3>> colour_palette;
};

#endif // __SPRITE_HPP__
