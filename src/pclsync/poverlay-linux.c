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

#ifdef P_OS_LINUX

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#include "poverlay.h"
#include "plibs.h"
#include "logger.h"

#define POVERLAY_BUFSIZE 512

char *mysoc = "/tmp/pcloud_unix_soc.sock";

void overlay_main_loop() {
  struct sockaddr_un addr;
  int fd,cl;

  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    log_error("Unix socket error failed to open %s", mysoc);
    return;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, mysoc, sizeof(addr.sun_path)-1);

  unlink(mysoc);

  if (bind(fd, (struct sockaddr*)&addr, strlen(mysoc) + sizeof(addr.sun_family)) == -1) {
    log_error("Unix socket bind error");
    return;
  }

  if (listen(fd, 5) == -1) {
    log_error("Unix socket listen error");
    return;
  }

  while (1) {
    if ( (cl = accept(fd, NULL, NULL)) == -1) {
      log_error("Unix socket accept error");
      continue;
    }
    psync_run_thread1(
      "Pipe request handle routine",
      instance_thread,    // thread proc
      (void*)&cl     // thread parameter
      );
  }
}

void instance_thread(void* payload)
{
  int *cl, rc;
  char  chbuf[POVERLAY_BUFSIZE];
  message* request = NULL;
  char * curbuf = &chbuf[0];
  int bytes_read = 0;
  message* reply = (message*)psync_malloc(POVERLAY_BUFSIZE);

  memset(reply, 0, POVERLAY_BUFSIZE);
  memset(chbuf, 0, POVERLAY_BUFSIZE);

  cl = (int *)payload;

  while ( (rc=read(*cl,curbuf,(POVERLAY_BUFSIZE - bytes_read))) > 0) {
    bytes_read += rc;
    log_error( "Read %u bytes: %u %s", bytes_read, rc, curbuf );
    curbuf = curbuf + rc;
    if (bytes_read > 12) {
      request = (message *)chbuf;
      if(request->length == bytes_read)
        break;
    }
  }
  if (rc == -1) {
    log_error("Unix socket read");
    close(*cl);
    return;
  }
  else if (rc == 0) {
    log_info("Message received");
    close(*cl);
  }
  request = (message *)chbuf;
  if (request) {
  get_answer_to_request(request, reply);
    if (reply ) {
      rc = write(*cl,reply,reply->length);
      if (rc != reply->length)
        log_error("Unix socket reply not sent.");

    }
  }
  if (cl) {
    close(*cl);
  }
};

#endif  /* P_OS_LINUX */
