#include <CLI/CLI.hpp>

#include "options.hpp"

OPTIONS parse_command_line(int argc, char *argv[]) {
  OPTIONS options;
  CLI::App app;

  options.err = 0;
  options.called_for_help = false;
  options.output_path = "output";
  options.create_dirs = false;

  app.option_defaults()->always_capture_default();

  app.add_option("-p,--path", options.image_path, "path to image")->required();
  app.add_option("-o,--out", options.output_path, "path to save output");
  app.add_flag("-c,--create_dirs", options.create_dirs, "create directories if needed");

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
