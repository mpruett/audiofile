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

AF_VirtualFile *
af_virtual_file_new(void)
{
  return (AF_VirtualFile *) calloc(sizeof (AF_VirtualFile), 1);
}

void
af_virtual_file_destroy(AF_VirtualFile *vfile)
{
  vfile->destroy(vfile);

  free(vfile);
}

size_t
af_fread(void *data, size_t size, size_t nmemb, AF_VirtualFile *vfile)
{
  if(vfile->read) {
    int retval;

    retval = (* vfile->read) (vfile, data, size * nmemb);

    return retval/size;
  } else
    return 0;
}

size_t
af_fwrite(const void *data, size_t size, size_t nmemb, AF_VirtualFile *vfile)
{
  if(vfile->write) {
    int retval;

    retval = (* vfile->write) (vfile, data, size * nmemb);

    return retval/size;
  } else
    return 0;
}

int
af_fclose(AF_VirtualFile *vfile)
{
  af_virtual_file_destroy(vfile);

  return 0;
}

long
af_flength(AF_VirtualFile *vfile)
{
  if(vfile->length)
    return (* vfile->length)(vfile);
  else
    return 0;
}

int
af_fseek(AF_VirtualFile *vfile, long offset, int whence)
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
af_ftell(AF_VirtualFile *vfile)
{
  if(vfile->tell)
    return (* vfile->tell)(vfile);
  else
    return 0;
}

static int af_file_read(AF_VirtualFile *vfile, unsigned char *data, int nbytes);
static long af_file_length(AF_VirtualFile *vfile);
static int af_file_write(AF_VirtualFile *vfile, const unsigned char *data, int nbytes);
static void af_file_destroy(AF_VirtualFile *vfile);
static long af_file_seek(AF_VirtualFile *vfile, long offset, int is_relative);
static long af_file_tell(AF_VirtualFile *vfile);

AF_VirtualFile *
af_virtual_file_new_for_file(FILE *fh)
{
  AF_VirtualFile *vf;

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

static int
af_file_read(AF_VirtualFile *vfile, unsigned char *data, int nbytes)
{
  return fread(data, 1, nbytes, vfile->closure);
}

static long
af_file_length(AF_VirtualFile *vfile)
{
  long curpos, retval;

  curpos = ftell(vfile->closure);
  fseek(vfile->closure, 0, SEEK_END);
  retval = ftell(vfile->closure);
  fseek(vfile->closure, curpos, SEEK_SET);

  return retval;
}

static int
af_file_write(AF_VirtualFile *vfile, const unsigned char *data, int nbytes)
{
  return fwrite(data, 1, nbytes, vfile->closure);
}

static void
af_file_destroy(AF_VirtualFile *vfile)
{
  fclose(vfile->closure); vfile->closure = NULL;
}

static long
af_file_seek(AF_VirtualFile *vfile, long offset, int is_relative)
{
  fseek(vfile->closure, offset, is_relative?SEEK_CUR:SEEK_SET);

  return ftell(vfile->closure);
}

static long
af_file_tell(AF_VirtualFile *vfile)
{
  return ftell(vfile->closure);
}
