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
	setup.c
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <audiofile.h>

#include "afinternal.h"
#include "pcm.h"
#include "util.h"
#include "units.h"
#include "marker.h"

extern _Unit _af_units[];

_AFfilesetup _af_default_file_setup =
{
	_AF_VALID_FILESETUP,	/* valid */
#if WORDS_BIGENDIAN
	AF_FILE_AIFFC,	/* file format */
#else
	AF_FILE_WAVE,	/* file format */
#endif
	AF_FALSE,	/* trackSet */
	AF_FALSE,	/* instrumentSet */
	AF_FALSE,	/* miscellaneousSet */
	1,		/* trackCount */
	NULL,		/* tracks */
	1,		/* instrumentCount */
	NULL,		/* instruments */
	0,		/* miscellaneousCount */
	NULL		/* miscellaneous */
};

_InstrumentSetup _af_default_instrumentsetup =
{
	0,		/* id */
	2,		/* loopCount */
	NULL,		/* loops */
	AF_FALSE	/* loopSet */
};

_TrackSetup _af_default_tracksetup =
{
	0,
	{
		44100.0,
		AF_SAMPFMT_TWOSCOMP,
		16,
#if WORDS_BIGENDIAN
		AF_BYTEORDER_BIGENDIAN,
#else
		AF_BYTEORDER_LITTLEENDIAN,
#endif
		{ SLOPE_INT16, 0, MIN_INT16, MAX_INT16 },
		2,
		AF_COMPRESSION_NONE,
		NULL
	},
	AF_FALSE,	/* rateSet */
	AF_FALSE,	/* sampleFormatSet */
	AF_FALSE,	/* sampleWidthSet */
	AF_FALSE,	/* byteOrderSet */
	AF_FALSE,	/* channelCountSet */
	AF_FALSE,	/* compressionSet */
	AF_FALSE,	/* aesDataSet */
	AF_FALSE,	/* markersSet */
	AF_FALSE,	/* dataOffsetSet */
	AF_FALSE,	/* frameCountSet */

	4,		/* markerCount */
	NULL,		/* markers */
	0,		/* dataOffset */
	0		/* frameCount */
};

_TrackSetup *_af_tracksetup_new (int trackCount)
{
	int		i;
	_TrackSetup	*tracks;

	if (trackCount == 0)
		return NULL;

	tracks = _af_calloc(trackCount, sizeof (_TrackSetup));
	if (tracks == NULL)
		return NULL;

	for (i=0; i<trackCount; i++)
	{
		tracks[i] = _af_default_tracksetup;

		tracks[i].id = AF_DEFAULT_TRACK + i;

		/* XXXmpruett deal with compression */

		_af_set_sample_format(&tracks[i].f, tracks[i].f.sampleFormat,
			tracks[i].f.sampleWidth);

		if (tracks[i].markerCount == 0)
			tracks[i].markers = NULL;
		else
		{
			int	j;
			tracks[i].markers = _af_calloc(tracks[i].markerCount,
				sizeof (_MarkerSetup));

			if (tracks[i].markers == NULL)
				return NULL;

			for (j=0; j<tracks[i].markerCount; j++)
			{
				tracks[i].markers[j].id = j+1;

				tracks[i].markers[j].name = _af_strdup("");
				if (tracks[i].markers[j].name == NULL)
					return NULL;

				tracks[i].markers[j].comment = _af_strdup("");
				if (tracks[i].markers[j].comment == NULL)
					return NULL;
			}
		}
	}

	return tracks;
}

_InstrumentSetup *_af_instsetup_new (int instrumentCount)
{
	int	i;

	_InstrumentSetup	*instruments;

	if (instrumentCount == 0)
		return NULL;
	instruments = _af_calloc(instrumentCount, sizeof (_InstrumentSetup));
	if (instruments == NULL)
		return NULL;

	for (i=0; i<instrumentCount; i++)
	{
		instruments[i] = _af_default_instrumentsetup;
		instruments[i].id = AF_DEFAULT_INST + i;
		if (instruments[i].loopCount == 0)
			instruments[i].loops = NULL;
		else
		{
			int	j;
			instruments[i].loops = _af_calloc(instruments[i].loopCount, sizeof (_LoopSetup));
			if (instruments[i].loops == NULL)
				return NULL;

			for (j=0; j<instruments[i].loopCount; j++)
				instruments[i].loops[j].id = j+1;
		}
	}

	return instruments;
}

