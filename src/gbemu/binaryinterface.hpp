#ifndef __GBEMU_BINARYINTERFACE_HPP__
#define __GBEMU_BINARYINTERFACE_HPP__

#include <array>
#include <cstdint> // std::size_t

namespace gbemu {
  class BinaryInterface {
  public:
    BinaryInterface(std::array<std::uint8_t, 0x4000> &buffer);
    void attach_buffer(std::array<std::uint8_t, 0x4000> &buffer);

    std::size_t size() const;
    std::array<std::uint8_t, 0x4000> &data() const;

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
    std::array<std::uint8_t, 0x4000> &buffer;
    std::size_t pointer;
  };
}


#endif // __GBEMU_BINARYINTERFACE_HPP__
