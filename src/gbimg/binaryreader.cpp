#include <iostream>
#include <bitset>

#include "binaryreader.hpp"

std::ostream &operator<<(std::ostream &os, std::uint8_t b) {
  os << "0b" << std::bitset<8>(b);
  return os;
}

BinaryReader::BinaryReader(std::vector<std::uint8_t> &buffer)
: buffer(buffer), pointer(0) {
  bitcount = buffer.size() * 8;
}

std::size_t BinaryReader::size() const {
  return bitcount;
}

std::vector<std::uint8_t> &BinaryReader::data() const {
  return buffer;
}

void BinaryReader::seek(std::size_t p) {
  pointer = p;
}

std::size_t BinaryReader::tell() const {
  return pointer;
}

bool BinaryReader::peek() const {
  std::size_t byte_index = pointer / 8;
  std::size_t bit_index = 7 - (pointer % 8);
  std::uint8_t byte = buffer[byte_index];
  byte >>= bit_index;
  byte &= 0b00000001;

  return byte == 0b00000001;
}

bool BinaryReader::get() {
  bool b = peek();

  ++pointer;

  return b;
}

void BinaryReader::put(bool b) {
  std::size_t byte_index = pointer / 8;
  std::size_t bit_index = 7 - (pointer % 8);
  std::uint8_t byte = buffer[byte_index];
  std::uint8_t mask = 0b00000001 << bit_index;
  std::uint8_t masked = byte & (~mask);

  buffer[byte_index] = b ? (masked |= mask) : masked;

  ++pointer;
}

std::uint8_t BinaryReader::get_pair() {
  return get_bits(2);
}

std::uint8_t BinaryReader::get_nibble() {
  return get_bits(4);
}

std::uint8_t BinaryReader::get_byte() {
  return get_bits(8);
}

std::uint8_t BinaryReader::get_bits(std::size_t n) {
  std::uint8_t p = 0;

  for (std::size_t i = 0; i < n; ++i) {
    p <<= 1;
    p |= get();
  }

  return p;
}