AFfilesetup afNewFileSetup (void)
{
	AFfilesetup	setup;

	setup = _af_malloc(sizeof (_AFfilesetup));
	if (setup == NULL) return AF_NULL_FILESETUP;

	*setup = _af_default_file_setup;

	setup->tracks = _af_tracksetup_new(setup->trackCount);

	setup->instruments = _af_instsetup_new(setup->instrumentCount);

	if (setup->miscellaneousCount == 0)
		setup->miscellaneous = NULL;
	else
	{
		int	i;

		setup->miscellaneous = _af_calloc(setup->miscellaneousCount,
			sizeof (_MiscellaneousSetup));
		for (i=0; i<setup->miscellaneousCount; i++)
		{
			setup->miscellaneous[i].id = i+1;
			setup->miscellaneous[i].type = 0;
			setup->miscellaneous[i].size = 0;
		}
	}

	return setup;
}

/*
	Free the specified track's markers and their subfields.
*/
void _af_setup_free_markers (AFfilesetup setup, int trackno)
{
	if (setup->tracks[trackno].markerCount != 0)
	{
		int	i;
		for (i=0; i<setup->tracks[trackno].markerCount; i++)
		{
			free(setup->tracks[trackno].markers[i].name);
			free(setup->tracks[trackno].markers[i].comment);
		}

		free(setup->tracks[trackno].markers);
	}

	setup->tracks[trackno].markers = NULL;
	setup->tracks[trackno].markerCount = 0;
}

/*
	Free the specified setup's tracks and their subfields.
*/
void _af_setup_free_tracks (AFfilesetup setup)
{
	int	i;

	if (setup->tracks)
	{
		for (i=0; i<setup->trackCount; i++)
		{
			_af_setup_free_markers(setup, i);
		}

		free(setup->tracks);
	}

	setup->tracks = NULL;
	setup->trackCount = 0;
}

/*
	Free the specified instrument's loops.
*/
void _af_setup_free_loops (AFfilesetup setup, int instno)
{
	if (setup->instruments[instno].loops)
	{
		free(setup->instruments[instno].loops);
	}

	setup->instruments[instno].loops = NULL;
	setup->instruments[instno].loopCount = 0;
}

/*
	Free the specified setup's instruments and their subfields.
*/
void _af_setup_free_instruments (AFfilesetup setup)
{
	int i;

	if (setup->instruments)
	{
		for (i = 0; i < setup->instrumentCount; i++)
			_af_setup_free_loops(setup, i);

		free(setup->instruments);
	}

	setup->instruments = NULL;
	setup->instrumentCount = 0;
}

void afFreeFileSetup (AFfilesetup setup)
{
	if (!_af_filesetup_ok(setup))
		return;

	_af_setup_free_tracks(setup);

	_af_setup_free_instruments(setup);

	if (setup->miscellaneousCount)
	{
		free(setup->miscellaneous);
		setup->miscellaneous = NULL;
		setup->miscellaneousCount = 0;
	}

	memset(setup, 0, sizeof (_AFfilesetup));
	free(setup);
}

void afInitFileFormat (AFfilesetup setup, int filefmt)
{
	if (!_af_filesetup_ok(setup))
		return;

	if (filefmt < 0 || filefmt > _AF_NUM_UNITS)
	{
		_af_error(AF_BAD_FILEFMT, "unrecognized file format %d",
			filefmt);
		return;
	}

	if (_af_units[filefmt].implemented == AF_FALSE)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED,
			"%s format not currently supported",
			_af_units[filefmt].name);
		return;
	}

	setup->fileFormat = filefmt;
}

