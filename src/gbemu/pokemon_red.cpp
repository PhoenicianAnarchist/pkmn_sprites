
#include <algorithm>
#include <bitset>
#include <exception>
#include <sstream>

#include "../util/io.hpp"
#include "helpers.hpp"

#include "pokemon_red.hpp"

pkmnred::PokemonStats pkmnred::get_stats(
  std::uint8_t pokemon_id, Cartridge &cart
) {
  if (
    (pokemon_id < minimum_index) ||
    (pokemon_id > maximum_index)
  ) {
    std::stringstream ss;
    ss << "pokemon index " << int(pokemon_id) << " out of range.";
    throw std::out_of_range(ss.str());
  }


  if (
    std::find(missingno.begin(), missingno.end(), pokemon_id) != missingno.end()
  ) {
    std::stringstream ss;
    ss << "pokemon index " << int(pokemon_id) << " out of range. (MISSINGNO)";
    throw std::out_of_range(ss.str());
  }

  PokemonStats stats;

  // get pokedex number
  cart.switch_bank(pokedex_order_table_bank);
  stats.dexno = cart.read_from_table(
    pokedex_order_table_offset, pokemon_id - 1,
    pokedex_order_table_width
  )[0];

  // get pokemon stats
  cart.switch_bank(pokemon_stats_table_bank);
  std::vector<std::uint8_t> pokemon_data = cart.read_from_table(
    pokemon_stats_table_offset, stats.dexno - 1,
    pokemon_stats_table_width
  );

  // MEW
  if (stats.dexno == 151) {
    cart.switch_bank(mew_stats_table_bank);
    pokemon_data = cart.read_from_table(
      mew_stats_table_offset, 0,
      mew_stats_table_width
    );
  }

  stats.id = pokemon_id;
  stats.hp = pokemon_data[1];
  stats.attack = pokemon_data[2];
  stats.defence = pokemon_data[3];
  stats.speed = pokemon_data[4];
  stats.special = pokemon_data[5];
  stats.type_1_index = pokemon_data[6];
  stats.type_2_index = pokemon_data[7];
  stats.catch_rate = pokemon_data[8];
  stats.exp_yield = pokemon_data[9];
  stats.sprite_size = pokemon_data[10];
  stats.move_1_index = pokemon_data[15];
  stats.move_2_index = pokemon_data[16];
  stats.move_3_index = pokemon_data[17];
  stats.move_4_index = pokemon_data[18];
  stats.null_byte = pokemon_data[27];

  stats.front_sprite_offset = pokemon_data[12];
  stats.front_sprite_offset <<= 8;
  stats.front_sprite_offset |= pokemon_data[11];

  stats.back_sprite_offset = pokemon_data[14];
  stats.back_sprite_offset <<= 8;
  stats.back_sprite_offset |= pokemon_data[13];

  stats.hm_and_tm_bits = 0;
  for (std::size_t i = 0; i < 8; ++i) {
    stats.hm_and_tm_bits <<= 8;
    stats.hm_and_tm_bits |= pokemon_data[19 + i];
  }

  // read strings
  // name
  std::map<std::uint8_t, std::string> charmap = get_charmap();

  cart.switch_bank(pokemon_names_table_bank);
  stats.name = gbhelp::decode_string(
    cart.read_from_table(
      pokemon_names_table_offset, (stats.id - 1),
      pokemon_names_table_width
    ), charmap, eos_char
  );

  // types
  cart.switch_bank(type_names_pointer_bank);
  std::uint16_t type_1_offset = cart.read_address_from_table(
    type_names_pointer_offset, stats.type_1_index
  );
  stats.type_1 = gbhelp::decode_string(
    cart.read_string(type_1_offset, eos_char), charmap, eos_char
  );

  std::uint16_t type_2_offset = cart.read_address_from_table(
    type_names_pointer_offset, stats.type_2_index
  );
  stats.type_2 = gbhelp::decode_string(
    cart.read_string(type_2_offset, eos_char), charmap, eos_char
  );

  // moves
  load_move_names(cart);
  cart.switch_bank(move_names_pointer_bank);

  std::uint16_t move_1_offset = cart.read_address_from_table(
    move_names_pointer_offset, (stats.move_1_index - 1)
  );
  stats.move_1 = gbhelp::decode_string(
    cart.read_string(move_1_offset, eos_char), charmap, eos_char
  );

  if (stats.move_2_index != 0) {
    std::uint16_t move_2_offset = cart.read_address_from_table(
      move_names_pointer_offset, (stats.move_2_index - 1)
    );
    stats.move_2 = gbhelp::decode_string(
      cart.read_string(move_2_offset, eos_char), charmap, eos_char
    );
  } else {
    stats.move_2 = "----";
  }

  if (stats.move_3_index != 0) {
    std::uint16_t move_3_offset = cart.read_address_from_table(
      move_names_pointer_offset, (stats.move_3_index - 1)
    );
    stats.move_3 = gbhelp::decode_string(
      cart.read_string(move_3_offset, eos_char), charmap, eos_char
    );
  } else {
    stats.move_3 = "----";
  }

  if (stats.move_4_index != 0) {
    std::uint16_t move_4_offset = cart.read_address_from_table(
      move_names_pointer_offset, (stats.move_4_index - 1)
    );
    stats.move_4 = gbhelp::decode_string(
      cart.read_string(move_4_offset, eos_char), charmap, eos_char
    );
  } else {
    stats.move_4 = "----";
  }
  // pokedex entry
  cart.switch_bank(pokedex_data_pointer_bank);
  std::uint16_t pokedex_data_offset = cart.read_address_from_table(
    pokedex_data_pointer_offset, (stats.id - 1)
  );
  stats.pokemon_type_name = gbhelp::decode_string(
    cart.read_string(pokedex_data_offset, eos_char), charmap, eos_char
  );
  pokedex_data_offset += (stats.pokemon_type_name.size() + 1);

  std::vector<std::uint8_t> pokedex_data = cart.read(pokedex_data_offset, 9);

  stats.height_ft = pokedex_data[0];
  stats.height_in = pokedex_data[1];

  std::stringstream height_ss;
  height_ss << int(stats.height_ft) << "' " << int(stats.height_in) << "\"";
  stats.height_string = height_ss.str();

  stats.weight = pokedex_data[3];
  stats.weight <<= 8;
  stats.weight |= pokedex_data[2];

  std::stringstream weight_ss;
  weight_ss << float(stats.weight / 10.0) << " lbs";
  stats.weight_string = weight_ss.str();

  stats.pokedex_entry_offset = pokedex_data[6];
  stats.pokedex_entry_offset <<= 8;
  stats.pokedex_entry_offset |= pokedex_data[5];

  cart.switch_bank(0x2b);
  stats.pokedex_entry = gbhelp::decode_string(
    cart.read_string(stats.pokedex_entry_offset + 1, eos_char), charmap, eos_char
  );

  return stats;
}

