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
  int fd, cl;

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
    /* 6. Accept connections */
    if ((cl = accept(fd, NULL, NULL)) == -1) {
      log_error("failed to accept connections: %s", strerror(errno));
      continue;
    }

    psync_run_thread1(
      "Pipe request handle routine",
      instance_thread, /* thread proc */
      (void*)&cl       /* thread parameter */
    );
  }
}

void instance_thread(void *payload) {
  int *cl;
  char chbuf[POVERLAY_BUFSIZE];
  message *request = NULL;
  char *curbuf = &chbuf[0];
  size_t ret, br = 0;
  message *response = (message *)psync_malloc(POVERLAY_BUFSIZE);

  memset(response, 0, POVERLAY_BUFSIZE);
  memset(chbuf, 0, POVERLAY_BUFSIZE);

  cl = (int *)payload;

  while ((ret = read(*cl, curbuf, (POVERLAY_BUFSIZE - br))) > 0) {
    br += ret;
    log_debug("read %u bytes: %u %s", br, ret, curbuf);
    curbuf = curbuf + ret;
    if (br > 12) {
      request = (message *)chbuf;
      if (request->length == br)
        break;
    }
  }

  if (ret == -1) {
    log_error("failed to read request payload: %s", strerror(errno));
    close(*cl);
    return;
  } else if (ret == 0) {
    log_info("received message from socket");
    close(*cl);
  }

  request = (message *)chbuf;
  if (request) {
    get_answer_to_request(request, response);
    if (response) {
      ret = write(*cl, response, response->length);
      if (ret == -1) {
        log_error("failed to write to socket: %s", strerror(errno));
      } else if (ret != response->length) {
        log_error("socket response not sent");
      }
    }
  }

  if (cl) {
    close(*cl);
  }
};

#endif  /* P_OS_POSIX */
