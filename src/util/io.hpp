#ifndef __IO_HPP__
#define __IO_HPP__

#include <filesystem>
#include <vector>

#include <cstdint> // std::uint8_t

std::vector<std::uint8_t> loadFromFile(const std::filesystem::path &path);
int writeToFile(
  const std::filesystem::path &path, const std::vector<std::uint8_t> &data
);

#endif // __IO_HPP__
