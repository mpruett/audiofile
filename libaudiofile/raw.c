/*
	Audio File Library
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
	raw.c
*/

#include "afinternal.h"
#include "audiofile.h"
#include "raw.h"
#include "util.h"
#include "setup.h"

_AFfilesetup _af_raw_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_RAWDATA,	/* fileFormat */
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

int _af_raw_compression_types[_AF_RAW_NUM_COMPTYPES] =
{
	AF_COMPRESSION_G711_ULAW,
	AF_COMPRESSION_G711_ALAW
};

bool _af_raw_recognize (AFvirtualfile *fh)
{
	return AF_FALSE;
}

status _af_raw_read_init (AFfilesetup filesetup, AFfilehandle filehandle)
{
	_Track	*track;

	if (filesetup == NULL)
	{
		_af_error(AF_BAD_FILEHANDLE, "a valid AFfilesetup is required for reading raw data");
		return AF_FAIL;
	}

	if (_af_filesetup_make_handle(filesetup, filehandle) == AF_FAIL)
		return AF_FAIL;

	track = &filehandle->tracks[0];

	/* Set the track's data offset. */
	if (filesetup->tracks[0].dataOffsetSet)
		track->fpos_first_frame = filesetup->tracks[0].dataOffset;
	else
		track->fpos_first_frame = 0;

	/* Set the track's frame count. */
	if (filesetup->tracks[0].frameCountSet)
	{
		track->totalfframes = filesetup->tracks[0].frameCount;
	}
	else
	{
		off_t	filesize;
		filesize = af_flength(filehandle->fh);
		if (filesize == -1)
			track->totalfframes = -1;
		else
			track->totalfframes = filesize / _af_format_frame_size(&track->f, AF_FALSE);
		track->data_size = filesize;
	}

	return AF_SUCCEED;
}

status _af_raw_write_init (AFfilesetup filesetup, AFfilehandle filehandle)
{
	_Track	*track;

	if (_af_filesetup_make_handle(filesetup, filehandle) == AF_FAIL)
		return AF_FAIL;

	track = &filehandle->tracks[0];
	track->totalfframes = 0;
	if (filesetup->tracks[0].dataOffsetSet)
		track->fpos_first_frame = filesetup->tracks[0].dataOffset;
	else
		track->fpos_first_frame = 0;

	return AF_SUCCEED;
}

status _af_raw_update (AFfilehandle filehandle)
{
	return AF_SUCCEED;
}

AFfilesetup _af_raw_complete_setup (AFfilesetup setup)
{
	AFfilesetup	newSetup;
	_TrackSetup	*track;

	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_FILESETUP, "raw file must have exactly one track");
		return AF_NULL_FILESETUP;
	}

	if ((track = _af_filesetup_get_tracksetup(setup, AF_DEFAULT_TRACK)) == NULL)
	{
		_af_error(AF_BAD_FILESETUP, "could not access track in file setup");
		return AF_NULL_FILESETUP;
	}

	if (track->aesDataSet)
	{
		_af_error(AF_BAD_FILESETUP, "raw file cannot have AES data");
		return AF_NULL_FILESETUP;
	}

	if (track->markersSet && track->markerCount != 0)
	{
		_af_error(AF_BAD_NUMMARKS, "raw file cannot have markers");
		return AF_NULL_FILESETUP;
	}

	if (setup->instrumentSet && setup->instrumentCount != 0)
	{
		_af_error(AF_BAD_NUMINSTS, "raw file cannot have instruments");
		return AF_NULL_FILESETUP;
	}

	if (setup->miscellaneousSet && setup->miscellaneousCount != 0)
	{
		_af_error(AF_BAD_NUMMISC, "raw file cannot have miscellaneous data");
		return AF_NULL_FILESETUP;
	}

	newSetup = _af_malloc(sizeof (_AFfilesetup));
	*newSetup = _af_raw_default_filesetup;

	newSetup->tracks = _af_malloc(sizeof (_TrackSetup));
	newSetup->tracks[0] = setup->tracks[0];
	newSetup->tracks[0].f.compressionParams = NULL;

	newSetup->tracks[0].markerCount = 0;
	newSetup->tracks[0].markers = NULL;

	return newSetup;
}
