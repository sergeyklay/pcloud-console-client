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

#if defined(P_OS_LINUX) || defined(P_OS_MACOSX) || defined(P_OS_BSD)

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#include "plibs.h"
#include "poverlay.h"

#define POVERLAY_BUFSIZE 512

char *mysoc = "/tmp/pcloud_unix_soc.sock";

void overlay_main_loop()
{
  struct sockaddr_un addr;
  int fd,cl;

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    //debug(D_NOTICE, "Unix socket error failed to open %s", mysoc);
    return;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, mysoc, sizeof(addr.sun_path)-1);

  unlink(mysoc);

  if (bind(fd, (struct sockaddr*)&addr,  strlen(mysoc) + sizeof(addr.sun_family)) == -1) {
    debug(D_ERROR,"Unix socket bind error");
    return;
  }

  if (listen(fd, 5) == -1) {
    debug(D_ERROR,"Unix socket listen error");
    return;
  }

  while (1) {
    if ( (cl = accept(fd, NULL, NULL)) == -1) {
      debug(D_ERROR,"Unix socket accept error");
      continue;
    }
    psync_run_thread1(
      "Pipe request handle routine",
      instance_thread,    // thread proc
      (LPVOID)&cl     // thread parameter
      );
  }

  return;
}

void instance_thread(void* lpvParam)
{
  int *cl, rc;
  char  chbuf[POVERLAY_BUFSIZE];
  message* request = NULL;
  char * curbuf = &chbuf[0];
  int bytes_read = 0;
  message* reply = (message*)psync_malloc(POVERLAY_BUFSIZE);

  memset(reply, 0, POVERLAY_BUFSIZE);
  memset(chbuf, 0, POVERLAY_BUFSIZE);

  cl = (int *)lpvParam;

  while ( (rc=read(*cl,curbuf,(POVERLAY_BUFSIZE - bytes_read))) > 0) {
    bytes_read += rc;
    //debug(D_ERROR, "Read %u bytes: %u %s", bytes_read, rc, curbuf );
    curbuf = curbuf + rc;
    if (bytes_read > 12){
      request = (message *)chbuf;
      if(request->length == bytes_read)
        break;
    }
  }
  if (rc == -1) {
    //debug(D_ERROR,"Unix socket read");
    close(*cl);
    return;
  }
  else if (rc == 0) {
    //debug(D_NOTICE,"Message received");
    close(*cl);
  }
  request = (message *)chbuf;
  if (request) {
  get_answer_to_request(request, reply);
    if (reply ) {
      rc = write(*cl,reply,reply->length);
      if (rc != reply->length)
        debug(D_ERROR,"Unix socket reply not sent.");

    }
  }
  if (cl) {
    close(*cl);
  }
  //debug(D_NOTICE, "InstanceThread exitting.\n");
  return;
};

#endif //defined(P_OS_LINUX) || definef(P_OS_MACOSX) || defined(P_OS_BSD)