void pkmnred::load_move_names(Cartridge &cart) {
  cart.switch_bank(0x2c);
  // 165 total moves
  std::uint16_t address = 0x4000;
  for (std::size_t i = 0; i < 165; ++i) {
    cart.write(move_names_pointer_offset + (i * 2),     address & 0xff);
    cart.write(move_names_pointer_offset + (i * 2) + 1, address >> 8);

    std::vector<std::uint8_t> s = cart.read_string(address, eos_char);
    address += s.size();
  }
}

std::map<std::uint8_t, std::string> pkmnred::get_charmap() {
  std::map<std::uint8_t, std::string> charmap;

  // uppercase
  for (int c = 0x80; c <= 0x9a; ++c) {
    charmap[c] = {static_cast<char>(c - 0x3f)};
  }
  // lowercase
  for (int c = 0xa0; c <= 0xba; ++c) {
    charmap[c] = {static_cast<char>(c - 0x3f)};
  }
  // digits
  for (int c = 0xf6; c <= 0xff; ++c) {
    charmap[c] = {static_cast<char>(c - 0xc6)};
  }

  charmap[0x50] = "⋄";

  charmap[0x00] = "⋄";
  charmap[0x49] = "\n"; // next page token?
  charmap[0x4e] = "\n";
  charmap[0x54] = "Poké";
  charmap[0x5f] = ".";  // ??
  charmap[0x7f] = " ";
  charmap[0xbd] = "'s";
  charmap[0xbe] = "'t";
  charmap[0xe0] = "'";
  charmap[0xe3] = "-";
  charmap[0xe8] = ".";
  charmap[0xef] = "♂";
  charmap[0xf4] = ",";
  charmap[0xf5] = "♀";

  return charmap;
}

