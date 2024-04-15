#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <bitset>
#include <array>
#include <vector>

#include <cstdint> // std::uint8_t

#include "gbimg/sprite.hpp"
#include "util/io.hpp"
#include "util/options.hpp"

int main(int argc, char *argv[]) {
  OPTIONS options = parse_command_line(argc, argv);

  if (options.err) {
    return options.err;
  } else if (options.called_for_help) {
    return 0;
  }

  std::vector<std::uint8_t> raw_data = loadFromFile(options.image_path);
  Sprite sprite(raw_data);

  sprite.save("out.pgm");
  // sprite.save_tiles("debug");

  // todo: change to output full colour .ppm (with palette options!)

  return 0;
}
