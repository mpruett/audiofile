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
	audiofile.c

	This file implements many of the main interface routines of the
	Audio File Library.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "audiofile.h"
#include "util.h"
#include "afinternal.h"
#include "afinternal.h"
#include "units.h"
#include "modules.h"

extern _Unit _af_units[];

AFfileoffset afGetDataOffset (AFfilehandle file, int trackid)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	return track->fpos_first_frame;
}

AFfileoffset afGetTrackBytes (AFfilehandle file, int trackid)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	track = _af_filehandle_get_track(file, trackid);

	return track->data_size;
}

/*
	afGetFrameSize returns the size (in bytes) of a sample frame from
	the specified track of an audio file.

	stretch3to4 == AF_TRUE: size which user sees
	stretch3to4 == AF_FALSE: size used in file
*/
float afGetFrameSize (AFfilehandle file, int trackid, int stretch3to4)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	return _af_format_frame_size(&track->f, stretch3to4);
}

float afGetVirtualFrameSize (AFfilehandle file, int trackid, int stretch3to4)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	return _af_format_frame_size(&track->v, stretch3to4);
}

AFframecount afSeekFrame (AFfilehandle file, int trackid, AFframecount frame)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if (!_af_filehandle_can_read(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (track->ms.modulesdirty)
		if (_AFsetupmodules(file, track) != AF_SUCCEED)
			return -1;

	if (frame < 0)
		return track->nextvframe;

	/* Optimize the case of seeking to the current position. */
	if (frame == track->nextvframe)
		return track->nextvframe;

	if (track->totalvframes != -1)
		if (frame > track->totalvframes)
			frame = track->totalvframes - 1;

	track->nextvframe = frame;

	if (_AFsetupmodules(file, track) != AF_SUCCEED)
		return -1;

	return track->nextvframe;
}

AFfileoffset afTellFrame (AFfilehandle file, int trackid)
{
	return afSeekFrame(file, trackid, -1);
}

int afSetVirtualByteOrder (AFfilehandle handle, int track, int byteorder)
{
	_Track *currentTrack;

	if (NULL == (currentTrack = _af_filehandle_get_track(handle, track)))
		return AF_FAIL;

	if (byteorder != AF_BYTEORDER_BIGENDIAN &&
		byteorder != AF_BYTEORDER_LITTLEENDIAN)
	{
		_af_error(AF_BAD_BYTEORDER, "invalid byte order %d", byteorder);
		return AF_FAIL;
	}

	currentTrack->v.byteOrder = byteorder;
	currentTrack->ms.modulesdirty = AF_TRUE;

	return AF_SUCCEED;
}

int afGetByteOrder (AFfilehandle handle, int track)
{
	_Track *currentTrack;

	if ((currentTrack = _af_filehandle_get_track(handle, track)) == NULL)
		return -1;

	return (currentTrack->f.byteOrder);
}

int afGetVirtualByteOrder (AFfilehandle handle, int track)
{
	_Track *currentTrack;

	if ((currentTrack = _af_filehandle_get_track(handle, track)) == NULL)
		return -1;

	return (currentTrack->v.byteOrder);
}

AFframecount afGetFrameCount (AFfilehandle file, int trackid)
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (track->ms.modulesdirty)
	{
		if (_AFsetupmodules(file, track) != AF_SUCCEED)
			return -1;
	}

	return track->totalvframes;
}

double afGetRate (AFfilehandle file, int trackid)
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	return track->f.sampleRate;
}

int afGetChannels (AFfilehandle file, int trackid)
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	return track->f.channelCount;
}

void afGetSampleFormat (AFfilehandle file, int trackid, int *sampleFormat, int *sampleWidth)
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return;

	if (sampleFormat != NULL)
		*sampleFormat = track->f.sampleFormat;

	if (sampleFormat != NULL)
		*sampleWidth = track->f.sampleWidth;
}

void afGetVirtualSampleFormat (AFfilehandle file, int trackid, int *sampleFormat, int *sampleWidth)
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return;

	if (sampleFormat != NULL)
		*sampleFormat = track->v.sampleFormat;

	if (sampleFormat != NULL)
		*sampleWidth = track->v.sampleWidth;
}

int afSetVirtualSampleFormat (AFfilehandle file, int trackid,
	int sampleFormat, int sampleWidth)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (_af_set_sample_format(&track->v, sampleFormat, sampleWidth) == AF_FAIL)
		return -1;

	track->ms.modulesdirty = AF_TRUE;

	return 0;
}

/* XXXmpruett fix the version */
int afGetFileFormat (AFfilehandle file, int *version)
{
	if (!_af_filehandle_ok(file))
		return -1;

	if (version != NULL)
	{
		if (_af_units[file->fileFormat].getversion)
			*version = _af_units[file->fileFormat].getversion(file);
		else
			*version = 0;
	}

	return file->fileFormat;
}

int afSetVirtualChannels (AFfilehandle file, int trackid, int channelCount)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	track->v.channelCount = channelCount;
	track->ms.modulesdirty = AF_TRUE;

	if (track->channelMatrix)
		free(track->channelMatrix);
	track->channelMatrix = NULL;

	return 0;
}

double afGetVirtualRate (AFfilehandle file, int trackid)
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	return track->v.sampleRate;
}

int afSetVirtualRate (AFfilehandle file, int trackid, double rate)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (rate < 0)
	{
		_af_error(AF_BAD_RATE, "invalid sampling rate %.30g", rate);
		return -1;
	}

	track->v.sampleRate = rate;
	track->ms.modulesdirty = AF_TRUE;

	return 0;
}
