/*
	Audio File Library
	Copyright (C) 1999, Elliot Lee <sopwith@redhat.com>

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA  02111-1307  USA.
*/

/*
	af_vfs.h

	Virtual file operations for the Audio File Library.
*/

#ifndef AUDIOFILE_VFS_H
#define AUDIOFILE_VFS_H 1

#include <stdio.h>
#include <audiofile.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef class File AFvirtualfile;
#else
typedef struct File AFvirtualfile;
#endif

ssize_t af_read (void *data, size_t size, AFvirtualfile *vfile);
ssize_t af_write (const void *data, size_t size, AFvirtualfile *vfile);
int af_fclose (AFvirtualfile *vfile);
long af_flength (AFvirtualfile *vfile);
int af_fseek (AFvirtualfile *vfile, long offset, int whence);
long af_ftell (AFvirtualfile *vfile);

#ifdef __cplusplus
}
#endif

#endif
