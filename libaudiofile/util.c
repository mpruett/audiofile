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
#include <math.h>
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

#include "print.h"

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

void _af_printid (u_int32_t id)
{
	printf("%c%c%c%c",
		(id >> 24) & 0xff,
		(id >> 16) & 0xff,
		(id >> 8) & 0xff,
		id & 0xff);
}

void _af_print_pvlist (AUpvlist list)
{
	int i;

	assert(list);

	printf("list.valid: %d\n", list->valid);
	printf("list.count: %d\n", list->count);

	for (i=0; i<list->count; i++)
	{
		printf("item %d valid %d, should be %d\n",
			i, list->items[i].valid, _AU_VALID_PVITEM);

		switch (list->items[i].type)
		{
			case AU_PVTYPE_LONG:
				printf("item #%d, parameter %d, long: %ld\n",
					i, list->items[i].parameter,
					list->items[i].value.l);
				break;
			case AU_PVTYPE_DOUBLE:
				printf("item #%d, parameter %d, double: %f\n",
					i, list->items[i].parameter,
					list->items[i].value.d);
				break;
			case AU_PVTYPE_PTR:
				printf("item #%d, parameter %d, pointer: %p\n",
					i, list->items[i].parameter,
					list->items[i].value.v);
				break;

			default:
				printf("item #%d, invalid type %d\n", i,
					list->items[i].type);
				assert(0);
				break;
		}
	}
}

void _af_print_audioformat (_AudioFormat *fmt)
{
	/* sampleRate, channelCount */
	printf("{ %7.2f Hz %d ch ", fmt->sampleRate, fmt->channelCount);

	/* sampleFormat, sampleWidth */
	switch (fmt->sampleFormat)
	{
		case AF_SAMPFMT_TWOSCOMP:
			printf("%db 2 ", fmt->sampleWidth);
			break;
		case AF_SAMPFMT_UNSIGNED:
			printf("%db u ", fmt->sampleWidth);
			break;
		case AF_SAMPFMT_FLOAT:
			printf("flt ");
			break;
		case AF_SAMPFMT_DOUBLE:
			printf("dbl ");
			break;
		default:
			printf("%dsampfmt? ", fmt->sampleFormat);
	}

	/* pcm */
	printf("(%.30g+-%.30g [%.30g,%.30g]) ",
		fmt->pcm.intercept, fmt->pcm.slope,
		fmt->pcm.minClip, fmt->pcm.maxClip);

	/* byteOrder */
	switch (fmt->byteOrder)
	{
		case AF_BYTEORDER_BIGENDIAN:
			printf("big ");
			break;
		case AF_BYTEORDER_LITTLEENDIAN:
			printf("little ");
			break;
		default:
			printf("%dbyteorder? ", fmt->byteOrder);
			break;
	}

	/* compression */
	{
		int idx = _af_compression_index_from_id(fmt->compressionType);
		if (idx < 0)
		{
			printf("%dcompression?", fmt->compressionType);
		}
		else if (fmt->compressionType == AF_COMPRESSION_NONE)
			printf("pcm");
		else
			printf("%s", _af_compression[idx].label);
	}

	printf(" }");
}

