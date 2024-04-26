#ifndef __OPTIONS_HPP__
#define __OPTIONS_HPP__
#include <filesystem>

#include <cstdint>

struct OPTIONS {
  int err;
  bool called_for_help;
  std::filesystem::path rom_path;
  std::filesystem::path output_path;
  std::uint8_t index;
  std::uint8_t dexno;
  bool create_dirs;
};

OPTIONS parse_command_line(int argc, char *argv[]);

#endif // __OPTIONS_HPP__
