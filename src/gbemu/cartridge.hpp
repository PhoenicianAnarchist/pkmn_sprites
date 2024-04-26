#ifndef __CARTRIDGE_HPP__
#define __CARTRIDGE_HPP__

#include <filesystem>
#include <array>
#include <vector>

#include <cstdint> // std::uint8_t

class Cartridge {
public:
  Cartridge() = default;

  void load_rom(const std::filesystem::path &rom_path);
  void switch_bank(std::uint8_t banknumber);

  std::uint8_t read(std::uint16_t address);
  void write(std::uint16_t address, std::uint8_t data);

  std::array<std::uint8_t, 0x2000> dump_ram() const;

  /////

  // read (and swap) two bytes
  std::uint16_t read_address(std::uint16_t address);
  std::uint16_t read_address_from_table(
    std::uint16_t address, std::uint16_t index, std::uint16_t count=2
  );

  // get data from indexed table
  std::vector<std::uint8_t> read_from_table(
    std::uint16_t address, std::uint16_t index, std::uint16_t count
  );

  // read a null-terminated string
  std::vector<std::uint8_t> read_string(
    std::uint16_t address, std::uint8_t nullchar=0x00
  );

  // read/write multiple bytes
  std::vector<std::uint8_t> read(std::uint16_t address, std::uint16_t count);
  void write(
    std::uint16_t address, std::vector<std::uint8_t> data, std::uint16_t count
  );

private:
  std::vector<std::uint8_t> rom;

  std::array<std::uint8_t, 0x4000> bank0;
  std::array<std::uint8_t, 0x4000> bank1;
  std::array<std::uint8_t, 0x2000> ram;
};

#endif // __CARTRIDGE_HPP__
