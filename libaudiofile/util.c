/*
	Audio File Library
	Copyright (C) 1998-2000, Michael Pruett <michael@68k.org>
	Copyright (C) 2000, Silicon Graphics, Inc.

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
	util.c

	This file contains general utility routines for the Audio File
	Library.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "audiofile.h"
#include "aupvlist.h"

#include "afinternal.h"
#include "util.h"
#include "units.h"
#include "compression.h"
#include "modules.h"
#include "byteorder.h"
#include "aupvinternal.h"

extern _PCMInfo _af_default_signed_integer_pcm_mappings[];
extern _PCMInfo _af_default_unsigned_integer_pcm_mappings[];
extern _PCMInfo _af_default_float_pcm_mapping;
extern _PCMInfo _af_default_double_pcm_mapping;

extern _CompressionUnit _af_compression[];

/*
	_af_filesetup_ok and _af_filehandle_ok are sanity check routines
	which are called at the beginning of every external subroutine.
*/
bool _af_filesetup_ok (AFfilesetup setup)
{
	if (setup == AF_NULL_FILESETUP)
	{
		_af_error(AF_BAD_FILESETUP, "null file setup");
		return AF_FALSE;
	}
	if (setup->valid != _AF_VALID_FILESETUP)
	{
		_af_error(AF_BAD_FILESETUP, "invalid file setup");
		return AF_FALSE;
	}
	return AF_TRUE;
}

bool _af_filehandle_can_read (AFfilehandle file)
{
	if (file->access != _AF_READ_ACCESS)
	{
		_af_error(AF_BAD_NOREADACC, "file not opened for read access");
		return AF_FALSE;
	}

	return AF_TRUE;
}

bool _af_filehandle_can_write (AFfilehandle file)
{
	if (file->access != _AF_WRITE_ACCESS)
	{
		_af_error(AF_BAD_NOWRITEACC, "file not opened for write access");
		return AF_FALSE;
	}

	return AF_TRUE;
}

bool _af_filehandle_ok (AFfilehandle file)
{
	if (file == AF_NULL_FILEHANDLE)
	{
		_af_error(AF_BAD_FILEHANDLE, "null file handle");
		return AF_FALSE;
	}
	if (file->valid != _AF_VALID_FILEHANDLE)
	{
		_af_error(AF_BAD_FILEHANDLE, "invalid file handle");
		return AF_FALSE;
	}
	return AF_TRUE;
}

void *_af_malloc (size_t size)
{
	void	*p;

	if (size <= 0)
	{
		_af_error(AF_BAD_MALLOC, "bad memory allocation size request %d", size);
		return NULL;
	}

	p = malloc(size);

#ifdef AF_DEBUG
	if (p)
		memset(p, 0xff, size);
#endif

	if (p == NULL)
	{
		_af_error(AF_BAD_MALLOC, "allocation of %d bytes failed", size);
		return NULL;
	}

	return p;
}

char *_af_strdup (char *s)
{
	char	*p = malloc(strlen(s) + 1);

	if (p)
		strcpy(p, s);

	return p;
}

void *_af_realloc (void *p, size_t size)
{
	if (size <= 0)
	{
		_af_error(AF_BAD_MALLOC, "bad memory allocation size request %d", size);
		return NULL;
	}

	p = realloc(p, size);

	if (p == NULL)
	{
		_af_error(AF_BAD_MALLOC, "allocation of %d bytes failed", size);
		return NULL;
	}

	return p;
}

void *_af_calloc (size_t nmemb, size_t size)
{
	void	*p;

	if (nmemb <= 0 || size <= 0)
	{
		_af_error(AF_BAD_MALLOC, "bad memory allocation size request "
			"%d elements of %d bytes each", nmemb, size);
		return NULL;
	}

	p = calloc(nmemb, size);

	if (p == NULL)
	{
		_af_error(AF_BAD_MALLOC, "allocation of %d bytes failed",
			nmemb*size);
		return NULL;
	}

	return p;
}

AUpvlist _af_pv_long (long val)
{
	AUpvlist	ret = AUpvnew(1);
	AUpvsetparam(ret, 0, 0);
	AUpvsetvaltype(ret, 0, AU_PVTYPE_LONG);
	AUpvsetval(ret, 0, &val);
	return ret;
}

AUpvlist _af_pv_double (double val)
{
	AUpvlist	ret = AUpvnew(1);
	AUpvsetparam(ret, 0, 0);
	AUpvsetvaltype(ret, 0, AU_PVTYPE_DOUBLE);
	AUpvsetval(ret, 0, &val);
	return ret;
}

