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

#include "pcloudcc/psync/compat.h"
#include "pcloudcc/psync/sockets.h"
#include "overlay_client.h"
#include "logger.h"

/* TODO: Duplicate. See: poverlay.h. Move to sockets.h? */
typedef struct message_ {
  uint32_t type;
  uint64_t length;
  char value[];
} message;

static void read_x_bytes(int socket, unsigned int x, void *buffer) {
  int bytesRead = 0;
  int result;
  while (bytesRead < x) {
    result = read(socket, buffer + bytesRead, x - bytesRead);
    if (result < 1 ) {
      return;
    }
    bytesRead += result;
  }
}

int query_state(overlay_file_state_t *state, char *path) {
  int rep = 0;
  char * errm = NULL;

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

int send_call(int id, const char * path, int * ret, void * out) {
#ifdef P_OS_MACOSX
  struct sockaddr_in addr;
#else
  struct sockaddr_un addr;
#endif

  int fd,rc;
  int path_size = strlen(path);
  int mess_size = sizeof(message) + path_size + 1;
  int bytes_writen = 0;
  char *curbuf = NULL;
  char *buf = NULL;
  uint32_t bufflen = 0;
  char sendbuf[mess_size];
  message *rep = NULL;

  log_debug("send_call[%d]: start processing for the path: %s", id, path);

#ifdef P_OS_MACOSX
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == - 1) {
    if (out)
      /* TODO: Is this works? */
      out = (void *)strndup("Unable to create INET socket", 28);
    *ret = - 1;
    return - 1;
  }

  memset (&addr, 0, sizeof (addr));
  addr. sin_family = AF_INET;
  addr. sin_addr . s_addr = htonl(INADDR_LOOPBACK);
  addr. sin_port = htons(POVERLAY_PORT);
  if (connect (fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == - 1) {
    if (out)
      /* TODO: Is this works? */
      out = (void *)strndup("Unable to connect to INET socket", 32);
    *ret = - 2;
    return - 2;
  }
#else
  const char *socket_path = psync_unix_socket_path();
  if (!socket_path) {
    log_error("send_call[%d]: socket path is empty", id);
    if (out)
      out = (void *)strdup("Unable to create UNIX socket");
    *ret = - 1;
    return -1;
  }

  /* 1. Open a socket */
  if ((fd = socket(AF_UNIX, SOCK_STREAM, POVERLAY_PROTOCOL)) == -1) {
    log_error("send_call[%d]: failed to create socket %s", id, socket_path);
    if (out)
      out = (void *)strdup("Unable to create UNIX socket");
    *ret = - 3;
    return - 3;
  }

  /* 2. Create an address */
  bzero((char *)&addr, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

  /* 3. Initiate a connection on a socket */
  if (connect(fd, (struct sockaddr*)&addr, SUN_LEN(&addr)) == - 1) {
    log_error("send_call[%d]: failed to connect to socket %s",
              id, strerror(errno));
    if (out)
      out = (void *)strdup("Unable to connect to UNIX socket");
    *ret = - 4;
    return - 4;
  }
#endif

  message *mes = (message *)sendbuf;
  memset (mes, 0, mess_size);
  mes->type = id;
  strncpy(mes->value, path, path_size);
  mes->length = mess_size;
  curbuf = (char *)mes;

  while ((rc = write(fd,curbuf,(mes-> length - bytes_writen))) > 0) {
    bytes_writen += rc;
    curbuf = curbuf + rc;
  }

  log_debug("send_call[%d]: send %d bytes", id, bytes_writen);
  if (bytes_writen != mes->length) {
    log_error("send_call[%d]: communication error", id);
    if (out)
      out = (void *)strdup("Communication error");
      close(fd);
    *ret = - 5;
    return - 5;
  }

  read_x_bytes(fd, 4, &bufflen);
  if (bufflen <= 0) {
    log_error("send_call[%d]: message size could not be read: %d", id, bufflen);
    return -6;
  }

  buf = (char *)malloc(bufflen);
  rep = ( message *)buf;
  rep->length = bufflen;

  read_x_bytes(fd, bufflen - 4, buf + 4);

  *ret = rep->type;
  if (out)
    out = (void *)strndup(rep->value, rep->length - sizeof(message));
  close(fd);

  return 0;
}
