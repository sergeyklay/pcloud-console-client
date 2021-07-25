// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

/// \file formatter.hpp
/// \brief Provides custom CLI::Formatter implementation.

#ifndef PCLOUD_CLI_FORMATTER_HPP_
#define PCLOUD_CLI_FORMATTER_HPP_

#include <string>

#include <CLI/CLI.hpp>
#include <CLI/Formatter.hpp>

namespace pcloud {
namespace cli {
/// \brief This is the default Formatter for pcloudcc CLI.
///
/// It pretty prints help output, and is broken into quite a few overridable
/// methods, to be highly customizable with minimal effort.
class Formatter : public CLI::Formatter {
 public:
  /// \brief Displays the description line.
  ///
  /// \param app The pointer to a CLI::App instance
  /// \return Program/command description string.
  std::string make_description(const CLI::App *app) const override;

  /// \brief Gets the usage line.
  ///
  /// \param app The pointer to a CLI::App instance
  /// \param name The application name (e.g. "pcloudcc"). Currently not unused
  /// \return A program (command) usage string.
  std::string make_usage(const CLI::App *app, std::string name) const override;

  /// \brief Format the name part of an option.
  ///
  /// \param opt The pointer to a CLI::Option instance
  /// \param is_positional Is this option positional?
  /// \return Option name as a string.
  std::string make_option_name(const CLI::Option *opt,
                               bool is_positional) const override;
};
}  // namespace cli
}  // namespace pcloud

#endif  // PCLOUD_CLI_FORMATTER_HPP_
