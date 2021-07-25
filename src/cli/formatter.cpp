// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#include "formatter.hpp"

#include <string>

std::string pcloud::cli::Formatter::make_description(
    const CLI::App *app) const {
  std::string out;

  const auto BANNER = R"BANNER(
           ________                __
    ____  / ____/ /___  __  ______/ /
   / __ \/ /   / / __ \/ / / / __  /
  / /_/ / /___/ / /_/ / /_/ / /_/ /
 / .___/\____/_/\____/\__,_/\__,_/
/_/)BANNER";

  std::string banner(BANNER);
  out += banner.replace(0, 1, "") + "\n\n";

  auto desc = app->get_description();
  out += desc + "\n\n";

  return out;
}

std::string pcloud::cli::Formatter::make_usage(const CLI::App *app,
                                               std::string name) const {
  auto out = get_label("Usage") + ":\n";
  out += "  " + app->get_name();

  auto groups = app->get_groups();

  // Print an OPTIONS badge if any options exist
  auto non_positionals = app->get_options(
      [](const CLI::Option *opt) { return opt->nonpositional(); });
  if (!non_positionals.empty()) {
    out += " [" + get_label("OPTIONS") + "]";
  }

  // Print an ARGUMENTS badge if any arguments exist
  // or we're show help for the main program
  auto positionals = app->get_options(
      [](const CLI::Option *opt) { return opt->get_positional(); });
  if (!app->get_parent() || !positionals.empty()) {
    out += " [--] [" + get_label("ARGUMENTS") + "]";
  }

  return out += "\n";
}

std::string pcloud::cli::Formatter::make_option_name(const CLI::Option *opt,
                                                     bool is_positional) const {
  std::string name;
  if (is_positional)
    name = opt->get_name(true, false);
  else
    name = opt->get_name(false, true);

  std::string new_name;
  if (name[0] == '-' && name[1] == '-') {
    new_name = "    " + name;
  } else {
    for (char i : name) {
      if (i != ',')
        new_name += i;
      else {
        new_name += i;
        new_name += " ";
      }
    }
  }

  return new_name;
}
