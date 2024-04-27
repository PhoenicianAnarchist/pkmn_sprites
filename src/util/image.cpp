#include "io.hpp"

#include "image.hpp"

int save_pgm(
  const std::vector<std::uint8_t> &data, std::size_t width, std::size_t height,
  const std::filesystem::path &filepath, bool create_dirs,
  const std::string &max_value
) {
  std::vector<std::uint8_t> image_data;

  std::vector<std::uint8_t> header;
  header.push_back('P');
  header.push_back('5');
  header.push_back('\n');

  std::string w_str = std::to_string(width);
  std::copy(w_str.begin(), w_str.end(), std::back_inserter(header));
  header.push_back(' ');
  std::string h_str = std::to_string(height);
  std::copy(h_str.begin(), h_str.end(), std::back_inserter(header));

  header.push_back('\n');
  std::copy(max_value.begin(), max_value.end(), std::back_inserter(header));
  // header.push_back('3'); // max colours. 3 = 2bpp
  header.push_back('\n');

  std::copy(header.begin(), header.end(), std::back_inserter(image_data));

  std::copy(data.begin(), data.end(), std::back_inserter(image_data));

  return writeToFile(filepath, image_data, create_dirs);
}

int save_ppm(
  const std::vector<std::uint8_t> &data, std::size_t width, std::size_t height,
  const std::filesystem::path &filepath, bool create_dirs,
  const std::string &max_value
) {
  std::vector<std::uint8_t> image_data;

  std::vector<std::uint8_t> header;
  header.push_back('P');
  header.push_back('6');
  header.push_back('\n');

  std::string w_str = std::to_string(width);
  std::copy(w_str.begin(), w_str.end(), std::back_inserter(header));
  header.push_back(' ');
  std::string h_str = std::to_string(height);
  std::copy(h_str.begin(), h_str.end(), std::back_inserter(header));

  header.push_back('\n');
  std::copy(max_value.begin(), max_value.end(), std::back_inserter(header));
  header.push_back('\n');

  std::copy(header.begin(), header.end(), std::back_inserter(image_data));

  std::copy(data.begin(), data.end(), std::back_inserter(image_data));

  return writeToFile(filepath, image_data, create_dirs);
}
