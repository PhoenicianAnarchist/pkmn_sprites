#ifndef __TABULATE_HPP__
#define __TABULATE_HPP__

#include <string>
#include <vector>

#include <cstdint>

struct column_config {
  std::size_t w = 0;
  std::size_t hex_w = 0;
  bool is_left_align = false;
  bool is_hex = false;
};

class Tabulate {
public:
  Tabulate() = default;

  void set_column_config(std::size_t col, const column_config& cfg);

  void add_header(const std::vector<std::string>& headers);
  void add_hr();
  void add_row(const std::vector<std::string>& row);

  friend std::ostream& operator<<(std::ostream& os, const Tabulate& t);
  static std::string hex_str(std::uint64_t i, std::size_t w);
  static std::string int_str(long long int i);
  static std::string bool_str(bool b);

private:
  char vc = '|';
  char hc = '-';
  char xc = '+';

  std::vector<column_config> configs;
  std::vector<std::string> table_strings;
};


#endif // __TABULATE_HPP__
