#include <iostream>
#include <vector>

#include <cstdint> // std::uint8_t

#include "gbimg/binaryreader.hpp"
#include "util/options.hpp"

int initialisation_data_test(
  BinaryReader &reader, const std::vector<std::uint8_t> &expected
);
int initialisation_size_test(BinaryReader &reader, std::size_t expected);
int seek_tell_test(BinaryReader &reader, std::size_t expected);
int peek_test(BinaryReader &reader, std::size_t expected);
int get_test(
  BinaryReader &reader, std::size_t expected_a, std::size_t expected_b
);
int put_test(BinaryReader &reader, std::size_t data, std::size_t expected);
int extended_get_test(
  BinaryReader &reader, std::size_t index, std::uint8_t expected
);

int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> data = {0x55, 0xaa, 0xff};
  BinaryReader reader(data);

  initialisation_data_test(reader, data);
  initialisation_size_test(reader, data.size() * 8);
  seek_tell_test(reader, 3);
  peek_test(reader, 1);
  get_test(reader, 1, 0);

  extended_get_test(reader, 4, 0x5a);

  return 0;
}

int initialisation_data_test(
  BinaryReader &reader, const std::vector<std::uint8_t> &expected
) {
  if (reader.data() != expected) {
    std::cerr << "[ FAIL ] BinaryReader.data()" << std::endl;
    return 1;
  }

  std::cout << "[ PASS ] BinaryReader.data()" << std::endl;
  return 0;
}

int initialisation_size_test(BinaryReader &reader, std::size_t expected) {
  if (reader.size() != expected) {
    std::cerr << "[ FAIL ] BinaryReader.size()" << std::endl;
    return 1;
  }

  std::cout << "[ PASS ] BinaryReader.size()" << std::endl;
  return 0;
}

int seek_tell_test(BinaryReader &reader, std::size_t expected) {
  reader.seek(expected);

  std::size_t index = reader.tell();

  if (index != expected) {
    std::cerr << "[ FAIL ] BinaryReader.seek() -> reader.tell(): got " << index;
    std::cerr << ", expected " << expected << "." << std::endl;
    return 1;
  }

  std::cout << "[ PASS ] BinaryReader.seek() -> reader.tell()" << std::endl;
  return 0;
}

int peek_test(BinaryReader &reader, std::size_t expected) {
  bool a = reader.peek();
  bool b = reader.peek();

  if (a != expected) {
    std::cerr << "[ FAIL ] BinaryReader.peek(): got " << a;
    std::cerr << ", expected " << expected << "." << std::endl;
    return 1;
  } else if (b != expected) {
    std::cerr << "[ FAIL ] BinaryReader.peek(): got " << b;
    std::cerr << ", expected " << expected << "." << std::endl;
    return 1;
  }

  std::cout << "[ PASS ] BinaryReader.peek()" << std::endl;
  return 0;
}

int get_test(
  BinaryReader &reader, std::size_t expected_a, std::size_t expected_b
) {
  bool a = reader.get();
  bool b = reader.get();

  if (a != expected_a) {
    std::cerr << "[ FAIL ] BinaryReader.get() (a): got " << a;
    std::cerr << ", expected " << expected_a << "." << std::endl;
    return 1;
  } else if (b != expected_b) {
    std::cerr << "[ FAIL ] BinaryReader.get() (b): got " << b;
    std::cerr << ", expected " << expected_b << "." << std::endl;
    return 1;
  }

  std::cout << "[ PASS ] BinaryReader.get()" << std::endl;
  return 0;
}

int put_test(BinaryReader &reader, std::size_t data, std::size_t expected) {
  reader.put(data);
  bool a = reader.peek();

  if (a != expected) {
    std::cerr << "[ FAIL ] BinaryReader.put(): got " << a;
    std::cerr << ", expected " << expected << "." << std::endl;
    return 1;
  }

  std::cout << "[ PASS ] BinaryReader.put()" << std::endl;
  return 0;
}

int extended_get_test(
  BinaryReader &reader, std::size_t index, std::uint8_t expected
) {
  // get()
  std::uint8_t extracted_8bits = 0;
  reader.seek(index);
  for (std::size_t i = 0; i < 8; ++i) {
    bool b = reader.get();
    extracted_8bits <<= 1;
    extracted_8bits |= std::uint8_t(b);
  }

  if (extracted_8bits != expected) {
    std::cerr << "[ FAIL ] 8 * reader.get(): got " << extracted_8bits;
    std::cerr << ", expected " << expected << "." << std::endl;
    return 1;
  } else {
    std::cout << "[ PASS ] 8 * reader.get()" << std::endl;
  }

  // get_pair()
  std::uint8_t extracted_4pairs = 0;
  reader.seek(index);
  for (std::size_t i = 0; i < 4; ++i) {
    std::uint8_t p = reader.get_pair();

    for (int j = 1; j >= 0; --j) {
      bool a = (p >> j) & 0b1;
      extracted_4pairs <<= 1;
      extracted_4pairs |= std::uint8_t(a);
    }
  }

  if (extracted_4pairs != expected) {
    std::cerr << "[ FAIL ] 4 * reader.get_pair(): got " << extracted_4pairs;
    std::cerr << ", expected " << expected << "." << std::endl;
    return 1;
  } else {
    std::cout << "[ PASS ] 4 * reader.get_pair()" << std::endl;
  }

  // get_nibble()
  std::uint8_t extracted_2nibbles = 0;
  reader.seek(index);
  for (std::size_t i = 0; i < 2; ++i) {
    std::uint8_t p = reader.get_nibble();

    for (int j = 3; j >= 0; --j) {
      bool a = (p >> j) & 0b1;
      extracted_2nibbles <<= 1;
      extracted_2nibbles |= std::uint8_t(a);
    }
  }

  if (extracted_2nibbles != expected) {
    std::cerr << "[ FAIL ] 2 * reader.get_nibble(): got " << extracted_2nibbles;
    std::cerr << ", expected " << expected << "." << std::endl;
    return 1;
  } else {
    std::cout << "[ PASS ] 2 * reader.get_nibble()" << std::endl;
  }

  // get_byte()
  reader.seek(index);
  std::uint8_t extracted_byte = reader.get_byte();
  if (extracted_byte != expected) {
    std::cerr << "[ FAIL ] reader.get_byte(): got " << extracted_byte;
    std::cerr << ", expected " << expected << "." << std::endl;
    return 1;
  } else {
    std::cout << "[ PASS ] reader.get_byte()" << std::endl;
  }

  return 0;
}