void afInitChannels (AFfilesetup setup, int trackid, int channels)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	if (channels < 1)
	{
		_af_error(AF_BAD_CHANNELS, "invalid number of channels %d",
			channels);
		return;
	}

	track->f.channelCount = channels;
	track->channelCountSet = AF_TRUE;
}

void afInitSampleFormat (AFfilesetup setup, int trackid, int sampfmt, int sampwidth)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	_af_set_sample_format(&track->f, sampfmt, sampwidth);

	track->sampleFormatSet = AF_TRUE;
	track->sampleWidthSet = AF_TRUE;
}

void afInitByteOrder (AFfilesetup setup, int trackid, int byteorder)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	if (byteorder != AF_BYTEORDER_BIGENDIAN &&
		byteorder != AF_BYTEORDER_LITTLEENDIAN)
	{
		_af_error(AF_BAD_BYTEORDER, "invalid byte order %d", byteorder);
		return;
	}

	track->f.byteOrder = byteorder;
	track->byteOrderSet = AF_TRUE;
}

void afInitRate (AFfilesetup setup, int trackid, double rate)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	if (rate <= 0.0)
	{
		_af_error(AF_BAD_RATE, "invalid sample rate %.30g", rate);
		return;
	}

	track->f.sampleRate = rate;
	track->rateSet = AF_TRUE;
}

/*
	track data: data offset within the file (initialized for raw reading only)
*/
void afInitDataOffset (AFfilesetup setup, int trackid, AFfileoffset offset)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	if (offset < 0)
	{
		_af_error(AF_BAD_DATAOFFSET, "invalid data offset %d", offset);
		return;
	}

	track->dataOffset = offset;
	track->dataOffsetSet = AF_TRUE;
}

/*
	track data: data offset within the file (initialized for raw reading only)
*/
void afInitFrameCount (AFfilesetup setup, int trackid, AFfileoffset count)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	if (count < 0)
	{
		_af_error(AF_BAD_FRAMECNT, "invalid frame count %d", count);
		return;
	}

	track->frameCount = count;
	track->frameCountSet = AF_TRUE;
}

#define alloccopy(type, n, var, copyfrom) \
{ \
	if (n == 0) \
		var = NULL; \
	else \
	{ \
		if ((var = _af_calloc(n, sizeof (type))) == NULL) \
			goto fail; \
		memcpy((var), (copyfrom), (n) * sizeof (type)); \
	} \
}

