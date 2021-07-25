/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include "pcloudcc/psync/compat.h"
#include "pcloudcc/psync/overlay.h"

#if defined P_OS_POSIX
#include <sys/socket.h>

#ifdef P_OS_LINUX
#include <sys/un.h>
#include "config.h"
#include "pcloudcc/psync/stringcompat.h"  /* strlcpy */
#else
#include <stdlib.h>
#include <netinet/in.h>
#endif /* P_OS_LINUX */

#include "pcloudcc/psync/sockets.h"
#include "poverlay.h"
#include "logger.h"

void overlay_main_loop() {
  int fd, acc;

#ifdef P_OS_LINUX
  struct sockaddr_un addr;

  const char *socket_path = psync_unix_socket_path();
  if (!socket_path) {
    log_debug("socket path is empty");
    return;
  }

  /* Open a socket */
  if ((fd = socket(AF_UNIX, SOCK_STREAM, POVERLAY_PROTOCOL)) == -1) {
    log_error("failed to create socket %s", socket_path);
    return;
  }

  /* Create an address */
  bzero((char *)&addr, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strlcpy(addr.sun_path, socket_path, sizeof(addr.sun_path));
  unlink(socket_path);

  /* Bind the address to the socket */
  if (bind(fd, (struct sockaddr*)&addr, strlen(socket_path) + sizeof(addr.sun_family)) == -1) {
    log_error("failed bind the address to the socket: %s", strerror(errno));
    return;
  }
#else
  struct sockaddr_in addr;
  const int enable = 1;

  /* Open a socket */
  if ((fd = socket(PF_INET, SOCK_STREAM, POVERLAY_PROTOCOL)) == -1) {
    log_error("failed to create socket on port %u", POVERLAY_PORT);
    return;
  }

  /* Create an address */
  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(POVERLAY_PORT);

  /* Configure socket */
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
    log_error("failed to enable reuse of local address on socket: %s", strerror(errno));
    return;
  }

  /* Bind the address to the socket */
  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    log_error("failed bind the address to the socket: %s", strerror(errno));
    return;
  }
#endif

  /* Listen for connections on the socket */
  if (listen(fd, 5) == -1) {
    log_error("failed to listen for connections on the socket: %s", strerror(errno));
    return;
  }

  while (1) {
    log_trace("waiting for incoming connections...");
    /* 6. Accept connections */
    if ((acc = accept(fd, NULL, NULL)) == -1) {
      log_error("failed to accept connections: %s", strerror(errno));
      continue;
    }

    psync_run_thread1(
      "Pipe request handle routine",
      instance_thread, /* thread proc */
      (void *)&acc     /* thread parameter */
    );
  }
}

void instance_thread(void *payload) {
  int *fd;
  char chbuf[POVERLAY_BUFSIZE];
  overlay_message_t *request = NULL;
  char *curbuf = &chbuf[0];
  size_t ret, br = 0;
  overlay_message_t *response = (overlay_message_t *)psync_malloc(POVERLAY_BUFSIZE);

  memset(response, 0, POVERLAY_BUFSIZE);
  memset(chbuf, 0, POVERLAY_BUFSIZE);

  fd = (int *)payload;

  while ((ret = read(*fd, curbuf, (POVERLAY_BUFSIZE - br))) > 0) {
    br += ret;
    curbuf = curbuf + ret;
    log_trace("read %u bytes from socket", br);
    if (br > 12) {
      request = (overlay_message_t *)chbuf;
      if (request->length == br)
        break;
    }
  }

  if (ret == -1) {
    log_error("failed to read request payload: %s", strerror(errno));
    close(*fd);
    return;
  } else if (ret == 0) {
    log_info("received message from socket");
    close(*fd);
  }

  request = (overlay_message_t *)chbuf;
  if (request) {
    psync_overlay_process_request(request, response);
    if (response) {
      log_trace("got answer to request [%d]: %s", (int)response->type, response->value);
      ret = write(*fd, response, response->length);
      if (ret == -1) {
        log_error("failed to write to socket: %s", strerror(errno));
      } else if (ret != response->length) {
        log_error("failed writing response to socket");
      }
    }
  }

  if (*fd) {
    ret = close(*fd);
    if (ret == -1) {
      log_error("failed to close file descriptor: %s", strerror(errno));
    }
  }
};

#endif  /* P_OS_POSIX */