std::ostream &pkmnred::operator<<(std::ostream &os, const PokemonStats &stats) {
  os << "Information for " << stats.name << ", ";
  os << " ID (" << int(stats.id) << ")";
  os << " DexNo (" << int(stats.dexno) << ")\n";


  /////////////////////////////////////////////////////////////////////////////
  // Pokedex Entry
  os << "Pokedex Entry\n";
  os << "  The " << stats.pokemon_type_name << " Pokémon\n";
  os << "  Height " << stats.height_string << '\n';
  os << "  Weight " << stats.weight_string << '\n';
  os << '\n';
  os << stats.pokedex_entry << '\n';
  os << '\n';

  /////////////////////////////////////////////////////////////////////////////
  // Basic Information
  os << "Type\n";
  os << "  " << stats.type_1;
  if (stats.type_1 != stats.type_2) {
    os << "/" << stats.type_2;
  }
  os << '\n';

  os << "Starting Moves\n";
  os << "  " << stats.move_1 << '\n';
  os << "  " << stats.move_2 << '\n';
  os << "  " << stats.move_3 << '\n';
  os << "  " << stats.move_4 << '\n';
  os << '\n';

  /////////////////////////////////////////////////////////////////////////////
  // Technical Information
  os << "Base stats\n";
  os << "  HP      " << std::right << std::setw(3) << int(stats.hp) << '\n';
  os << "  Attack  " << std::right << std::setw(3) << int(stats.attack) << '\n';
  os << "  Defence " << std::right << std::setw(3) << int(stats.defence) << '\n';
  os << "  Speed   " << std::right << std::setw(3) << int(stats.speed) << '\n';
  os << "  Special " << std::right << std::setw(3) << int(stats.special) << '\n';

  os << "Catch rate\n  " << int(stats.catch_rate);
  // approximates the worst case catch chance; no status, full health, pokeball
  int ball = 12;      // great ball == 8, otherwise == 12
  int ballmod = 255;  // poke ball == 255, great ball == 200, otherwise 150
  int ailment = 0;    // no status == 0, PSN BRN PAR == 12, FRZ SLP == 25
  float f = (stats.hp * 255 * 4) / float(stats.hp * ball);
  float p0 = (ailment / float(ballmod + 1));
  float p1 = ((stats.catch_rate + 1.0) / (ballmod + 1.0)) * ((f + 1.0) / 256.0);
  float catch_chance = (p0 + p1) * 100.0;
  os << " ~" << catch_chance << "%\n";

  os << "Base Experience Yield\n  " << int(stats.exp_yield) << '\n';
  os << '\n';

  /////////////////////////////////////////////////////////////////////////////
  // Sprite Information
  os << "Sprite Size\n";
  os << "  Height: " << int(stats.sprite_size >> 4) << '\n';
  os << "  Width : " << int(stats.sprite_size & 0x0f) << '\n';

  os << "Sprite data location\n";
  os << "  Front " << gbhelp::hex_str(stats.front_sprite_offset, 2) << '\n';
  os << "  Back  " << gbhelp::hex_str(stats.back_sprite_offset, 2) << '\n';

  // os << "hm_and_tm_bits      " << gbhelp::hex_str(pokemon_stats.hm_and_tm_bits, 8) << '\n';

  return os;
}
