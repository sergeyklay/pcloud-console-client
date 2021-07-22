/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <netinet/in.h>

#include "config.h"
#include "pcloudcc/psync/compat.h"
#include "pcloudcc/psync/stringcompat.h"
#include "pcloudcc/psync/sockets.h"
#include "overlay_client.h"
#include "logger.h"

/* TODO: Duplicate. See: poverlay.h. Move to sockets.h? */
typedef struct message_ {
  uint32_t type;
  uint64_t length;
  char value[];
} message;

static int logger_initialized = 0;

/*! \brief Reads \a nbyte bytes from \a socket.
 *
 *  Attempts to read \a nbyte bytes of data from the object referenced by the
 *  descriptor \a socket into the buffer pointed to by \a buf.
 */
static void read_x_bytes(int socket, void *buf, size_t nbyte) {
  size_t br = 0;
  size_t result;
  while (br < nbyte) {
    result = read(socket, buf + br, nbyte - br);
    if (result < 1) {
      return;
    }
    br += result;
  }
}

int query_state(overlay_file_state_t *state, char *path) {
  int rep = 0;
  char *errm = NULL;

  if (!logger_initialized) {
    setup_logging();
    logger_initialized = 1;
  }

  if (!send_call(4, path, &rep, &errm)) {
    log_debug("query_state: response code: %d, path: %s", rep, path);
    if (errm)
      log_error("query_state: %s", errm);

    switch (rep) {
      case 10:
        *state = FILE_STATE_IN_SYNC;
        break;
      case 11:
        *state = FILE_STATE_NO_SYNC;
        break;
      case 12:
        *state = FILE_STATE_IN_PROGRESS;
        break;
      default:
        *state = FILE_STATE_INVALID;
    }
  } else
    log_error("query_state: response code: %d, path: %s", rep, path);

  if (errm)
    free(errm);

  return 0;
}

static const char* cmd2str(const overlay_command_t cmd) {
  switch (cmd) {
    case STARTCRYPTO: return "startcrypto";
    case STOPCRYPTO: return "stopcrypto";
    case FINALIZE: return "finalize";
    case LISTSYNC: return "listsync";
    case ADDSYNC: return "addsync";
    case STOPSYNC: return "stopsync";
    default: return "unknown";
  }
}

int send_call(overlay_command_t cmd, const char *path, int *ret, char **out) {
#ifdef P_OS_MACOSX
  struct sockaddr_in addr;
#else
  struct sockaddr_un addr;
#endif

  int fd;
  size_t path_size = strlen(path);
  size_t mess_size = sizeof(message) + path_size + 1;
  size_t rc, bw = 0;
  char *curbuf = NULL;
  char *buf = NULL;
  char sendbuf[mess_size];
  message *rep = NULL;
  uint32_t bufflen = 0;
  uint64_t msg_type;

  if (!logger_initialized) {
    setup_logging();
    logger_initialized = 1;
  }

  log_info("%s: start processing command", cmd2str(cmd));

#ifdef P_OS_MACOSX
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == - 1) {
    log_error("%s: failed to create INET socket", cmd2str(cmd));
    *out = (void *)strdup("Unable to create INET socket");
    *ret = -1;
    return -1;
  }

  memset (&addr, 0, sizeof (addr));
  addr. sin_family = AF_INET;
  addr. sin_addr . s_addr = htonl(INADDR_LOOPBACK);
  addr. sin_port = htons(POVERLAY_PORT);
  if (connect (fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == - 1) {
    log_error("%s: failed to connect to socket %s",
              cmd2str(cmd), strerror(errno));
    *out = (void *)strdup("Unable to connect to INET socket");
    *ret = -2;
    return -1;
  }
#else
  const char *socket_path = psync_unix_socket_path();
  if (!socket_path) {
    log_error("%s: socket path is empty", cmd2str(cmd));
    *out = (void *)strdup("Unable to determine a path for UNIX socket");
    *ret = - 1;
    return -1;
  }

  /* 1. Open a socket */
  if ((fd = socket(AF_UNIX, SOCK_STREAM, POVERLAY_PROTOCOL)) == -1) {
    log_error("%s: failed to create socket %s", cmd2str(cmd), socket_path);
    *out = (void *)strdup("Unable to create UNIX socket");
    *ret = -3;
    return -1;
  }

  /* 2. Create an address */
  bzero((char *)&addr, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strlcpy(addr.sun_path, socket_path, sizeof(addr.sun_path));

  /* 3. Initiate a connection on a socket */
  if (connect(fd, (struct sockaddr*)&addr, SUN_LEN(&addr)) == - 1) {
    log_error("%s: failed to connect to socket %s",
              cmd2str(cmd), strerror(errno));
    *out = (void *)strdup("Unable to connect to UNIX socket");
    *ret = -4;
    return -1;
  }
#endif

  message *mes = (message *)sendbuf;
  memset (mes, 0, mess_size);
  mes->type = cmd;
  strlcpy(mes->value, path, path_size + 1);
  mes->length = mess_size;
  curbuf = (char *)mes;

  while ((rc = write(fd, curbuf, (mes->length - bw))) > 0) {
    bw += rc;
    curbuf = curbuf + rc;
  }

  log_trace("%s: send %lu bytes", cmd2str(cmd), bw);
  if (bw != mes->length) {
    log_error("%s: communication error", cmd2str(cmd));
    *out = (void *)strdup("Communication error");
    close(fd);
    *ret = -5;
    return -1;
  }

  /* read 2x8 bytes because the message structure is not packed and
   * the members are aligned on a 8-byte boundary */
  read_x_bytes(fd, &msg_type, sizeof(uint64_t));
  read_x_bytes(fd, &bufflen, sizeof(uint64_t));
  if (bufflen <= 0) {
    log_error("%s: message size could not be read: %d", cmd2str(cmd), bufflen);
    *out = (void *)strdup("Communication error");
    *ret = -5;
    return -1;
  }

  buf = (char *)malloc(bufflen);
  rep = ( message *)buf;
  rep->length = bufflen;
  rep->type = msg_type;

  read_x_bytes(fd, buf + 2 * sizeof(uint64_t), bufflen - 2 * sizeof(uint64_t));

  *ret = (int)rep->type;
  log_debug("%s: response message: %s", cmd2str(cmd), rep->value);

  *out = (void *)strdup(rep->value);
  close(fd);

  return 0;
}
