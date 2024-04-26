#include <iostream>
#include <iomanip>
#include <sstream>

#include "helpers.hpp"

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
