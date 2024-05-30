#include <CLI/CLI.hpp>

#include "options.hpp"

OPTIONS parse_command_line(int argc, char *argv[]) {
  OPTIONS options;
  CLI::App app;

  options.err = 0;
  options.called_for_help = false;
  options.output_path = "output";
  options.index = 0;
  options.dexno = 0;
  options.create_dirs = false;

  app.option_defaults()->always_capture_default();

  app.add_option("-r,--rom", options.rom_path, "path to rom")->required();
  app.add_option("-o,--out", options.output_path, "path to save output");
  app.add_flag("-c,--create_dirs", options.create_dirs, "create directories if needed");
  app.add_flag("-v,--verbose", options.verbose_level, "increase verbosity");

  auto index = app.add_option_group("subgroup");
  index->add_option("-i,--index", options.index, "pokemon internal index");
  index->add_option("-d,--dexno", options.dexno, "pokemon pokedex number");
  index->add_flag("-a,--all", options.extract_all, "extract all sprites");
  index->require_option(1);

  try {
    app.parse(argc, argv);
  } catch (const CLI::CallForHelp &e) {
    options.called_for_help = true;
    options.err = app.exit(e);
  } catch (const CLI::ParseError &e) {
    options.err = app.exit(e);
  }

  return options;
}
