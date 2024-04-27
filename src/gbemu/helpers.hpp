#ifndef __GBEMU_HELPERS__
#define __GBEMU_HELPERS__

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <cstdint>

#include "cartridge.hpp"

namespace gbhelp {
  std::string hex_str(
    std::uint64_t addr, std::size_t byte_count=2, bool show_base=true
  );

  std::uint32_t absolute_address(
    std::uint8_t bank, std::uint16_t offset, std::uint16_t bank_size=0x4000
  );

  std::string decode_string(
    const std::vector<std::uint8_t> &raw_data,
    const std::map<std::uint8_t, std::string> &charactermap,
    std::uint8_t eos_char
  );

  void dump_ram(
    Cartridge &cart, const std::filesystem::path &output_directory,
    bool create_dirs
  );
}


#endif // __GBEMU_HELPERS__
