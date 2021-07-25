// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#include <memory>
#include <string>
#include <vector>

#include "cli/app.hpp"

static inline std::vector<std::string> prepare_args(int argc, char **argv) {
  std::vector<std::string> args;
  args.reserve(static_cast<size_t>(argc - 1));
  for (int i = argc - 1; i > 0; i--) {
    if (std::string(argv[i]) == "-dumpversion") {
      args.emplace_back("--dumpversion");
    } else {
      args.emplace_back(argv[i]);
    }
  }

  return args;
}

/// \brief pcloudcc entrypoint.
///
/// \return 0 if everything went fine, or a positive error code
int main(int argc, char **argv) {
  auto args = prepare_args(argc, argv);
  auto app = std::make_unique<pcloud::cli::App>(args);

  return app->run();
}
