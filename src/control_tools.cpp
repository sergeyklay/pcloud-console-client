// pCloud Console Client
//
// Copyright (c) 2021 Serghei Iakovlev.
// Copyright (c) 2013-2016 Anton Titov.
// Copyright (c) 2013-2016 pCloud Ltd.
//
// This source file is subject to the New BSD License that is bundled with this
// project in the file LICENSE.
//
// If you did not receive a copy of the license and are unable to obtain it
// through the world-wide-web, please send an email to egrep@protonmail.ch so
// we can send you a copy immediately.

#include <iostream>
#include <sys/stat.h>
#include <cstdlib>
#include <unistd.h>
#include <string>

#include "control_tools.hpp"
#include "pclcli.hpp"
#include "overlay_client.h"

namespace control_tools {

static const int STOP = 0;

enum command_ids_ {
  STARTCRYPTO = 20,
  STOPCRYPTO,
  FINALIZE,
  LISTSYNC,
  ADDSYNC,
  STOPSYNC
};

void start_crypto(const char * pass) {
  int ret;
  char* errm;
  if (SendCall(STARTCRYPTO, pass, &ret, &errm))
    std::cout << "Start Crypto failed. return is " << ret<< " and message is "<<errm << std::endl;
  else
    std::cout << "Crypto started. "<< std::endl;
  free(errm);
}


void stop_crypto(){
  int ret;
  char* errm;
  if (SendCall(STOPCRYPTO, "", &ret, &errm))
    std::cout << "Stop Crypto failed. return is " << ret<< " and message is "<<errm << std::endl;
  else
    std::cout << "Crypto Stopped. "<< std::endl;
  free(errm);
}

void finalize(){
   int ret;
  char* errm;
  if (SendCall(FINALIZE, "", &ret, &errm))
    std::cout << "Finalize failed. return is " << ret<< " and message is "<<errm << std::endl;
  else
    std::cout << "Exiting ..."<< std::endl;

  free(errm);
}

void process_commands()
{
  std::cout<< "Supported commands are:" << std::endl << "startcrypto <crypto pass>, stopcrypto, finalize, q, quit" << std::endl;
  std::cout<< "> " ;
  for (std::string line; std::getline(std::cin, line);) {
    if (!line.compare("finalize")) {
      finalize();
      break;}
    else if (!line.compare("stopcrypto"))
       stop_crypto();
    else if (!line.compare(0,11,"startcrypto",0,11) && (line.length() > 12))
      start_crypto(line.c_str() + 12);
    else if (!line.compare("q") || !line.compare("quit"))
      break;

    std::cout<< "> " ;
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
