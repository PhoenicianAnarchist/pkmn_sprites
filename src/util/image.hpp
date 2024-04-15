#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <filesystem>
#include <vector>

#include <cstdint> // std::uint8_t

int save_pgm(
  const std::vector<std::uint8_t> &data, std::size_t width, std::size_t height,
  const std::filesystem::path &filepath
);


#endif // __IMAGE_HPP__