AFfilesetup _af_filesetup_copy (AFfilesetup setup, AFfilesetup defaultSetup,
	bool copyMarks)
{
	AFfilesetup	newsetup;
	int		i;
	int		instrumentCount, miscellaneousCount, trackCount;

	newsetup = _af_malloc(sizeof (_AFfilesetup));
	if (newsetup == AF_NULL_FILESETUP)
		return AF_NULL_FILESETUP;

#ifdef DEBUG
	printf("default: trackset=%d instset=%d miscset=%d\n",
		defaultSetup->trackSet, defaultSetup->instrumentSet,
		defaultSetup->miscellaneousSet);
	printf("setup: trackset=%d instset=%d miscset=%d\n",
		setup->trackSet, setup->instrumentSet, setup->miscellaneousSet);
#endif

	*newsetup = *defaultSetup;

	newsetup->tracks = NULL;
	newsetup->instruments = NULL;
	newsetup->miscellaneous = NULL;

	/* Copy tracks. */
	trackCount = setup->trackSet ? setup->trackCount :
		newsetup->trackSet ? newsetup->trackCount : 0;
	alloccopy(_TrackSetup, trackCount, newsetup->tracks, setup->tracks);
	newsetup->trackCount = trackCount;

	/* Copy instruments. */
	instrumentCount = setup->instrumentSet ? setup->instrumentCount :
		newsetup->instrumentSet ? newsetup->instrumentCount : 0;
	alloccopy(_InstrumentSetup, instrumentCount, newsetup->instruments, setup->instruments);
	newsetup->instrumentCount = instrumentCount;

	/* Copy miscellaneous information. */
	miscellaneousCount = setup->miscellaneousSet ? setup->miscellaneousCount :
		newsetup->miscellaneousSet ? newsetup->miscellaneousCount : 0;
	alloccopy(_MiscellaneousSetup, miscellaneousCount, newsetup->miscellaneous, setup->miscellaneous);
	newsetup->miscellaneousCount = miscellaneousCount;

	for (i=0; i<setup->trackCount; i++)
	{
		int	j;
		_TrackSetup	*track = &newsetup->tracks[i];

		/* XXXmpruett set compression information */

		if (setup->tracks[i].markersSet == AF_FALSE &&
			copyMarks == AF_FALSE)
		{
			track->markers = NULL;
			track->markerCount = 0;
			continue;
		}

		alloccopy(_MarkerSetup, setup->tracks[i].markerCount,
			track->markers, setup->tracks[i].markers);
		track->markerCount = setup->tracks[i].markerCount;

		for (j=0; j<setup->tracks[i].markerCount; j++)
		{
			track->markers[j].name =
				_af_strdup(setup->tracks[i].markers[j].name);
			if (track->markers[j].name == NULL)
				goto fail;

			track->markers[j].comment =
				_af_strdup(setup->tracks[i].markers[j].comment);
			if (track->markers[j].comment == NULL)
				goto fail;
		}
	}

	for (i=0; i<newsetup->instrumentCount; i++)
	{
		_InstrumentSetup	*instrument = &newsetup->instruments[i];
		alloccopy(_LoopSetup, setup->instruments[i].loopCount,
			instrument->loops, setup->instruments[i].loops);
	}

	return newsetup;

	fail:
		if (newsetup->miscellaneous)
			free(newsetup->miscellaneous);
		if (newsetup->instruments)
			free(newsetup->instruments);
		if (newsetup->tracks)
			free(newsetup->tracks);
		if (newsetup)
			free(newsetup);

	return AF_NULL_FILESETUP;
}

/*
	Do all the non-file-format dependent things necessary to "convert"
	a filesetup into a filehandle.

	This function assumes that the track count, instrument count,
	etc. of given filesetup is okay for the file format.

	Called from write.init and raw read.init unit functions.

	This function does NOT SET ALL THE FIELDS of the filesetup.
	You must be careful when writing a unit to set the fields
	you are supposed to (described below).

	These fields are not set here, so are somebody else's problem:
	- handle: valid, fd, access, filename, fileFormat, seekok (set in afOpenFile)
	- handle: formatSpecific (UNIT MUST SET! (set to NULL if no data))

	- track: virtual format v, modulesdirty, nmodules, module, chunk,
	(buffers), totalvframes, nextvframe, channelmatrix, frames2ignore
	(these are handled by _AFinitmodules and _AFsetupmodules)

	- track: totalfframes, nextfframe, fpos_first_frame,
	fpos_next_frame, data_size (UNIT MUST SET!)

	- mark: fpos_position (UNIT MUST SET!)
*/

