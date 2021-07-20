/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include "pcloudcc/psync/version.h"
#include "pcloudcc/psync/compat.h"
#include "pdeviceid.h"
#include "plibs.h"
#include "logger.h"

/* TODO: amend headers here */
#ifdef P_OS_WINDOWS
#include <winuser.h>
#elif defined(P_OS_MACOS)
#include <stdlib.h>
#elif defined(P_OS_POSIX)
#include <dirent.h>
#endif  /* P_OS_WINDOWS */

#ifdef __cplusplus
extern "C" {
#endif

static const char *psync_software_name = PSYNC_VERSION_STRING;
static const char *psync_os_name = NULL;

/* TODO: Consider to rename to avoid confusion with generate_device_id() */
char *psync_get_device_id() {
  char *device;
#ifdef P_OS_WINDOWS
  SYSTEM_POWER_STATUS bat;
  const char *hardware, *ver;
  char versbuff[32];
  DWORD vers, vmajor, vminor;
  if (GetSystemMetrics(SM_TABLETPC))
    hardware = "Tablet";
  else if (GetSystemPowerStatus(&bat) || (bat.BatteryFlag & 128))
    hardware = "Desktop";
  else
    hardware = "Laptop";
  vers = GetVersion();
  vmajor = (DWORD)(LOBYTE(LOWORD(vers)));
  vminor = (DWORD)(HIBYTE(LOWORD(vers)));
  if (vmajor == 6) {
    switch (vminor) {
      case 3: ver = "8.1"; break;
      case 2: ver = "8.0"; break;
      case 1: ver = "7.0"; break;
      case 0: ver = "Vista"; break;
      default:
        psync_slprintf(versbuff, sizeof(versbuff), "6.%u", (unsigned int)vminor);
        ver = versbuff;
    }
  } else if (vmajor == 5) {
    switch (vminor) {
      case 2: ver = "XP 64bit"; break;
      case 1: ver = "XP"; break;
      case 0: ver = "2000"; break;
      default:
        psync_slprintf(versbuff, sizeof(versbuff), "5.%u", (unsigned int)vminor);
        ver = versbuff;
    }
  } else if (vmajor == 10) {
    switch (vminor) {
      case 0: ver = "10.0"; break;
      default:
        psync_slprintf(versbuff, sizeof(versbuff), "10.%u", (unsigned int)vminor);
        ver = versbuff;
    }
  } else {
    psync_slprintf(versbuff, sizeof(versbuff), "%u.%u", (unsigned int)vmajor, (unsigned int)vminor);
    ver = versbuff;
  }

  device = psync_strcat(hardware, ", Windows ", ver, NULL);
#elif defined(P_OS_MACOSX)
  struct utsname un;
  const char *ver;
  size_t len;
  char versbuff[64], modelname[256];
  int v;
  if (uname(&un))
    ver="Mac OS X";
  else {
    v = atoi(un.release);
    switch (v) {
      case 21: ver="macOS 12 Monterey"; break;
      case 20: ver="macOS 11 Big Sur"; break;
      case 19: ver="macOS 10.15 Catalina"; break;
      case 18: ver="macOS 10.14 Mojave"; break;
      case 17: ver="macOS 10.13 High Sierra"; break;
      case 16: ver="macOS 10.12 Sierra"; break;
      case 15: ver="OS X 10.11 El Capitan"; break;
      case 14: ver="OS X 10.10 Yosemite"; break;
      case 13: ver="OS X 10.9 Mavericks"; break;
      case 12: ver="OS X 10.8 Mountain Lion"; break;
      case 11: ver="OS X 10.7 Lion"; break;
      case 10: ver="OS X 10.6 Snow Leopard"; break;
      default:
        psync_slprintf(versbuff, sizeof(versbuff), "Mac/Darwin %s", un.release);
        ver = versbuff;
    }
  }
  len = sizeof(modelname);
  if (sysctlbyname("hw.model", modelname, &len, NULL, 0))
    strlcpy(modelname, "Mac", len);
  versbuff[sizeof(versbuff) - 1] = 0;
  device = psync_strcat(modelname, ", ", ver, NULL);
#elif defined(P_OS_LINUX)
  DIR *dh;
  struct dirent *ent;
  const char *hardware;
  char *path, buf[8];
  int fd;
  hardware = "Desktop";

  dh = opendir("/sys/class/power_supply");
  if (dh) {
    while (NULL != (ent = readdir(dh))) {
      if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
        continue;
      }

      path = psync_strcat("/sys/class/power_supply/", ent->d_name, "/type", NULL);
      fd = open(path, O_RDONLY);
      psync_free(path);

      if (fd == -1)
        continue;

      if (read(fd, buf, 7) == 7 && !memcmp(buf, "Battery", 7)) {
        close(fd);
        hardware = "Laptop";
        break;
      }

      close(fd);
    }

    closedir(dh);
  } else {
    log_error("failed to open /sys/class/power_supply: %s", strerror(errno));
  }

  device = psync_strcat(hardware, ", Linux", NULL);
#else
  device=psync_strcat("Desktop", NULL);
#endif
  /* TODO: Rephrase to avoid confusion with log in get_connected_socket() */
  log_info("detected device: %s", device);
  return device;
}

void psync_set_os_name(const char *name) {
  psync_os_name = name;
}

void psync_set_software_name(const char *name) {
  psync_software_name = name;
}

const char *psync_get_software_name() {
  if (psync_software_name) {
    return psync_software_name;
  }

  return PSYNC_VERSION_STRING;
}

char *psync_get_device_os() {
  return psync_os_name ? psync_strdup(psync_os_name) : psync_get_device_id();
}

char *psync_get_device_string() {
  /* TODO: Do we need ifdef here? */
#ifdef P_OS_LINUX
  char *osname = psync_get_device_os();
  char *ret = psync_strcat(osname, ", ", psync_software_name, NULL);
  free(osname);
  return ret;
#endif
  return psync_strcat(psync_get_device_id(), ", ", psync_software_name, NULL);
}

#ifdef __cplusplus
}  /* extern "C" */
#endif