AUpvlist _af_pv_pointer (void *val)
{
	AUpvlist	ret = AUpvnew(1);
	AUpvsetparam(ret, 0, 0);
	AUpvsetvaltype(ret, 0, AU_PVTYPE_PTR);
	AUpvsetval(ret, 0, &val);
	return ret;
}

bool _af_pv_getlong (AUpvlist pvlist, int param, long *l)
{
	int	i;

	for (i=0; i<AUpvgetmaxitems(pvlist); i++)
	{
		int	p, t;

		AUpvgetparam(pvlist, i, &p);

		if (p != param)
			continue;

		AUpvgetvaltype(pvlist, i, &t);

		/* Ensure that this parameter is of type AU_PVTYPE_LONG. */
		if (t != AU_PVTYPE_LONG)
			return AF_FALSE;

		AUpvgetval(pvlist, i, l);
		return AF_TRUE;
	}

	return AF_FALSE;
}

bool _af_pv_getdouble (AUpvlist pvlist, int param, double *d)
{
	int	i;

	for (i=0; i<AUpvgetmaxitems(pvlist); i++)
	{
		int	p, t;

		AUpvgetparam(pvlist, i, &p);

		if (p != param)
			continue;

		AUpvgetvaltype(pvlist, i, &t);

		/* Ensure that this parameter is of type AU_PVTYPE_DOUBLE. */
		if (t != AU_PVTYPE_DOUBLE)
			return AF_FALSE;

		AUpvgetval(pvlist, i, d);
		return AF_TRUE;
	}

	return AF_FALSE;
}

bool _af_pv_getptr (AUpvlist pvlist, int param, void **v)
{
	int	i;

	for (i=0; i<AUpvgetmaxitems(pvlist); i++)
	{
		int	p, t;

		AUpvgetparam(pvlist, i, &p);

		if (p != param)
			continue;

		AUpvgetvaltype(pvlist, i, &t);

		/* Ensure that this parameter is of type AU_PVTYPE_PTR. */
		if (t != AU_PVTYPE_PTR)
			return AF_FALSE;

		AUpvgetval(pvlist, i, v);
		return AF_TRUE;
	}

	return AF_FALSE;
}

_TrackSetup *_af_filesetup_get_tracksetup (AFfilesetup setup, int trackid)
{
	int	i;
	for (i=0; i<setup->trackCount; i++)
	{
		if (setup->tracks[i].id == trackid)
			return &setup->tracks[i];
	}

	_af_error(AF_BAD_TRACKID, "bad track id %d", trackid);

	return NULL;
}

_Track *_af_filehandle_get_track (AFfilehandle file, int trackid)
{
	int	i;
	for (i=0; i<file->trackCount; i++)
	{
		if (file->tracks[i].id == trackid)
			return &file->tracks[i];
	}

	_af_error(AF_BAD_TRACKID, "bad track id %d", trackid);

	return NULL;
}

int _af_format_sample_size_uncompressed (_AudioFormat *format, bool stretch3to4)
{
	int	size = 0;

	switch (format->sampleFormat)
	{
		case AF_SAMPFMT_FLOAT:
			size = sizeof (float);
			break;
		case AF_SAMPFMT_DOUBLE:
			size = sizeof (double);
			break;
		default:
			size = (int) (format->sampleWidth + 7) / 8;
			if (format->compressionType == AF_COMPRESSION_NONE &&
				size == 3 && stretch3to4)
				size = 4;
			break;
	}

	return size;
}

float _af_format_sample_size (_AudioFormat *fmt, bool stretch3to4)
{
	int	compressionIndex;
	float	squishFactor;

	compressionIndex = _af_compression_index_from_id(fmt->compressionType);
	squishFactor = _af_compression[compressionIndex].squishFactor;

	return _af_format_sample_size_uncompressed(fmt, stretch3to4) /
		squishFactor;
}

int _af_format_frame_size_uncompressed (_AudioFormat *fmt, bool stretch3to4)
{
	return _af_format_sample_size_uncompressed(fmt, stretch3to4) *
		fmt->channelCount;
}

float _af_format_frame_size (_AudioFormat *fmt, bool stretch3to4)
{
	int	compressionIndex;
	float	squishFactor;

	compressionIndex = _af_compression_index_from_id(fmt->compressionType);
	squishFactor = _af_compression[compressionIndex].squishFactor;

	return _af_format_frame_size_uncompressed(fmt, stretch3to4) /
		squishFactor;
}

