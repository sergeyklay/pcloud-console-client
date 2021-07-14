/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef OVERLAY_CLIENT_H
#define OVERLAY_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _pCloud_FileState
{
  FileStateInSync = 0,
  FileStateNoSync,
  FileStateInProgress,
  FileStateInvalid
}pCloud_FileState;

int QueryState(pCloud_FileState *state /*OUT*/, char* path /*IN*/);

int SendCall( int id /*IN*/ ,const char * path /*IN*/ , int * ret /*OUT*/ , void * out /*OUT*/ );
#ifdef __cplusplus
}
#endif
#endif //OVERLAY_CLIENT_H
