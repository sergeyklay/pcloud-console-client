// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
// (c) 2013-2016 Anton Titov <anton@pcloud.com>
// (c) 2013-2016 pCloud Ltd
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#include "ct.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include <pcloudcc/version.hpp>

#include "bridge.hpp"
#include "overlay_client.h"
#include "pcloudcc/psync/compiler.h"
#include "pcloudcrypto.h"

void pcloud::cli::start_crypto(const char *pass) {
  int ret;
  char *errm = nullptr;
  int status = send_call(STARTCRYPTO, pass, &ret, &errm);

  /* -1 can only be returned from overlay_client */
  if (status == -1) {
    std::cout << "Failed to start crypto: " << errm << std::endl;
  } else if (status == PSYNC_CRYPTO_START_SUCCESS) {
    std::cout << "Crypto session started" << std::endl;
  } else if (status == PSYNC_CRYPTO_START_ALREADY_STARTED) {
    std::cout << "Crypto session has already started" << std::endl;
  } else {
    std::cout << "Failed to start crypto: "
              << psync_cloud_crypto_strstart(status) << std::endl;
  }

  if (errm) free(errm);
}

void pcloud::cli::stop_crypto() {
  int ret;
  char *errm = nullptr;
  int status = send_call(STOPCRYPTO, "", &ret, &errm);

  /* -1 can only be returned from overlay_client */
  if (status == -1) {
    std::cout << "Failed to stop crypto: " << errm << std::endl;
  } else if (status == PSYNC_CRYPTO_STOP_NOT_STARTED) {
    std::cout << "Crypto session is not started" << std::endl;
  } else if (status == PSYNC_CRYPTO_STOP_SUCCESS) {
    std::cout << "Crypto session was stop" << std::endl;
  } else {
    std::cout << "Failed to stop crypto: unknown status" << errm << std::endl;
  }

  if (errm) free(errm);
}

void static print_menu() {
  std::cout << std::endl << "Help:" << std::endl << std::endl;

  std::cout << "  Crypto" << std::endl;
  std::cout << "   startcrypto <crypto pass>   "
            << "Start a crypto session using given password" << std::endl;
  std::cout << "   stopcrypto                  "
            << "Stop a crypto session" << std::endl
            << std::endl;

  std::cout << "  Misc" << std::endl;
  std::cout << "   m, menu                     "
            << "Print this menu" << std::endl
            << std::endl;

  std::cout << "  Exit" << std::endl;
  std::cout << "   q, quit                     "
            << "Quit the current client (daemon stays alive)" << std::endl
            << std::endl;
}

void pcloud::cli::process_commands() {
  std::cout << "Welcome to" << PCLOUD_VERSION_FULL << std::endl << std::endl;
  std::cout << "Command (m for help): ";

  for (std::string line; std::getline(std::cin, line);) {
    if (!line.compare(0, 11, "startcrypto", 0, 11) && (line.length() > 12)) {
      start_crypto(line.c_str() + 12);
    } else if (line == "stopcrypto") {
      stop_crypto();
    } else if (line == "menu" || line == "m") {
      print_menu();
    } else if (line == "quit" || line == "q") {
      break;
    }

    std::cout << "Command (m for help): ";
  }
}

PSYNC_NO_RETURN void pcloud::cli::daemonize(bool do_commands) {
  pid_t pid, sid;

  pid = fork();
  if (pid < 0) exit(EXIT_FAILURE);

  if (pid > 0) {
    std::cout << "Daemon process created. Process id is: " << pid << std::endl;
    if (do_commands) {
      process_commands();
    } else
      std::cout << "Use \"kill " << pid << "\""
                << " to stop it." << std::endl;
    exit(EXIT_SUCCESS);
  }

  umask(0);

  /* Open any logs here */
  sid = setsid();
  if (sid < 0) exit(EXIT_FAILURE);

  if ((chdir("/")) < 0) exit(EXIT_FAILURE);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  if (Bridge::get_lib().init()) exit(EXIT_FAILURE);

  while (true) {
    sleep(10);
  }
}
