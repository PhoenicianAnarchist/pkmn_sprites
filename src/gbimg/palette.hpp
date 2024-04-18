#ifndef __PALETTE_HPP__
#define __PALETTE_HPP__

namespace palettes {
  namespace gb {
    std::vector<std::array<std::uint8_t, 3>> greyscale = {
      {0xff, 0xff, 0xff},
      {0xaa, 0xaa, 0xaa},
      {0x55, 0x55, 0x55},
      {0x00, 0x00, 0x00}
    };

    std::vector<std::array<std::uint8_t, 3>> dmg_01 = {
      {0xb8, 0xf8, 0x78},
      {0x80, 0xb0, 0x50},
      {0x48, 0x68, 0x28},
      {0x10, 0x20, 0x00}
    };
  }

  namespace gbc {
    std::array<std::uint8_t, 3> light = {0xff, 0xff, 0xff};
    std::array<std::uint8_t, 3> dark  = {0x00, 0x00, 0x00};

    std::vector<std::array<std::uint8_t, 3>> red = {
      light, {0xff, 0x84, 0x84}, {0x94, 0x3a, 0x3a}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> blue = {
      light, {0x63, 0xa5, 0xff}, {0x00, 0x00, 0xff}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> green = {
      light, {0x7b, 0xff, 0x31}, {0x00, 0x63, 0xc5}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> yellow = {
      light, {0xff, 0xff, 0x00}, {0xff, 0x00, 0x00}, dark
    };
  }

  namespace sgb {
    std::array<std::uint8_t, 3> light = {0xff, 0xef, 0xff};
    std::array<std::uint8_t, 3> dark  = {0x19, 0x10, 0x10};

    std::vector<std::array<std::uint8_t, 3>> green = {
      light, {0xa5, 0xd6, 0x84}, {0x4a, 0xa5, 0x5a}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> red = {
      light, {0xff, 0xa5, 0x52}, {0xd6, 0x52, 0x31}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> cyan = {
      light, {0xad, 0xce, 0xef}, {0x73, 0x9c, 0xce}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> yellow = {
      light, {0xff, 0xe6, 0x73}, {0xd6, 0xa5, 0x00}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> brown = {
      light, {0xe6, 0xa5, 0x7b}, {0xad, 0x73, 0x4a}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> gray = {
      light, {0xd6, 0xad, 0xb5}, {0x7b, 0x7b, 0x94}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> purple = {
      light, {0xde, 0xb5, 0xc5}, {0xad, 0x7b, 0xbd}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> blue = {
      light, {0x94, 0xa5, 0xde}, {0x5a, 0x7b, 0xbd}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> pink = {
      light, {0xf7, 0xb5, 0xc5}, {0xe6, 0x7b, 0xad}, dark
    };

    std::vector<std::array<std::uint8_t, 3>> mew = {
      light, {0xf7, 0xb5, 0x8c}, {0x84, 0x73, 0x9c}, dark
    };
  }
}

#endif // __PALETTE_HPP__
