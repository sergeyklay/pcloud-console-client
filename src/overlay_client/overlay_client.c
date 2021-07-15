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

#include "overlay_client.h"
#include "debug.h"
#include "logger.h"

#define POVERLAY_BUFSIZE 512

typedef struct _message {
uint32_t type;
uint64_t length;
char value[];
} message;

static void read_x_bytes(int socket, unsigned int x, void * buffer) {
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

#if defined(P_OS_MACOSX)
uint32_t clport = 8989;
#else
char *clsoc = "/tmp/pcloud_unix_soc.sock";
#endif

int QueryState( pCloud_FileState *state, char * path) {
  int rep = 0;
  char * errm;
  if (!SendCall(4, path, &rep, &errm)) {
    log_info("QueryState response rep[%d] path[%s]", rep, path);
    if (errm)
      log_error("The error is %s", errm);
    if (rep == 10)
      *state = FileStateInSync;
    else if (rep == 12)
      *state = FileStateInProgress;
    else if (rep == 11)
      *state = FileStateNoSync;
    else
      *state = FileStateInvalid;
  } else
    log_error("QueryState ERROR rep[%d] path[%s]", rep, path);
  free (errm);

  return 0;
}

int SendCall( int id, const char * path, int * ret, void * out) {
  #if defined(P_OS_MACOSX)
  struct sockaddr_in addr;
  #else
  struct sockaddr_un addr;
  #endif

  int fd,rc;
  int path_size = strlen (path);
  int mess_size = sizeof ( message )+path_size + 1;
  int bytes_writen = 0;
  char *curbuf = NULL;
  char *buf = NULL;
  uint32_t bufflen=0;
  char sendbuf[mess_size];
  int bytes_read = 0;
  message *rep = NULL;

  log_info("SenCall id: %d path: %s", id, path);

  #if defined(P_OS_MACOSX)
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
  addr. sin_port = htons(clport);
  if (connect (fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == - 1) {
    if (out)
      /* TODO: Is this works? */
      out = (void *)strndup("Unable to connect to INET socket", 32);
    *ret = - 2;
    return - 2;
  }
  #else
  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == - 1) {
    if (out)
      out = (void *)strndup("Unable to create UNIX socket", 27);
    *ret = - 3;
    return - 3;
   }
  memset(&addr, 0, sizeof (addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, clsoc, sizeof (addr.sun_path)- 1 );

  if (connect(fd, ( struct sockaddr*)&addr,SUN_LEN(&addr)) == - 1 ) {
    if (out)
     out = (void *)strndup( "Unable to connect to UNIX socket", 32 );
    *ret = - 4;
    return - 4;
  }
  #endif

  message * mes = (message *)sendbuf;
  memset (mes, 0, mess_size);
  mes-> type = id;
  strncpy (mes-> value, path, path_size);
  mes-> length = mess_size;
  curbuf = (char *)mes;
  while ((rc = write (fd,curbuf,(mes-> length - bytes_writen))) > 0 ) {
    bytes_writen += rc;
    curbuf = curbuf + rc;
  }
  log_info("QueryState bytes send [%d]", bytes_writen);
  if (bytes_writen != mes-> length) {
    if (out)
      out = strndup ("Communication error", 19);
    close(fd);
    *ret = - 5;
    return - 5;
  }

  read_x_bytes(fd, 4, &bufflen);
  if (bufflen <= 0) {
    log_info("Message size could not be read: [%d]", bufflen);
    return -6;
  }
  buf = (char *)malloc(bufflen);
  rep = ( message *)buf;
  rep->length = bufflen;

  read_x_bytes(fd, bufflen - 4, buf + 4);

  *ret = rep-> type;
  if (out)
    out = strndup (rep-> value, rep-> length - sizeof(message));

  close(fd);

  return 0;
}
