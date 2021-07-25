// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

/// \file app.hpp
/// \brief Represent console application class.

#ifndef PCLOUD_CLI_APP_HPP_
#define PCLOUD_CLI_APP_HPP_

#include <string>
#include <vector>

#include <CLI/CLI.hpp>

namespace pcloud {
namespace cli {
/// \brief The console application class.
///
/// \details It is the main entry point of a console application. This class
/// is optimized for a standard CLI environment.
class App {
 public:
  /// \brief Construct application object with a given configuration, CLI args
  /// and project base path.
  ///
  /// \param args CLI args / options
  explicit App(std::vector<std::string> args);

  App(const App&) = delete;

  /// \brief Runs the current application.
  ///
  /// \return 0 if everything went fine, or a positive error code
  int run();

  App& operator=(const App&) = delete;

 private:
  /// Prints program version information.
  static void print_version(std::size_t count);

  static void print_full_version(std::size_t count);
  static void print_version_id(std::size_t count);

  /// \brief Setups global CLI options.
  void setup_global_options();

  /// \brief Setups CLI options.
  void setup_options();

  /// A vector of CLI arguments
  std::vector<std::string> args_;

  /// An instance of the CLI::App
  CLI::App_p app_;

  /// An instance of the CLI::Option used to indicate that help was requested
  /// (may be nullptr)
  CLI::Option* help_;

  bool commands_only_ = false;
  bool daemonize_ = false;
  bool password_ = false;
  bool crypto_ = false;
  bool passascrypto_ = false;
  bool newuser_ = false;
  bool commands_ = false;
  bool savepassword_ = false;
  std::string username_;
  std::string mountpoint_;
};
}  // namespace cli
}  // namespace pcloud

#endif  // PCLOUD_CLI_APP_HPP_
