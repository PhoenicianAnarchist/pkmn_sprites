#include <CLI/CLI.hpp>

#include "options.hpp"

OPTIONS parse_command_line(int argc, char *argv[]) {
  OPTIONS options;
  CLI::App app;

  options.err = 0;
  options.called_for_help = false;

  app.add_option("-p,--path", options.image_path, "path to image")->required();

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
