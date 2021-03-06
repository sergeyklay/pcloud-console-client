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
#include "pcloudcc/psync/deviceid.h"
#include "pssl.h"
#include "plibs.h"
#include "logger.h"

#if defined(P_OS_MACOSX)
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#else
#include <dirent.h>
#endif  /* P_OS_MACOSX */

static const char *psync_software_name = PSYNC_VERSION_FULL;
static const char *psync_os_name = NULL;

static char *psync_detect_os_name() {
  char *device;
#ifdef P_OS_MACOSX
  struct utsname un;
  const char *ver;
  char *endptr;
  size_t len;
  char versbuff[64], modelname[256];
  long v;

  if (uname(&un))
    ver = "macOS";
  else {
    v = strtol(un.release, &endptr, 10);
    /*  out of range,      not X.Y.Z,        no conversion at all */
    if (errno == ERANGE || *endptr != '.' || un.release == endptr) {
      log_error("failed determine OS release: %s", strerror(errno));
      v = 0;
    }
#if LONG_MIN < INT_MIN || LONG_MAX > INT_MAX
    else if (v < INT_MIN || v > INT_MAX) {
      errno = ERANGE;
      log_error("failed determine OS release: %s", strerror(errno));
      v = 0;
    }
#endif

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
  log_info("automatically detected device: %s", device);
  return device;
}

void psync_set_os_name(const char *name) {
  psync_os_name = name;
}

char *psync_get_os_name() {
  return psync_os_name ? psync_strdup(psync_os_name) : psync_detect_os_name();
}

void psync_set_software_name(const char *name) {
  psync_software_name = name;
}

const char *psync_get_software_name() {
  if (psync_software_name) {
    return psync_software_name;
  }

  return PSYNC_VERSION_FULL;
}

char *psync_get_device_string() {
  char *osname = psync_get_os_name();
  char *ret = psync_strcat(osname, ", ", psync_software_name, NULL);
  free(osname);
  return ret;
}

static char *generate_device_id() {
  unsigned char device_id_bin[16];
  char device_id_hex[32+2];

  psync_ssl_rand_strong(device_id_bin, sizeof(device_id_bin));
  psync_binhex(device_id_hex, device_id_bin, sizeof(device_id_bin));

  device_id_hex[sizeof(device_id_bin) * 2] = 0;
  psync_sql_res *query = psync_sql_prep_statement(
      "REPLACE INTO setting (id, value) VALUES ('deviceid', ?)"
  );

  psync_sql_bind_string(query, 1, device_id_hex);
  psync_sql_run_free(query);

  return psync_strdup(device_id_hex);
}

char *psync_get_device_id() {
  char *deviceid = psync_sql_cellstr(
      "SELECT value FROM setting WHERE id='deviceid'"
  );

  if (!deviceid) {
    deviceid = generate_device_id();
  }

  return deviceid;
}
