// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
// (c) 2013-2016 Anton Titov <anton@pcloud.com>
// (c) 2013-2016 pCloud Ltd
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#include <iostream>
#include <sys/stat.h>
#include <cstdlib>
#include <unistd.h>
#include <string>

#include "overlay_client.h"

#include "pcloudcc/version.hpp"
#include "control_tools.hpp"
#include "pclcli.hpp"

namespace control_tools {

void start_crypto(const char *pass) {
  int ret;
  overlay_command_t cmd = STARTCRYPTO;
  char *errm = nullptr;

  if (send_call(cmd, pass, &ret, &errm) == -1)
    std::cout << "Failed to start crypto: " << errm << std::endl;
  else
    std::cout << "Crypto started" << std::endl;

  if (errm)
    free(errm);
}

void stop_crypto() {
  int ret;
  overlay_command_t cmd = STOPCRYPTO;
  char *errm = nullptr;

  if (send_call(cmd, "", &ret, &errm) == -1)
    std::cout << "Failed to stop crypto: " << errm << std::endl;
  else
    std::cout << "Crypto Stopped" << std::endl;

  if (errm)
    free(errm);
}

void finalize() {
  int ret;
  overlay_command_t cmd = FINALIZE;
  char *errm = nullptr;

  if (send_call(cmd, "", &ret, &errm) == -1)
    std::cout << "Failed to finalize crypto: " << errm << std::endl;
  else
    std::cout << "Exiting..." << std::endl;

  if (errm)
    free(errm);
}

void static print_menu() {
  std::cout << std::endl << "Help:" << std::endl << std::endl;

  std::cout << "  Crypto"<< std::endl;
  std::cout << "   startcrypto <crypto pass>   "
            << "Start a crypto session using given password"
            << std::endl;
  std::cout << "   stopcrypto                  "
            << "Stop a crypto session"
            << std::endl
            << std::endl;

  std::cout << "  Misc"<< std::endl;
  std::cout << "   m, menu                     "
            << "Print this menu"
            << std::endl
            << std::endl;

  std::cout << "  Exit"<< std::endl;
  std::cout << "   finalize                    "
            << "Stop the running daemon"
            << std::endl;
  std::cout << "   q, quit                     "
            << "Quit the current client (daemon stays alive)"
            << std::endl
            << std::endl;
}

void process_commands() {
  std::cout << "Welcome to" << PCLOUD_VERSION_FULL << std::endl << std::endl;
  std::cout<< "Command (m for help): ";

  for (std::string line; std::getline(std::cin, line) ; ) {
    if (!line.compare(0, 11, "startcrypto", 0, 11) && (line.length() > 12)) {
      start_crypto(line.c_str() + 12);
    } else if (line == "stopcrypto") {
      stop_crypto();
    } else if (line == "menu" || line == "m") {
      print_menu();
    } else if (line == "quit" || line == "q") {
      break;
    } else if (line ==  "finalize") {
      finalize();
      break;
    }

    std::cout<< "Command (m for help): " ;
  }
}

void daemonize(bool do_commands) {
  pid_t pid, sid;

  pid = fork();
  if (pid < 0)
    exit(EXIT_FAILURE);
  if (pid > 0) {
    std::cout << "Daemon process created. Process id is: " << pid << std::endl;
    if (do_commands) {
      process_commands();
    }
    else
      std::cout  << "sudo kill -9 "<<pid<< std::endl<<" To stop it."<< std::endl;
    exit(EXIT_SUCCESS);
  }
  umask(0);
  /* Open any logs here */
  sid = setsid();
  if (sid < 0)
    exit(EXIT_FAILURE);

  if ((chdir("/")) < 0)
    exit(EXIT_FAILURE);
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  if (console_client::clibrary::pclcli::get_lib().init())
     exit(EXIT_FAILURE);
  while (true) {
    sleep(10);
  }
}
}
