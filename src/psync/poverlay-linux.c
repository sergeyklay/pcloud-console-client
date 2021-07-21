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

#ifdef P_OS_LINUX
#include <sys/socket.h>
#include <sys/un.h>

#include "pcloudcc/psync/sockets.h"
#include "poverlay.h"
#include "plibs.h"
#include "logger.h"

void overlay_main_loop() {
  struct sockaddr_un addr;
  int fd, cl;

  const char *socket_path = psync_unix_socket_path();
  if (!socket_path) {
    log_debug("socket path is empty");
    return;
  }

  /* 1. Open a socket */
  if ((fd = socket(AF_UNIX, SOCK_STREAM, POVERLAY_PROTOCOL)) == -1) {
    log_error("failed to create socket %s", socket_path);
    return;
  }

  /* 2. Create an address */
  bzero((char *)&addr, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
  unlink(socket_path);

  /* 3. Bind the address to the socket */
  if (bind(fd, (struct sockaddr*)&addr, strlen(socket_path) + sizeof(addr.sun_family)) == -1) {
    log_error("failed bind the address to the socket: %s", strerror(errno));
    return;
  }

  /* 4. Listen for connections on the socket */
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
      instance_thread, // thread proc
      (void*)&cl       // thread parameter
    );
  }
}

void instance_thread(void* payload) {
  int *cl, rc;
  char chbuf[POVERLAY_BUFSIZE];
  message* request = NULL;
  char * curbuf = &chbuf[0];
  int bytes_read = 0;
  message* reply = (message*)psync_malloc(POVERLAY_BUFSIZE);

  memset(reply, 0, POVERLAY_BUFSIZE);
  memset(chbuf, 0, POVERLAY_BUFSIZE);

  cl = (int *)payload;

  while ((rc = read(*cl, curbuf, (POVERLAY_BUFSIZE - bytes_read))) > 0) {
    bytes_read += rc;
    log_debug("Read %u bytes: %u %s", bytes_read, rc, curbuf);
    curbuf = curbuf + rc;
    if (bytes_read > 12) {
      request = (message *)chbuf;
      if(request->length == bytes_read)
        break;
    }
  }

  if (rc == -1) {
    log_error("failed to read the socket");
    close(*cl);
    return;
  } else if (rc == 0) {
    log_info("received message from the socket");
    close(*cl);
  }

  request = (message *)chbuf;
  if (request) {
  get_answer_to_request(request, reply);
    if (reply ) {
      rc = write(*cl,reply,reply->length);
      if (rc != reply->length)
        log_error("socket reply not sent");
    }
  }

  if (cl) {
    close(*cl);
  }
};

#endif  /* P_OS_LINUX */
