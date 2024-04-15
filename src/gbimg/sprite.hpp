#ifndef __SPRITE_HPP__
#define __SPRITE_HPP__

#include <filesystem>
#include <vector>

#include <cstdint> // std::uint8_t

class Sprite {
public:
  Sprite(const std::vector<std::uint8_t> &rd);
  void save(const std::filesystem::path &filepath) const;
  void save_tiles(const std::filesystem::path &directory) const;

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
};

#endif // __SPRITE_HPP__
