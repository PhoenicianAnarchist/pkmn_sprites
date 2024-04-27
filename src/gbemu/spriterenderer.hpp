#ifndef __GBEMU_SPRITE_RENDERER_HPP__
#define __GBEMU_SPRITE_RENDERER_HPP__

#include <array>
#include <filesystem>
#include <string>
#include <vector>

#include <cstdint>

namespace gbemu {
  enum class IMAGE_FORMAT {
    PGM,
    PPM
  };

  class Renderer {
  public:
    Renderer(
      const std::vector<std::uint8_t> &data, std::uint8_t width,
      std::uint8_t height
    );

    void set_palette(
      const std::vector<std::array<std::uint8_t, 3>> &palette
    );

    void interlace();
    void expand();
    void add_padding();
    void transpose();

    void save(
      const std::filesystem::path &directory, const std::string &name,
      IMAGE_FORMAT format, bool create_dirs
    );
  private:
    std::vector<std::uint8_t> data;
    std::uint8_t width;
    std::uint8_t height;
    std::vector<std::array<std::uint8_t, 3>> colour_palette;
  };
}

#endif // __GBEMU_SPRITE_RENDERER_HPP__
