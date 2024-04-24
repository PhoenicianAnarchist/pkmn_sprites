#ifndef __BINARYREADER_HPP__
#define __BINARYREADER_HPP__

#include <vector>
#include <cstdint> // std::size_t

class BinaryReader {
public:
  BinaryReader() = default;
  BinaryReader(std::vector<std::uint8_t> &buffer);

  std::size_t size() const;
  std::vector<std::uint8_t> &data() const;

  void seek(std::size_t p);
  std::size_t tell() const;
  bool peek() const;
  bool get();
  void put(bool b);

  ////

  std::uint8_t get_pair();
  std::uint8_t get_nibble();
  std::uint8_t get_byte();

private:
  std::uint8_t get_bits(std::size_t n);
  std::vector<std::uint8_t> &buffer;

  std::size_t pointer;
  std::size_t bitcount;
};



#endif // __BINARYREADER_HPP__
