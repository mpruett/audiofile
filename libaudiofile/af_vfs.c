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

#include "afinternal.h"
#include "af_vfs.h"

#include <stdlib.h>

AFvirtualfile *
af_virtual_file_new(void)
{
  return (AFvirtualfile *) calloc(sizeof (AFvirtualfile), 1);
}

void
af_virtual_file_destroy(AFvirtualfile *vfile)
{
  vfile->destroy(vfile);

  free(vfile);
}

size_t af_fread (void *data, size_t size, size_t nmemb, AFvirtualfile *vfile)
{
  if (size == 0 || nmemb == 0)
    return 0;

  if (vfile->read) {
    int retval;

    retval = (* vfile->read) (vfile, data, size * nmemb);

    return retval/size;
  } else
    return 0;
}

size_t af_fwrite (const void *data, size_t size, size_t nmemb,
	AFvirtualfile *vfile)
{
  if (size == 0 || nmemb == 0)
    return 0;

  if (vfile->write) {
    int retval;

    retval = (* vfile->write) (vfile, data, size * nmemb);

    return retval/size;
  } else
    return 0;
}

int
af_fclose(AFvirtualfile *vfile)
{
  af_virtual_file_destroy(vfile);

  return 0;
}

long
af_flength(AFvirtualfile *vfile)
{
  if(vfile->length)
    return (* vfile->length)(vfile);
  else
    return 0;
}

int
af_fseek(AFvirtualfile *vfile, long offset, int whence)
{
  if(whence == SEEK_CUR)
    (* vfile->seek) (vfile, offset, 1);
  else if(whence == SEEK_SET)
    (* vfile->seek) (vfile, offset, 0);
  else
    return -1;

  return 0;
}

long
af_ftell(AFvirtualfile *vfile)
{
  if(vfile->tell)
    return (* vfile->tell)(vfile);
  else
    return 0;
}

static ssize_t af_file_read (AFvirtualfile *vfile, void *data, size_t nbytes);
static long af_file_length (AFvirtualfile *vfile);
static ssize_t af_file_write (AFvirtualfile *vfile, const void *data,
	size_t nbytes);
static void af_file_destroy(AFvirtualfile *vfile);
static long af_file_seek(AFvirtualfile *vfile, long offset, int is_relative);
static long af_file_tell(AFvirtualfile *vfile);

AFvirtualfile *
af_virtual_file_new_for_file(FILE *fh)
{
  AFvirtualfile *vf;

  if(!fh)
    return NULL;

  vf = af_virtual_file_new();
  vf->closure = fh;
  vf->read = af_file_read;
  vf->write = af_file_write;
  vf->length = af_file_length;
  vf->destroy = af_file_destroy;
  vf->seek = af_file_seek;
  vf->tell = af_file_tell;

  return vf;
}

static ssize_t af_file_read(AFvirtualfile *vfile, void *data, size_t nbytes)
{
	return fread(data, 1, nbytes, vfile->closure);
}

static long
af_file_length(AFvirtualfile *vfile)
{
  long curpos, retval;

  curpos = ftell(vfile->closure);
  fseek(vfile->closure, 0, SEEK_END);
  retval = ftell(vfile->closure);
  fseek(vfile->closure, curpos, SEEK_SET);

  return retval;
}

static ssize_t af_file_write (AFvirtualfile *vfile, const void *data,
	size_t nbytes)
{
	return fwrite(data, 1, nbytes, vfile->closure);
}

static void
af_file_destroy(AFvirtualfile *vfile)
{
  fclose(vfile->closure); vfile->closure = NULL;
}

static long
af_file_seek(AFvirtualfile *vfile, long offset, int is_relative)
{
  fseek(vfile->closure, offset, is_relative?SEEK_CUR:SEEK_SET);

  return ftell(vfile->closure);
}

static long
af_file_tell(AFvirtualfile *vfile)
{
  return ftell(vfile->closure);
}
