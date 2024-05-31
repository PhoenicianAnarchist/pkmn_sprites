#include <bitset>

#include "binaryinterface.hpp"

gbemu::BinaryInterface::BinaryInterface(
  std::array<std::uint8_t, 0x4000> &buffer
) : buffer(buffer), pointer(0) {}


void gbemu::BinaryInterface::attach_buffer(
  std::array<std::uint8_t, 0x4000> &new_buffer
) {
  buffer = new_buffer;
}

std::array<std::uint8_t, 0x4000> &gbemu::BinaryInterface::data() const {
  return buffer;
}

void gbemu::BinaryInterface::seek(std::size_t p) {
  pointer = p;
}

std::size_t gbemu::BinaryInterface::tell() const {
  return pointer;
}

bool gbemu::BinaryInterface::peek() const {
  std::size_t byte_index = pointer / 8;
  std::size_t bit_index = 7 - (pointer % 8);
  std::uint8_t byte = buffer[byte_index];
  byte >>= bit_index;
  byte &= 0b00000001;

  return byte == 0b00000001;
}

bool gbemu::BinaryInterface::get() {
  bool b = peek();

  ++pointer;

  return b;
}

void gbemu::BinaryInterface::put(bool b) {
  std::size_t byte_index = pointer / 8;
  std::size_t bit_index = 7 - (pointer % 8);
  std::uint8_t byte = buffer[byte_index];
  std::uint8_t mask = 0b00000001 << bit_index;
  std::uint8_t masked = byte & (~mask);

  buffer[byte_index] = b ? (masked |= mask) : masked;

  ++pointer;
}

std::uint8_t gbemu::BinaryInterface::get_pair() {
  return get_bits(2);
}

std::uint8_t gbemu::BinaryInterface::get_nibble() {
  return get_bits(4);
}

std::uint8_t gbemu::BinaryInterface::get_byte() {
  return get_bits(8);
}

std::uint8_t gbemu::BinaryInterface::get_bits(std::size_t n) {
  std::uint8_t p = 0;

  for (std::size_t i = 0; i < n; ++i) {
    p <<= 1;
    p |= get();
  }

  return p;
}
