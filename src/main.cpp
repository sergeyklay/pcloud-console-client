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

class PcloudFormatter : public CLI::Formatter {
 public:
  std::string make_usage(const CLI::App *app,
                         const std::string /* name */) const override {

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

  std::string make_description(const CLI::App *app) const override {
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

  std::string make_option_name(const CLI::Option *opt, bool is_positional) const override {
    std::string name;
    if(is_positional)
      name = opt->get_name(true, false);
    else
      name = opt->get_name(false, true);

    std::string new_name;
    if (name[0] == '-' && name[1] == '-') {
      new_name = "    " + name;
    } else {
      for(char i : name) {
        if(i != ',')
          new_name += i;
        else {
          new_name += i;
          new_name += " ";
        }
      }
    }

    return new_name;
  }
};

/// \brief pcloudcc entrypoint.
///
/// \return #EXIT_SUCCESS on success, #EXIT_FAILURE otherwise.
int main(int argc, char** argv) {
  auto args = prepare_args(argc, argv);
  auto const description =
      std::string(PCLOUD_PACKAGE_NAME) + " " + std::string(PCLOUD_VERSION);

  CLI::App app{description, "pcloudcc"};

  app.formatter(std::make_shared<PcloudFormatter>());
  app.get_formatter()->column_width(26);
  app.get_formatter()->label("OPTIONS", "options");
  app.get_formatter()->label("ARGUMENTS", "arguments");
  app.get_formatter()->label("TEXT", "arg");

  // Global flag & options

  bool commands_only = false;
  app.add_flag("--commands-only,-k", commands_only,
               "Daemon already started pass only commands");

  bool daemonize = false;
  app.add_flag("--daemonize,-d", daemonize,
               "Daemonize the process");

  std::string username;
  app.add_option("--username,-u", username,
                 "pCloud account name");

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

  auto version = [](int /* count */){
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
    exit(EXIT_SUCCESS);
  };
  app.add_flag_function(
      "--version,-V",
      version,
      "Print client version information and quit");

  auto vernum = [](int /* count */){
    std::cout << PCLOUD_VERSION_ID << std::endl;
    exit(EXIT_SUCCESS);
  };
  app.add_flag_function(
      "--vernum",
      vernum,
      "Print the version of the client as integer and quit");

  auto dumpversion = [](int /* count */){
    std::cout << PCLOUD_VERSION << std::endl;
    exit(EXIT_SUCCESS);
  };
  app.add_flag_function(
      "--dumpversion",
      dumpversion,
      "Print the version of the client and don't do anything else");

  // Remove help flag because it shortcuts all processing
  app.set_help_flag();

  // Add custom flag that activates help
  auto help = app.add_flag("-h, --help", "Print this help message and quit");

  // Process commands
  try {
    app.parse(args);

    if (*help) {
      throw CLI::CallForHelp();
    }

    if (commands_only) {
      control_tools::process_commands();
      return EXIT_SUCCESS;
    }

    if (username.empty()) {
      std::cout << "Username option is required" << std::endl;
      return EXIT_FAILURE;
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
