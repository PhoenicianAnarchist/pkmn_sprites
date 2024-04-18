#include <iostream>
#include <fstream>

#include "io.hpp"

std::vector<std::uint8_t> loadFromFile(const std::filesystem::path& path) {
  std::ifstream ifs;
  ifs.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  std::vector<std::uint8_t> data;
  std::vector<char> buffer;

  try {
    ifs.open(path, std::ios::in | std::ios::binary | std::ios::ate);
    std::size_t size = ifs.tellg();
    ifs.seekg(0);

    // read needs a char type
    buffer.resize(size);
    ifs.read(buffer.data(), size);
    ifs.close();
  } catch (std::ifstream::failure &e) {
    std::cerr << e.what() << ", failed to read file: " << path << std::endl;
    return {};
  }

  // convert chars to std::uint8_ts
  for (char c : buffer) {
    data.push_back(static_cast<std::uint8_t>(c));
  }

  return data;
}

int writeToFile(
  const std::filesystem::path &path, const std::vector<std::uint8_t> &data,
  bool create_dirs
) {
  std::ofstream ofs;
  ofs.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  std::vector<char> buffer;

  for (std::uint8_t c : data) {
    buffer.push_back(static_cast<char>(c));
  }

  if (create_dirs) {
    auto dir = path.parent_path();
    if (!std::filesystem::exists(dir)) {
      std::filesystem::create_directories(dir);
    }
  }

  try {
    ofs.open(path, std::ios::out | std::ios::binary);
    ofs.write(buffer.data(), buffer.size());
    ofs.close();
  } catch (std::ofstream::failure &e) {
    std::cerr << e.what() << ", failed to write file: " << path << std::endl;
    return 1;
  }

  return 0;
}
