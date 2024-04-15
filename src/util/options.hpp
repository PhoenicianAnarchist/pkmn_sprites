#ifndef __OPTIONS_HPP__
#define __OPTIONS_HPP__
#include <filesystem>

struct OPTIONS {
  int err;
  bool called_for_help;
  std::filesystem::path image_path;
};

OPTIONS parse_command_line(int argc, char *argv[]);

#endif // __OPTIONS_HPP__