status _af_filesetup_make_handle (AFfilesetup setup, AFfilehandle handle)
{
	int	i;

	handle->valid = _AF_VALID_FILEHANDLE;

	if ((handle->trackCount = setup->trackCount) == 0)
		handle->tracks = NULL;
	else
	{
		handle->tracks = _af_calloc(handle->trackCount, sizeof (_Track));
		if (handle->tracks == NULL)
			return AF_FAIL;

		for (i=0; i<handle->trackCount; i++)
		{
			_Track		*track = &handle->tracks[i];
			_TrackSetup	*tracksetup = &setup->tracks[i];

			track->id = tracksetup->id;

			track->f = tracksetup->f;
			track->channelMatrix = NULL;

			/* XXXmpruett copy compression stuff too */

			if ((track->markerCount = tracksetup->markerCount) == 0)
				track->markers = NULL;
			else
			{
				int	j;

				track->markers = _af_marker_new(track->markerCount);
				if (track->markers == NULL)
					return AF_FAIL;
				for (j=0; j<track->markerCount; j++)
				{
					track->markers[j].id = tracksetup->markers[j].id;
					track->markers[j].name =
						_af_strdup(tracksetup->markers[j].name);
					if (track->markers[j].name == NULL)
						return AF_FAIL;

					track->markers[j].comment =
						_af_strdup(tracksetup->markers[j].comment);
					if (track->markers[j].comment == NULL)
						return AF_FAIL;
					track->markers[j].position = 0;
				}
			}

			track->hasAESData = tracksetup->aesDataSet;

			track->ms.modulesdirty = AF_TRUE;
			track->ms.nmodules = 0;
			track->ms.chunk = NULL;
			track->ms.module = NULL;
			track->ms.buffer = NULL;

			track->ms.filemodinst.valid = AF_FALSE;
			track->ms.filemod_rebufferinst.valid = AF_FALSE;
			track->ms.rateconvertinst.valid = AF_FALSE;
			track->ms.rateconvert_rebufferinst.valid = AF_FALSE;
		}
	}

	/* Copy instrument data. */
	if ((handle->instrumentCount = setup->instrumentCount) == 0)
		handle->instruments = NULL;
	else
	{
		handle->instruments = _af_calloc(handle->instrumentCount,
			sizeof (_Instrument));
		if (handle->instruments == NULL)
			return AF_FAIL;

		for (i=0; i<handle->instrumentCount; i++)
		{
			int	instParamCount;

			handle->instruments[i].id = setup->instruments[i].id;

			/* Copy loops. */
			if ((handle->instruments[i].loopCount =
				setup->instruments[i].loopCount) == 0)
				handle->instruments[i].loops = NULL;
			else
			{
				int	j;

				handle->instruments[i].loops = _af_calloc(handle->instruments[i].loopCount, sizeof (_Loop));
				if (handle->instruments[i].loops == NULL)
					return AF_FAIL;
				for (j=0; j<handle->instruments[i].loopCount; j++)
				{
					_Loop	*loop;
					loop = &handle->instruments[i].loops[j];
					loop->id = setup->instruments[i].loops[j].id;
					loop->mode = AF_LOOP_MODE_NOLOOP;
					loop->count = 0;
					loop->trackid = AF_DEFAULT_TRACK;
					loop->beginMarker = 1 + (2*j);
					loop->endMarker = 2 + (2*j);
				}
			}

			/* Copy instrument parameters. */
			if ((instParamCount = _af_units[setup->fileFormat].instrumentParameterCount) == 0)
				handle->instruments[i].values = NULL;
			else
			{
				int	j;
				handle->instruments[i].values = _af_calloc(instParamCount, sizeof (AFPVu));
				if (handle->instruments[i].values == NULL)
					return AF_FAIL;
				for (j=0; j<instParamCount; j++)
				{
					handle->instruments[i].values[j] =
						_af_units[setup->fileFormat].instrumentParameters[j].defaultValue;
				}
			}
		}
	}

	/* Copy miscellaneous information. */

	if ((handle->miscellaneousCount = setup->miscellaneousCount) == 0)
		handle->miscellaneous = NULL;
	else
	{
		handle->miscellaneous = _af_calloc(handle->miscellaneousCount,
			sizeof (_Miscellaneous));
		if (handle->miscellaneous == NULL)
			return AF_FAIL;
		for (i=0; i<handle->miscellaneousCount; i++)
		{
			handle->miscellaneous[i].id = setup->miscellaneous[i].id;
			handle->miscellaneous[i].type = setup->miscellaneous[i].type;
			handle->miscellaneous[i].size = setup->miscellaneous[i].size;
			handle->miscellaneous[i].position = 0;
			handle->miscellaneous[i].buffer = NULL;
		}
	}

	return AF_SUCCEED;
}
