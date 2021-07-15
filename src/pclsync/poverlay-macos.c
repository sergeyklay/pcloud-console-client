/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include "pcloudcc/pcompat.h"

#if defined(P_OS_MACOSX) || defined(P_OS_BSD)

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "poverlay.h"
#include "logger.h"

#define POVERLAY_BUFSIZE  512
#define POVERLAY_PORT     8989
#define POVERLAY_PROTOCOL 0

void overlay_main_loop() {
  struct sockaddr_in addr;
  int fd, cl;
  const int enable = 1;

  /* 1. Open a socket */
  if ((fd = socket(PF_INET, SOCK_STREAM, POVERLAY_PROTOCOL)) == -1) {
    log_error("failed to create socket on port %u", POVERLAY_PORT);
    return;
  }

  /* 2. Create an address */
  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(POVERLAY_PORT);

  /* 3. Configure socket */
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
    log_error("failed to enable reuse of local address on socket: %s", strerror(errno));
    return;
  }

  /* 4. Bind the address to the socket */
  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    log_error("failed bind the address to the socket: %s", strerror(errno));
    return;
  }

  /* 5. Listen for connections on the socket */
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
  char *curbuf = &chbuf[0];
  int bytes_read = 0;
  message* reply = (message*)psync_malloc(POVERLAY_BUFSIZE);

  memset(reply, 0, POVERLAY_BUFSIZE);
  memset(chbuf, 0, POVERLAY_BUFSIZE);

  cl = (int *)payload;

  while ((rc = read(*cl, curbuf, (POVERLAY_BUFSIZE - bytes_read))) > 0) {
    bytes_read += rc;
    log_debug("Read %u bytes: %u %s", bytes_read, rc, curbuf );
    curbuf = curbuf + rc;
    if (bytes_read > 12){
      request = (message *)chbuf;
      if (request->length == bytes_read)
        break;
    }
  }

  if (rc == -1) {
    log_error("TCP/IP socket read");
    close(*cl);
    return;
  } else if (rc == 0) {
    log_debug("Message received");
    close(*cl);
  }
  request = (message *)chbuf;
  if (request) {
    get_answer_to_request(request, reply);
    if (reply) {
      rc = write(*cl, reply, reply->length);
      if (rc != reply->length)
        log_error("TCP/IP  socket reply not sent.");
    }
  }

  if (cl) {
    close(*cl);
  }
};

#endif  /* defined(P_OS_MACOSX) || defined(P_OS_BSD) */
