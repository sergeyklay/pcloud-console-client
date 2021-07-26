/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2015 Anton Titov <anton@pcloud.com>
 * (c) 2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PCOMPRESSION_H_
#define PCLOUD_PSYNC_PCOMPRESSION_H_

struct _psync_deflate_t;

typedef struct _psync_deflate_t psync_deflate_t;

#define PSYNC_DEFLATE_DECOMPRESS   0
#define PSYNC_DEFLATE_COMP_FASTEST 1
#define PSYNC_DEFLATE_COMP_FAST    2
#define PSYNC_DEFLATE_COMP_MED     6
#define PSYNC_DEFLATE_COMP_BEST    9

#define PSYNC_DEFLATE_NOFLUSH    0
#define PSYNC_DEFLATE_FLUSH      1
#define PSYNC_DEFLATE_FLUSH_END  2

#define PSYNC_DEFLATE_NODATA (-1)
#define PSYNC_DEFLATE_FULL   (-2)
#define PSYNC_DEFLATE_ERROR  (-3)
#define PSYNC_DEFLATE_EOF      0


psync_deflate_t *psync_deflate_init(int level);
void psync_deflate_destroy(psync_deflate_t *def);
int psync_deflate_write(psync_deflate_t *def, const void *data, int len, int flush);
int psync_deflate_read(psync_deflate_t *def, void *data, int len);
int psync_deflate_pending(psync_deflate_t *def);

#endif /* PCLOUD_PSYNC_PCOMPRESSION_H_ */
