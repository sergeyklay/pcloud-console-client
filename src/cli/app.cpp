// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#include "app.hpp"

#include <unistd.h>

#include <string>

#include <CLI/CLI.hpp>

#include "bridge.hpp"
#include "ct.hpp"
#include "formatter.hpp"
#include "pcloudcc/psync/version.h"
#include "pcloudcc/version.hpp"

pcloud::cli::App::App(std::vector<std::string> args)
    : args_(std::move(args)),
      app_(std::make_shared<CLI::App>()),
      help_(nullptr) {
  auto const description =
      std::string(PCLOUD_PACKAGE_NAME) + " " + std::string(PCLOUD_VERSION);

  app_->description(description);
  app_->name("pcloudcc");

  // Create an instance of the CLI Formatter
  app_->formatter(std::make_shared<pcloud::cli::Formatter>());

  app_->get_formatter()->column_width(26);
  app_->get_formatter()->label("OPTIONS", "options");
  app_->get_formatter()->label("ARGUMENTS", "arguments");
  app_->get_formatter()->label("TEXT", "arg");

  // Setup CLI options
  setup_global_options();
  setup_options();
}

// Process commands
int pcloud::cli::App::run() {
  try {
    app_->parse(args_);

    if (*help_) {
      throw CLI::CallForHelp();
    }

    if (commands_only_) {
      process_commands();
      return EXIT_SUCCESS;
    }

    if (username_.empty()) {
      std::cout << "Username option is required" << std::endl;
      return EXIT_FAILURE;
    }
    Bridge::get_lib().set_username(username_);

    if (password_) {
      Bridge::get_lib().get_pass_from_console();
    }

    std::string password;
    if (crypto_) {
      Bridge::get_lib().setup_crypto_ = true;
      if (passascrypto_)
        Bridge::get_lib().set_crypto_pass(password);
      else {
        std::cout << "Crypto password: ";
        Bridge::get_lib().get_cryptopass_from_console();
      }
    } else
      Bridge::get_lib().setup_crypto_ = false;

    if (!mountpoint_.empty()) Bridge::get_lib().set_mount(mountpoint_);

    Bridge::get_lib().newuser_ = newuser_;
    Bridge::get_lib().set_savepass(savepassword_);
    Bridge::get_lib().set_daemon(daemonize_);

    if (daemonize_)
      daemonize(commands_);
    else {
      if (commands_)
        std::cout << "The \"commands\" option was ignored because the "
                  << "client is not running in daemon mode" << std::endl;
      if (!Bridge::get_lib().init()) sleep(360000);
    }
  } catch (const CLI::Error &e) {
    return app_->exit(e);
  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unknown error. "
              << "Please open a bug report: " << PCLOUD_PACKAGE_URL
              << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void pcloud::cli::App::setup_global_options() {
  app_->add_flag("--dumpversion", print_version,
                 "Print the version of the client and don't do anything else\n"
                 "(also works with a single hyphen)");

  app_->add_flag("--vernum", print_version_id,
                 "Print the version of the client as integer and quit");

  app_->add_flag("-V, --version", print_full_version,
                 "Print client version information and quit");

  // Remove help flag because it shortcuts all processing
  app_->set_help_flag();

  // Add custom flag that activates help
  help_ = app_->add_flag("-h, --help", "Print this help message and quit");
}

void pcloud::cli::App::setup_options() {
  app_->add_flag("--commands-only,-k", commands_only_,
                 "Daemon already started pass only commands");

  app_->add_flag("--daemonize,-d", daemonize_, "Daemonize the process");

  app_->add_option("--username,-u", username_, "pCloud account name");

  app_->add_flag("--password,-p", password_, "Ask pCloud account password");

  app_->add_flag("--crypto,-c", crypto_, "Ask crypto password");

  app_->add_flag("--passascrypto,-y", passascrypto_,
                 "Use user password as crypto password also");

  app_->add_option("--mountpoint,-m", mountpoint_,
                   "Mount point where drive to be mounted");

  app_->add_flag("--newuser,-n", newuser_,
                 "Use if this is a new user to be registered");

  app_->add_flag("--commands,-o", commands_,
                 "Parent stays alive and processes commands");

  app_->add_flag("--savepassword,-s", savepassword_,
                 "Save password in database");
}

void pcloud::cli::App::print_version(std::size_t /* count */) {
  std::cout << PCLOUD_VERSION << std::endl;
  exit(EXIT_SUCCESS);
}

void pcloud::cli::App::print_full_version(std::size_t /* count */) {
  std::cout << PCLOUD_VERSION_FULL << " (" << PSYNC_VERSION_FULL << ") "
            << std::endl;

  std::cout << "Copyright " << PCLOUD_COPYRIGHT << "." << std::endl;

  std::cout << "This is free software; see the source for copying "
               "conditions.  There is NO"
            << std::endl;

  std::cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A "
               "PARTICULAR PURPOSE."
            << std::endl
            << std::endl;
  exit(EXIT_SUCCESS);
}

void pcloud::cli::App::print_version_id(std::size_t /* count */) {
  std::cout << PCLOUD_VERSION_ID << std::endl;
  exit(EXIT_SUCCESS);
}