/*
	Set the sampleFormat and sampleWidth fields in f, and set the
	PCM info to the appropriate default values for the given sample
	format and sample width.
*/
status _af_set_sample_format (_AudioFormat *f, int sampleFormat, int sampleWidth)
{
	switch (sampleFormat)
	{
		case AF_SAMPFMT_UNSIGNED:
		case AF_SAMPFMT_TWOSCOMP:
		if (sampleWidth < 1 || sampleWidth > 32)
		{
			_af_error(AF_BAD_SAMPFMT,
				"illegal sample width %d for integer data",
				sampleWidth);
			return AF_FAIL;
		}
		else
		{
			int bytes;

			f->sampleFormat = sampleFormat;
			f->sampleWidth = sampleWidth;

			bytes = _af_format_sample_size_uncompressed(f, AF_FALSE);

			if (sampleFormat == AF_SAMPFMT_TWOSCOMP)
				f->pcm = _af_default_signed_integer_pcm_mappings[bytes];
			else
				f->pcm = _af_default_unsigned_integer_pcm_mappings[bytes];
		}
		break;

		case AF_SAMPFMT_FLOAT:
			f->sampleFormat = sampleFormat;
			f->sampleWidth = 32;
			f->pcm = _af_default_float_pcm_mapping;
			break;
		case AF_SAMPFMT_DOUBLE:
			f->sampleFormat = sampleFormat;
			f->sampleWidth = 64;      /*for convenience */
			f->pcm = _af_default_double_pcm_mapping;
			break;
		default:
			_af_error(AF_BAD_SAMPFMT, "unknown sample format %d",
				sampleFormat);
			return AF_FAIL;
	}

	return AF_SUCCEED;
}

/*
	Verify the uniqueness of the nids ids given.

	idname is the name of what the ids identify, as in "loop"
	iderr is an error as in AF_BAD_LOOPID
*/
bool _af_unique_ids (int *ids, int nids, char *idname, int iderr)
{
	int i;

	for (i = 0; i < nids; i++)
	{
		int j;
		for (j = 0; j < i; j++)
			if (ids[i] == ids[j])
			{
				_af_error(iderr, "nonunique %s id %d",
					idname, ids[i]);
				return AF_FALSE;
			}
	}

	return AF_TRUE;
}

status af_read_uint32_be (u_int32_t *value, AFvirtualfile *vf)
{
	u_int32_t	v;

	if (af_fread(&v, sizeof (v), 1, vf) != 1)
		return AF_FAIL;
	*value = BENDIAN_TO_HOST_INT32(v);
	return AF_SUCCEED;
}

status af_read_uint32_le (u_int32_t *value, AFvirtualfile *vf)
{
	u_int32_t	v;

	if (af_fread(&v, sizeof (v), 1, vf) != 1)
		return AF_FAIL;
	*value = LENDIAN_TO_HOST_INT32(v);
	return AF_SUCCEED;
}

status af_read_uint16_be (u_int16_t *value, AFvirtualfile *vf)
{
	u_int16_t	v;

	if (af_fread(&v, sizeof (v), 1, vf) != 1)
		return AF_FAIL;
	*value = BENDIAN_TO_HOST_INT16(v);
	return AF_SUCCEED;
}

status af_read_uint16_le (u_int16_t *value, AFvirtualfile *vf)
{
	u_int16_t	v;

	if (af_fread(&v, sizeof (v), 1, vf) != 1)
		return AF_FAIL;
	*value = LENDIAN_TO_HOST_INT16(v);
	return AF_SUCCEED;
}

status af_write_uint32_be (const u_int32_t *value, AFvirtualfile *vf)
{
	u_int32_t	v;
	v = HOST_TO_BENDIAN_INT32(*value);
	if (af_fwrite(&v, sizeof (v), 1, vf) != 1)
		return AF_FAIL;
	return AF_SUCCEED;
}

status af_write_uint32_le (const u_int32_t *value, AFvirtualfile *vf)
{
	u_int32_t	v;
	v = HOST_TO_LENDIAN_INT32(*value);
	if (af_fwrite(&v, sizeof (v), 1, vf) != 1)
		return AF_FAIL;
	return AF_SUCCEED;
}

status af_write_uint16_be (const u_int16_t *value, AFvirtualfile *vf)
{
	u_int16_t	v;
	v = HOST_TO_BENDIAN_INT16(*value);
	if (af_fwrite(&v, sizeof (v), 1, vf) != 1)
		return AF_FAIL;
	return AF_SUCCEED;
}

status af_write_uint16_le (const u_int16_t *value, AFvirtualfile *vf)
{
	u_int16_t	v;
	v = HOST_TO_LENDIAN_INT16(*value);
	if (af_fwrite(&v, sizeof (v), 1, vf) != 1)
		return AF_FAIL;
	return AF_SUCCEED;
}
