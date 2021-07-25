// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
// (c) 2013-2016 Anton Titov <anton@pcloud.com>
// (c) 2013-2016 pCloud Ltd
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#ifndef PCLOUD_CLI_CT_HPP_
#define PCLOUD_CLI_CT_HPP_

#include "pcloudcc/psync/compiler.h"

namespace pcloud {
namespace cli {
void start_crypto(const char *pass);
void stop_crypto();
PSYNC_NO_RETURN void daemonize(bool do_commands);
void process_commands();
}  // namespace cli
}  // namespace pcloud

#endif  // PCLOUD_CLI_CT_HPP_
