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

#ifdef P_OS_WINDOWS

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#define POVERLAY_BUFSIZE 600
#define MAX_SEM_COUNT 10
#define THREADCOUNT 12

#include "poverlay.h"
#include "logger.h"

LPCWSTR PORT = TEXT("\\\\.\\pipe\\pStatusPipe");

void overlay_main_loop() {
  BOOL   fConnected = FALSE;
  HANDLE hPipe = INVALID_HANDLE_VALUE;
  HANDLE ghSemaphore;
  DWORD dwWaitResult;

  ghSemaphore = CreateSemaphore(
    NULL,           /* default security attributes */
    MAX_SEM_COUNT,  /* initial count */
    MAX_SEM_COUNT,  /* maximum count */
    NULL);          /* unnamed semaphore */

  if (ghSemaphore == NULL) {
    printf("failed create semaphore: %d", GetLastError());
    return 1;
  }

  for (;;) {
    log_debug("main thread awaiting client connection on %s", PORT);

    dwWaitResult = WaitForSingleObject(
      ghSemaphore, /* handle to semaphore */
      INFINITE     /* zero-second time-out interval */
    );

    hPipe = CreateNamedPipe(
      PORT,                     /* pipe name */
      PIPE_ACCESS_DUPLEX,       /* read/write access */
      PIPE_TYPE_MESSAGE |       /* message type pipe */
      PIPE_READMODE_MESSAGE |   /* message-read mode */
      PIPE_WAIT,                /* blocking mode */
      PIPE_UNLIMITED_INSTANCES, /* max. instances */
      POVERLAY_BUFSIZE,         /* output buffer size */
      POVERLAY_BUFSIZE,         /* input buffer size */
      0,                        /* client time-out */
      NULL);                    /* default security attribute */

    if (hPipe == INVALID_HANDLE_VALUE) {
      log_debug("failed create named pipe: %d", GetLastError());
      return;
    }

    fConnected = ConnectNamedPipe(hPipe, NULL) ?
        TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (fConnected) {
      log_info("client connected, creating a processing thread");

      /* Create a thread for this client. */
      psync_run_thread1(
        "Pipe request handle routine",
        instance_thread, /* thread proc */
        (void*)hPipe     /* thread parameter */
      );
    }
    else
      CloseHandle(hPipe);

    if (!ReleaseSemaphore(
        ghSemaphore,  /* handle to semaphore */
        1,            /* increase count by one */
        NULL)) {      /* not interested in previous count */
      log_warn("failed release semaphore: %d", GetLastError());
    }
  }

  CloseHandle(ghSemaphore);
}

void instance_thread(void* payload) {
  DWORD cbBytesRead = 0, cbWritten = 0;
  BOOL fSuccess = FALSE;
  HANDLE hPipe = NULL;
  char  chBuf[POVERLAY_BUFSIZE];

  message* request = NULL;
  message* reply = (message*)psync_malloc(POVERLAY_BUFSIZE);
  memset(reply, 0, POVERLAY_BUFSIZE);
  if (payload == NULL) {
    log_error("InstanceThread got an unexpected NULL value in payload.");
    return;
  }

  log_info("InstanceThread created, receiving and processing messages.");
  hPipe = (HANDLE)payload;
  while (1) {
    do {
      fSuccess = ReadFile(
        hPipe,             /* pipe handle */
        chBuf,             /* buffer to receive reply */
        POVERLAY_BUFSIZE,  /* size of buffer */
        &cbBytesRead,      /* number of bytes read */
        NULL               /* not overlapped */
      );
      if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
        break;
    } while (!fSuccess);  /* repeat loop if ERROR_MORE_DATA */

    if (!fSuccess || cbBytesRead == 0) {
      if (GetLastError() == ERROR_BROKEN_PIPE) {
        log_info("InstanceThread: client disconnected.");
      }
      break;
    }
    message *request = (message *)chBuf;

    log_debug("bytes received  %d buffer[%s]", cbBytesRead, chBuf);
    get_answer_to_request(request, reply);
    fSuccess = WriteFile(
      hPipe,         /* handle to pipe */
      reply,         /* buffer to write from */
      reply->length, /* number of bytes to write */
      &cbWritten,    /* number of bytes written */
      NULL);         /* not overlapped I/O */

    if (!fSuccess || reply->length != cbWritten) {
      log_debug("InstanceThread WriteFile failed, GLE=%d", GetLastError());
      break;
    }
  }

  FlushFileBuffers(hPipe);
  DisconnectNamedPipe(hPipe);
  CloseHandle(hPipe);
  psync_free(request);
  psync_free(reply);
}

#endif  /* P_OS_WINDOWS */
