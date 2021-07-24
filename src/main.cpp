// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
// (c) 2013-2016 Anton Titov <anton@pcloud.com>
// (c) 2013-2016 pCloud Ltd
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#include <iostream>
#include <vector>
#include <CLI/CLI.hpp>

#include "unistd.h"
#include "pcloudcc/psync/version.h"

#include "pcloudcc/version.hpp"
#include "pclcli.hpp"
#include "control_tools.hpp"

static inline std::vector<std::string> prepare_args(int argc, char** argv) {
  std::vector<std::string> args;
  args.reserve(static_cast<size_t>(argc - 1));
  for (int i = argc - 1; i > 0; i--) {
    args.emplace_back(argv[i]);
  }

  return args;
}

int main(int argc, char** argv) {
  auto args = prepare_args(argc, argv);
  auto out =
      std::string(PCLOUD_PACKAGE_NAME) + " " + std::string(PCLOUD_VERSION);

  CLI::App app{out};
  app.description(out);
  app.name("pcloudcc");

  app.get_formatter()->column_width(26);
  app.get_formatter()->label("OPTIONS", "options");

  // Global flag & options

  bool commands_only = false;
  app.add_flag("--commands-only,-k", commands_only,
               "Daemon already started pass only commands");

  bool daemonize = false;
  app.add_flag("--daemonize,-d", daemonize,
               "Daemonize the process");

  std::string username;
  app.add_option("--username,-u", username, "pCloud account name");

  bool passwordsw = false;
  app.add_flag("--password,-p", passwordsw, "Ask pCloud account password");

  bool crypto = false;
  app.add_flag("--crypto,-c", crypto, "Ask crypto password");

  bool passascrypto = false;
  app.add_flag("--passascrypto,-y", passascrypto,
               "Use user password as crypto password also");

  std::string mountpoint;
  app.add_option("--mountpoint,-m", username,
                 "Mount point where drive to be mounted");

  bool newuser = false;
  app.add_flag("--newuser,-n", newuser,
               "Use if this is a new user to be registered");

  bool commands = false;
  app.add_flag("--commands,-o", commands,
               "Parent stays alive and processes commands");

  bool savepassword = false;
  app.add_flag("--savepassword,-s", commands,
               "Save password in database");

  CLI::Option *version = app.add_flag(
      "-V, --version",
      "Print client version information and quit");

  // Remove help flag because it shortcuts all processing
  app.set_help_flag();

  // Add custom flag that activates help
  auto help = app.add_flag("-h, --help", "Print this help message and quit");

  try {
    app.parse(args);

    if (commands_only) {
      control_tools::process_commands();
      return EXIT_SUCCESS;
    }

    if (*help) {
      throw CLI::CallForHelp();
    }

    if (*version) {
      std::cout << PCLOUD_VERSION_FULL << " ("
                << PSYNC_VERSION_FULL
                << ") " << std::endl;

      std::cout << "Copyright " << PCLOUD_COPYRIGHT << "." << std::endl;

      std::cout << "This is free software; see the source for copying "
                   "conditions.  There is NO"
                << std::endl;

      std::cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A "
                   "PARTICULAR PURPOSE."
                << std::endl
                << std::endl;
      return 0;
    }

    if (username.empty()) {
      std::cout << "Username option is required" << std::endl;
      std::cout << "Daemonize" << daemonize << std::endl;
      return 1;
    }
    console_client::clibrary::pclcli::get_lib().set_username(username);

    if (passwordsw) {
      console_client::clibrary::pclcli::get_lib().get_pass_from_console();
    }

    std::string password;
    if (crypto) {
      console_client::clibrary::pclcli::get_lib().setup_crypto_ = true;
      if (passascrypto)
        console_client::clibrary::pclcli::get_lib().set_crypto_pass(password);
      else {
        std::cout << "Crypto password: ";
        console_client::clibrary::pclcli::get_lib().get_cryptopass_from_console();
      }
    } else
       console_client::clibrary::pclcli::get_lib().setup_crypto_ = false;

    if (!mountpoint.empty())
        console_client::clibrary::pclcli::get_lib().set_mount(mountpoint);

    console_client::clibrary::pclcli::get_lib().newuser_ = newuser;
    console_client::clibrary::pclcli::get_lib().set_savepass(savepassword);
    console_client::clibrary::pclcli::get_lib().set_daemon(daemonize);

    if (daemonize)
      control_tools::daemonize(commands);
    else {
      if (commands)
        std::cout << "The \"commands\" option was ignored because the "
                  << "client is not running in daemon mode"
                  << std::endl;
      if (!console_client::clibrary::pclcli::get_lib().init())
        sleep(360000);
    }
  } catch (const CLI::Error& e) {
    auto ret = app.exit(e);
    return ret;
  } catch(std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch(...) {
    std::cerr << "Unknown error. "
              << "Please open a bug report: "
              << PCLOUD_PACKAGE_URL
              << std::endl;
    return EXIT_FAILURE;
  }


  return EXIT_SUCCESS;
}