void _af_print_tracks (AFfilehandle filehandle)
{
	int	i;
	for (i=0; i<filehandle->trackCount; i++)
	{
		_Track	*track = &filehandle->tracks[i];
		printf("track %d\n", i);
		printf(" id %d\n", track->id);
		printf(" sample format\n");
		_af_print_audioformat(&track->f);
		printf(" virtual format\n");
		_af_print_audioformat(&track->v);
		printf(" total file frames: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->totalfframes);
		printf(" total virtual frames: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->totalvframes);
		printf(" next file frame: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->nextfframe);
		printf(" next virtual frame: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->nextvframe);
		printf(" frames to ignore: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->frames2ignore);

		printf(" data_size: %" AF_FILEOFFSET_PRINT_FMT "\n",
			track->data_size);
		printf(" fpos_first_frame: %" AF_FILEOFFSET_PRINT_FMT "\n",
			track->fpos_first_frame);
		printf(" fpos_next_frame: %" AF_FILEOFFSET_PRINT_FMT "\n",
			track->fpos_next_frame);
		printf(" fpos_after_data: %" AF_FILEOFFSET_PRINT_FMT "\n",
			track->fpos_after_data);

		printf(" channel matrix:");
		_af_print_channel_matrix(track->channelMatrix,
			track->f.channelCount, track->v.channelCount);
		printf("\n");

		printf(" marker count: %d\n", track->markerCount);
	}
}

void _af_print_filehandle (AFfilehandle filehandle)
{
	printf("file handle: 0x%p\n", filehandle);

	if (filehandle->valid == _AF_VALID_FILEHANDLE)
		printf("valid\n");
	else
		printf("invalid!\n");

	printf(" access: ");
	if (filehandle->access == _AF_READ_ACCESS)
		putchar('r');
	else
		putchar('w');

	printf(" fileFormat: %d\n", filehandle->fileFormat);

	printf(" instrument count: %d\n", filehandle->instrumentCount);
	printf(" instruments: 0x%p\n", filehandle->instruments);

	printf(" miscellaneous count: %d\n", filehandle->miscellaneousCount);
	printf(" miscellaneous: 0x%p\n", filehandle->miscellaneous);

	printf(" trackCount: %d\n", filehandle->trackCount);
	printf(" tracks: 0x%p\n", filehandle->tracks);
	_af_print_tracks(filehandle);
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

void _af_print_channel_matrix (double *matrix, int fchans, int vchans)
{
	int v, f;

	if (!matrix)
	{
		printf("NULL");
		return;
	}

	printf("{");
	for (v=0; v < vchans; v++)
	{
		if (v) printf(" ");
		printf("{");
		for (f=0; f < fchans; f++)
		{
			if (f) printf(" ");
			printf("%5.2f", *(matrix + v*fchans + f));
		}
		printf("}");
	}
	printf("}");
}

void _af_print_frame (AFframecount frameno, double *frame, int nchannels,
	char *formatstring, int numberwidth,
	double slope, double intercept, double minclip, double maxclip)
{
	char linebuf[81];
	int wavewidth = wavewidth = 78 - numberwidth*nchannels - 6;
	int c;

	memset(linebuf, ' ', 80);
	linebuf[0] = '|';
	linebuf[wavewidth-1] = '|';
	linebuf[wavewidth] = 0;

	printf("%05" AF_FRAMECOUNT_PRINT_FMT " ", frameno);

	for (c=0; c < nchannels; c++)
	{
		double pcm = frame[c];
		printf(formatstring, pcm);
	}
	for (c=0; c < nchannels; c++)
	{
		double pcm = frame[c], volts;
		if (maxclip > minclip)
		{
			if (pcm < minclip) pcm = minclip;
			if (pcm > maxclip) pcm = maxclip;
		}
		volts = (pcm - intercept) / slope;
		linebuf[(int)((volts/2 + 0.5)*(wavewidth-3)) + 1] = '0' + c;
	}
	printf("%s\n", linebuf);
}

/*
	dumpchunk
*/
void _af_print_chunk (_AFchunk *chnk)
{
	_AudioFormat fmt = chnk->f;
	AFframecount nframes = chnk->nframes;
	AFframecount nsamps = nframes * fmt.channelCount;
	AFframecount fr;

	double *outbuf;
	char formatstring[20];
	int digits, numberwidth;

	switch (fmt.compressionType)
	{
		case AF_COMPRESSION_NONE:
			break;

		case AF_COMPRESSION_G711_ULAW:
			printf("WARNING dumping ulaw data as if it were 8-bit unsigned\n");
			fmt.compressionType = AF_COMPRESSION_NONE;
			fmt.sampleWidth = 8;
			fmt.sampleFormat = AF_SAMPFMT_UNSIGNED;
		break;

		default:
			printf("LAME-O chunk dumper cannot deal with '%s' compression\n",
				_af_compression[_af_compression_index_from_id(fmt.compressionType)].name);
			return;
	}

	if (fmt.sampleWidth > 8 && fmt.byteOrder != _AF_BYTEORDER_NATIVE)
	{
		printf("LAME-O chunk dumper cannot deal with non-native byte order\n");
		return;
	}

#define transfer(type) \
	{ \
	int s; \
	for(s=0; s < nsamps; s++) \
	outbuf[s] = (double)(((type *)chnk->buf)[s]); \
	}

	/* Make the buffer large enough to hold doubles. */
	outbuf = malloc(sizeof(double) * nsamps);

	switch (fmt.sampleFormat)
	{
		case AF_SAMPFMT_DOUBLE:
		case AF_SAMPFMT_FLOAT:
		{
			if (fmt.sampleFormat == AF_SAMPFMT_DOUBLE)
			{
				transfer(double);
			}
			else
			{
				transfer(float);
			}

			digits = (int) log10(fmt.pcm.intercept + fabs(fmt.pcm.slope)) + 1;
			/* Account for the sign character. */
			digits += 1;

			if (digits > 4)
			{
				sprintf(formatstring, "%%%d.0f ", digits);
				numberwidth = digits + 1;
			}
			else
			{
				sprintf(formatstring, "%%%d.2f ", digits+3);
				numberwidth = digits + 3 + 1;
			}
		}
		break;

		case AF_SAMPFMT_TWOSCOMP:
		case AF_SAMPFMT_UNSIGNED:
		{
			bool issigned = (fmt.sampleFormat==AF_SAMPFMT_TWOSCOMP);

			/* # of bytes taken by the value */
			int realbytes = _af_format_sample_size_uncompressed(&fmt, AF_TRUE);

			switch (realbytes)
			{
				case 1:
					if (issigned) { transfer(schar1); }
					else { transfer(uchar1); }
					break;
				case 2:
					if (issigned) { transfer(schar2); }
					else { transfer(uchar2); }
					break;
				case 4:
					if (issigned) { transfer(schar4); }
					else { transfer(uchar4); }
					break;
				default:
					printf("LAME-O chunk dumper cannot deal with %d bits\n",
						realbytes*8);
					free(outbuf);
					return;
			}

			digits = (int) log10(fmt.pcm.intercept + fabs(fmt.pcm.slope)) + 1;
			if (issigned)
				digits++;

			sprintf(formatstring, "%%%d.0f ", digits);
			numberwidth = digits + 1;
		}
		break;

		default:
			assert(0);
			return;
	}

	for (fr=0; fr < nframes; fr++)
		_af_print_frame(fr, &outbuf[fr*fmt.channelCount],
			fmt.channelCount, formatstring, numberwidth,
			fmt.pcm.slope, fmt.pcm.intercept,
			fmt.pcm.minClip, fmt.pcm.maxClip);

	free(outbuf);
}
