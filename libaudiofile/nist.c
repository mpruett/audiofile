/*
	Audio File Library
	Copyright (C) 2004, Michael Pruett <michael@68k.org>

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
	nist.c

	This file contains code for reading NIST SPHERE files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "afinternal.h"
#include "audiofile.h"
#include "util.h"
#include "byteorder.h"
#include "setup.h"
#include "track.h"

#include "nist.h"

_AFfilesetup _af_nist_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_NIST_SPHERE,	/* fileFormat */
	AF_TRUE,		/* trackSet */
	AF_TRUE,		/* instrumentSet */
	AF_TRUE,		/* miscellaneousSet */
	1,			/* trackCount */
	NULL,			/* tracks */
	0,			/* instrumentCount */
	NULL,			/* instruments */
	0,			/* miscellaneousCount */
	NULL			/* miscellaneous */
};

bool _af_nist_recognize (AFvirtualfile *fh)
{
	u_int8_t	buffer[16];

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(buffer, 16, 1, fh) != 1)
		return AF_FALSE;

	/* Check to see if the file's magic number matches. */
	if (memcmp(buffer, "NIST_1A\n   1024\n", 16) == 0)
		return AF_TRUE;

	return AF_FALSE;
}

AFfilesetup _af_nist_complete_setup (AFfilesetup setup)
{
	_TrackSetup	*track;

	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "NIST SPHERE file must have 1 track");
		return AF_NULL_FILESETUP;
	}

	track = &setup->tracks[0];

	if (track->sampleFormatSet)
	{
		/* XXXmpruett: Currently we allow only 1-16 bit sample width. */
		if (track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP &&
			(track->f.sampleWidth < 1 || track->f.sampleWidth > 16))
		{
			_af_error(AF_BAD_WIDTH,
				"invalid sample width %d bits for NIST SPHERE format");
			return AF_NULL_FILESETUP;
		}
		else if (track->f.sampleFormat == AF_SAMPFMT_UNSIGNED)
		{
			_af_error(AF_BAD_SAMPFMT,
				"NIST SPHERE format does not support unsigned data");
			return AF_NULL_FILESETUP;
		}
		else if (track->f.sampleFormat == AF_SAMPFMT_FLOAT ||
			track->f.sampleFormat == AF_SAMPFMT_DOUBLE)
		{
			_af_error(AF_BAD_SAMPFMT,
				"NIST SPHERE format does not support floating-point data");
			return AF_NULL_FILESETUP;
		}
	}

	if (track->rateSet && track->f.sampleRate <= 0.0)
	{
		_af_error(AF_BAD_RATE,
			"invalid sample rate %.30g for NIST SPHERE file",
			track->f.sampleRate);
		return AF_NULL_FILESETUP;
	}

	if (track->channelCountSet && track->f.channelCount < 1)
	{
		_af_error(AF_BAD_CHANNELS,
			"invalid channel count (%d) for NIST SPHERE format",
			track->f.channelCount);
		return AF_NULL_FILESETUP;
	}

	if (track->compressionSet && track->f.compressionType != AF_COMPRESSION_NONE &&
		track->f.compressionType != AF_COMPRESSION_G711_ULAW &&
		track->f.compressionType != AF_COMPRESSION_G711_ALAW)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED,
			"NIST SPHERE format supports only G.711 u-law or A-law compression");
		return AF_NULL_FILESETUP;
	}

	if (track->aesDataSet)
	{
		_af_error(AF_BAD_FILESETUP, "NIST SPHERE file cannot have AES data");
		return AF_NULL_FILESETUP;
	}

	if (track->markersSet && track->markerCount != 0)
	{
		_af_error(AF_BAD_NUMMARKS, "NIST SPHERE format does not support markers");
		return AF_NULL_FILESETUP;
	}

	if (setup->instrumentSet && setup->instrumentCount != 0)
	{
		_af_error(AF_BAD_NUMINSTS, "NIST SPHERE format does not support instruments");
		return AF_NULL_FILESETUP;
	}

	/* XXXmpruett: We don't support miscellaneous chunks for now. */
	if (setup->miscellaneousSet && setup->miscellaneousCount != 0)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED, "NIST SPHERE format does not currently support miscellaneous chunks");
		return AF_NULL_FILESETUP;
	}

	return _af_filesetup_copy(setup, &_af_nist_default_filesetup, AF_TRUE);
}

static bool nist_header_read_int (char *header, char *key, int *val)
{
	char	*cp;
	char	keystring[256], scanstring[256];

	snprintf(keystring, 256, "\n%s -i", key);

	if ((cp = strstr(header, keystring)) != NULL)
	{
		snprintf(scanstring, 256, "\n%s -i %%d", key);
		sscanf(cp, scanstring, val);
		return AF_TRUE;
	}

	return AF_FALSE;
}

static bool nist_header_read_string (char *header, char *key, int *length, char *val)
{
	char	*cp;
	char	keystring[256], scanstring[256];

	snprintf(keystring, 256, "\n%s -s", key);

	if ((cp = strstr(header, keystring)) != NULL)
	{
		snprintf(scanstring, 256, "\n%s -s%%d %%79s", key);
		sscanf(cp, scanstring, length, val);
		return AF_TRUE;
	}

	return AF_FALSE;
}

