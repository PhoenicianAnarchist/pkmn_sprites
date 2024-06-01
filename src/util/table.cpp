#include <iomanip>
#include <sstream>

#include "table.hpp"

void Tabulate::set_column_config(std::size_t col, const column_config& cfg) {
  if (configs.size() <= col) {
    configs.resize(col + 1);
  }

  configs[col] = cfg;
}

void Tabulate::add_header(const std::vector<std::string>& headers) {
  std::stringstream ss;
  for (std::size_t i = 0; i < headers.size(); ++i) {
    auto cfg = configs[i];
    ss << std::setw(cfg.w) << std::left << headers[i];

    if (i != headers.size() - 1) {
      ss << " " << vc << " ";
    }
  }

  table_strings.push_back(ss.str());
}

void Tabulate::add_hr() {
  std::stringstream ss;
  for (std::size_t i = 0; i < configs.size(); ++i) {
    auto cfg = configs[i];

    if (i != 0) {
      ss << hc;
    }

    for (std::size_t j = 0; j < cfg.w; ++j) {
      ss << hc;
    }

    if (i != configs.size() - 1) {
      ss << hc << xc;
    }
  }

  table_strings.push_back(ss.str());
}

void Tabulate::add_row(const std::vector<std::string>& row) {
  std::stringstream ss;

  for (std::size_t i = 0; i < row.size(); ++i) {
    auto cfg = configs[i];
    ss << std::setw(cfg.w);

    if (cfg.is_left_align) {
      ss << std::left;
    } else {
      ss << std::right;
    }

    ss << row[i];

    if (i != row.size() - 1) {
      ss << " " << vc << " ";
    }
  }

  table_strings.push_back(ss.str());
}

std::ostream& operator<<(std::ostream& os, const Tabulate& t) {
  for (auto& s : t.table_strings) {
    os << s << '\n';
  }

  return os;
}

std::string Tabulate::hex_str(std::uint64_t i, std::size_t w) {
  std::stringstream ss;
  ss << "0x" << std::setw(w * 2) << std::setfill('0') << std::hex << i;
  return ss.str();
}

std::string Tabulate::int_str(long long int i) {
  std::stringstream ss;
  ss << i;
  return ss.str();
}

std::string Tabulate::bool_str(bool b) {
  std::stringstream ss;
  ss << std::boolalpha << b;
  return ss.str();
}
