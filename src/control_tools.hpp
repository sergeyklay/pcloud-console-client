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

#ifndef PSYNC_CONTROL_TOOLS_H
#define PSYNC_CONTROL_TOOLS_H

namespace control_tools {
  void start_crypto(const char * pass);
  void stop_crypto();
  void finalize();
  void daemonize(bool do_commands);
  void process_commands();
}

#endif  // PSYNC_CONTROL_TOOLS_H
