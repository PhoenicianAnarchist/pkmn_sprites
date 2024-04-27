#include <iostream>
#include <iomanip>
#include <sstream>

#include "helpers.hpp"
#include "../util/image.hpp"

std::string gbhelp::hex_str(
  std::uint64_t addr, std::size_t byte_count, bool show_base
) {
  std::stringstream ss;

  if (show_base) {
    ss << "0x";
  }

  ss << std::setw(byte_count * 2) << std::setfill('0') << std::hex << addr;
  return ss.str();
}

std::uint32_t gbhelp::absolute_address(
  std::uint8_t bank, std::uint16_t offset, std::uint16_t bank_size
) {
  if ((offset > bank_size) && (offset < (bank_size * 2))) {
    std::uint32_t addr = bank_size * bank;
    addr += (offset - bank_size);
    return addr;
  }

  return offset;
}

std::string gbhelp::decode_string(
  const std::vector<std::uint8_t> &raw_data,
  const std::map<std::uint8_t, std::string> &charactermap,
  std::uint8_t eos_char
) {
  std::stringstream ss;

  for (std::uint8_t b : raw_data) {
    if (b == eos_char) {
      break;
    }
    try {
      ss << charactermap.at(b);
    } catch (std::out_of_range &e) {
      ss << "·";
      std::cout << "unknown character index " << hex_str(b, 1) << "\n";
    }
  }

  return ss.str();
}

void gbhelp::dump_ram(
  Cartridge &cart, const std::filesystem::path &output_directory,
  bool create_dirs
) {
  // dump scprite scratch ram to a texture, in column order
  std::array<std::uint8_t, 0x4000> &ram = cart.ram;

  // onle need a new buffer for 3 * 392 bytes
  std::array<std::uint8_t, 0x0498> transposed;

  for (std::size_t col = 0; col < 21; ++col) {
    std::size_t offset = col * 56;
    for (std::size_t row = 0; row < 56; ++row) {
      std::size_t index = row + offset;
      std::size_t transposed_index = col + (row * 21);

      transposed[transposed_index] = ram[index];
    }
  }

  std::vector<std::uint8_t> image_data;
  for (auto b : transposed) {
    for (int i = 8; i > 0; --i) {
      std::uint8_t new_byte;
      new_byte = (b >> (i - 1)) & 0b1;
      image_data.push_back(new_byte);
    }
  }

  std::filesystem::path filepath = output_directory / "ram.pgm";
  save_pgm(image_data, 168, 56, filepath, true, "1");
}