status _af_nist_read_init (AFfilesetup setup, AFfilehandle handle)
{
	_Track		*track;
	char		header[NIST_SPHERE_HEADER_LENGTH + 1];
	int		intval;
	char		strval[NIST_SPHERE_MAX_FIELD_LENGTH];
	int		sample_n_bytes;

	handle->instruments = NULL;
	handle->instrumentCount = 0 ;
	handle->miscellaneous = NULL;
	handle->miscellaneousCount = 0;

	handle->tracks = NULL;
	handle->trackCount = 1;

	af_fseek(handle->fh, 0, SEEK_SET);

	if (af_fread(header, NIST_SPHERE_HEADER_LENGTH, 1, handle->fh) != 1)
	{
		_af_error(AF_BAD_READ, "Could not read NIST SPHERE file header");
		return AF_FAIL;
	}

	header[NIST_SPHERE_HEADER_LENGTH] = '\0';

	if (memcmp(header, "NIST_1A\n   1024\n", 16) != 0)
	{
		_af_error(AF_BAD_FILEFMT, "Bad NIST SPHERE file header");
		return AF_FAIL;
	}

	if ((handle->tracks = _af_track_new()) == NULL)
		return AF_FAIL;
	track = &handle->tracks[0];

	/* Read number of bytes per sample. */
	if (!nist_header_read_int(header, "sample_n_bytes", &sample_n_bytes))
	{
		_af_error(AF_BAD_HEADER, "bytes per sample not specified");
		return AF_FAIL;
	}

	/*
		Since some older NIST SPHERE files lack a sample_coding
		field, if sample_n_bytes is 1, assume mu-law;
		otherwise assume linear PCM.
	*/
	track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
	if (sample_n_bytes == 1)
	{
		track->f.compressionType = AF_COMPRESSION_G711_ULAW;
		track->f.sampleWidth = 16;
	}
	else
	{
		track->f.compressionType = AF_COMPRESSION_NONE;
		track->f.sampleWidth = sample_n_bytes * 8;
	}

	if (nist_header_read_string(header, "sample_coding", &intval, strval))
	{
		if (strcmp(strval, "pcm") == 0)
			;
		else if (strcmp(strval, "ulaw") == 0 || strcmp(strval, "mu-law") == 0)
		{
			track->f.compressionType = AF_COMPRESSION_G711_ULAW;
			track->f.sampleWidth = 16;
		}
		else if (strcmp(strval, "alaw") == 0)
		{
			track->f.compressionType = AF_COMPRESSION_G711_ALAW;
			track->f.sampleWidth = 16;
		}
		else
		{
			_af_error(AF_BAD_SAMPFMT,
				"unrecognized NIST SPHERE sample format %s", strval);
			return AF_FAIL;
		}
	}

	/* Read channel count. */
	if (nist_header_read_int(header, "channel_count", &intval))
	{
		if (intval < 1)
		{
			_af_error(AF_BAD_CHANNELS, "invalid number of channels %d",
				intval);
			return AF_FAIL;
		}
		track->f.channelCount = intval;
	}
	else
	{
		_af_error(AF_BAD_HEADER, "number of channels not specified");
		return AF_FAIL;
	}

	/* Read string representing byte order. */
	if (nist_header_read_string(header, "sample_byte_format", &intval, strval))
	{
		if (intval > 1)
		{
			if (strncmp(strval, "01", 2) == 0)
				track->f.byteOrder = AF_BYTEORDER_LITTLEENDIAN;
			else
				track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;
		}
		else
			track->f.byteOrder = _AF_BYTEORDER_NATIVE;
	}
	else
	{
		/*
			Fail if this field is not present and sample
			width is more than one byte.
		*/
		if (track->f.compressionType == AF_COMPRESSION_NONE &&
			track->f.sampleWidth > 8)
		{
			_af_error(AF_BAD_HEADER, "sample byte order not specified");
			return AF_FAIL;
		}
	}

	/* Read significant bits per sample. */
	if (nist_header_read_int(header, "sample_sig_bits", &intval))
	{
		if (intval < 1 || intval > 32)
		{
			_af_error(AF_BAD_WIDTH, "invalid sample width %d bits\n",
				intval);
			return AF_FAIL;
		}

		/*
			Use specified significant bits value as the
			sample width for uncompressed data as long
			as the number of bytes per sample remains
			unchanged.
		*/
		if (track->f.compressionType == AF_COMPRESSION_NONE &&
			(intval + 7) / 8 == sample_n_bytes)
		{
			track->f.sampleWidth = intval;
		}
	}

	/* Read sample rate. */
	if (nist_header_read_int(header, "sample_rate", &intval))
	{
		if (intval <= 0)
		{
			_af_error(AF_BAD_RATE, "invalid sample rate %d Hz\n", intval);
			return AF_FAIL;
		}
		track->f.sampleRate = intval;
	}
	else
	{
		_af_error(AF_BAD_HEADER, "sample rate not specified");
		return AF_FAIL;
	}

	/* Read sample count. */
	if (nist_header_read_int(header, "sample_count", &intval))
	{
		track->totalfframes = intval / track->f.channelCount;
	}
	else
	{
		_af_error(AF_BAD_HEADER, "number of samples not specified");
		return AF_FAIL;
	}

	if (_af_set_sample_format(&track->f, track->f.sampleFormat,
		track->f.sampleWidth) == AF_FAIL)
	{
		return AF_FAIL;
	}

	track->fpos_first_frame = NIST_SPHERE_HEADER_LENGTH;
	track->data_size = af_flength(handle->fh) - NIST_SPHERE_HEADER_LENGTH;
	track->nextfframe = 0;
	track->fpos_next_frame = track->fpos_first_frame;

	handle->formatSpecific = NULL;

	return AF_SUCCEED;
}
