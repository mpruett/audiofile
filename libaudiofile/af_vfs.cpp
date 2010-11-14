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
	af_vfs.c

	Virtual file operations for the Audio File Library.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "afinternal.h"
#include "af_vfs.h"
#include "File.h"
#include <string.h>
#include <errno.h>

#include <stdlib.h>

ssize_t af_read (void *data, size_t size, AFvirtualfile *vfile)
{
	return vfile->read(data, size);
}

ssize_t af_write (const void *data, size_t size, AFvirtualfile *vfile)
{
	return vfile->write(data, size);
}

int af_fclose (AFvirtualfile *vfile)
{
	vfile->close();
	delete vfile;
	return 0;
}

long af_flength (AFvirtualfile *vfile)
{
	off_t length = vfile->length();
	return length;
}

int af_fseek (AFvirtualfile *vfile, long offset, int whence)
{
	if (whence == SEEK_CUR)
		vfile->seek(offset, File::SeekFromCurrent);
	else if (whence == SEEK_SET)
		vfile->seek(offset, File::SeekFromBeginning);
	else
		return -1;
	return 0;
}

long af_ftell (AFvirtualfile *vfile)
{
	return vfile->tell();
}
