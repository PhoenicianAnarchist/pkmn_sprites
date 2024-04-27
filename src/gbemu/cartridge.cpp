#include <exception>

#include "../util/io.hpp"
#include "helpers.hpp"

#include "cartridge.hpp"

void Cartridge::load_rom(const std::filesystem::path &rom_path) {
  rom = loadFromFile(rom_path);

  std::copy(
    rom.begin(), rom.begin() + 0x4000,
    bank0.begin()
  );

  std::copy(
    rom.begin() + 0x4000, rom.begin() + 0x8000,
    bank1.begin()
  );

  ram.fill(0);
}

void Cartridge::switch_bank(std::uint8_t banknumber) {
  std::uint32_t offset = (banknumber * 0x4000);

  std::copy(
    rom.begin() + offset, rom.begin() + offset + 0x4000,
    bank1.begin()
  );
}

std::uint8_t Cartridge::read(std::uint16_t address) {
  // bank00
  if (address < 0x4000) {
    return bank0[address];
  }

  // switchable bank
  if ((address >= 0x4000) && (address < 0x8000)) {
    return bank1[address - 0x4000];
  }

  // cartridge ram
  if ((address >= 0xa000) && (address < 0xc000)) {
    return ram[address - 0xa000];
  }

  std::stringstream ss;
  ss << "read out of range @ " << gbhelp::hex_str(address, 2);

  throw std::out_of_range(ss.str());
}

void Cartridge::write(std::uint16_t address, std::uint8_t data) {
  if ((address >= 0xa000) && (address < 0xc000)) {
    ram[address - 0xa000] = data;
    return;
  }

  std::stringstream ss;
  ss << "write out of range @ " << gbhelp::hex_str(address, 2);

  throw std::out_of_range(ss.str());
}

std::uint16_t Cartridge::read_address(std::uint16_t address) {
  std::uint8_t low = read(address);
  std::uint8_t high = read(address + 1);

  std::uint16_t addr = high;
  addr <<= 8;
  addr |= low;

  return addr;
}

std::uint16_t Cartridge::read_address_from_table(
  std::uint16_t address, std::uint16_t index, std::uint16_t count
) {
  std::uint16_t offset = index * count;

  std::uint8_t low = read(address + offset);
  std::uint8_t high = read(address + offset + 1);

  std::uint16_t addr = high;
  addr <<= 8;
  addr |= low;

  return addr;
}

std::vector<std::uint8_t> Cartridge::read_from_table(
  std::uint16_t address, std::uint16_t index, std::uint16_t count
) {
  std::uint16_t offset = index * count;

  return read(address + offset, count);
}

std::vector<std::uint8_t> Cartridge::read(
  std::uint16_t address, std::uint16_t count
) {
  std::vector<std::uint8_t> data;
  for (std::uint16_t i = 0; i < count; ++i) {
    data.push_back(read(address + i));
  }

  return data;
}

std::vector<std::uint8_t> Cartridge::read_string(
  std::uint16_t address, std::uint8_t nullchar
) {
  std::vector<std::uint8_t> s;

  std::uint16_t index = address;
  std::uint8_t b;
  do {
    b = read(index);
    s.push_back(b);
    ++index;
  } while(b != nullchar);

  return s;
}

void Cartridge::write(
  std::uint16_t address, std::vector<std::uint8_t> data, std::uint16_t count
) {
  for (std::uint16_t i = 0; i < count; ++i) {
    write(address + i, data[i]);
  }
}
