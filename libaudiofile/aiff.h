/*
	Audio File Library
	Copyright (C) 1998-2000, Michael Pruett <michael@68k.org>

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
	aiff.h

	This file contains structures and constants related to the AIFF
	and AIFF-C formats.
*/

#ifndef AIFF_H
#define AIFF_H

#define _AF_AIFF_NUM_INSTPARAMS 9
#define _AF_AIFF_NUM_COMPTYPES 2

enum
{
	AIFCVersion1 = 0xa2805140
};

struct _COMM
{
	short			numChannels;
	long			numSampleFrames;
	short			sampleSize;
	unsigned char	sampleRate[10];
};

struct _MARK
{
	short			numMarkers;
	struct _Marker	*markers;
};

struct _INST
{
	u_int8_t	baseNote;
	int8_t		detune;
	u_int8_t	lowNote, highNote;
	u_int8_t	lowVelocity, highVelocity;
	int16_t		gain;

	int16_t	sustainLoopPlayMode;
	int16_t	sustainLoopBegin;
	int16_t	sustainLoopEnd;

	int16_t	releaseLoopPlayMode;
	int16_t	releaseLoopBegin;
	int16_t	releaseLoopEnd;
};

bool _af_aiff_recognize (AFvirtualfile *fh);
bool _af_aifc_recognize (AFvirtualfile *fh);

status _af_aiff_read_init (AFfilesetup, AFfilehandle);
status _af_aiff_write_init (AFfilesetup, AFfilehandle);
bool _af_aiff_instparam_valid (AFfilehandle, AUpvlist, int);

AFfilesetup _af_aiff_complete_setup (AFfilesetup);

status _af_aiff_update (AFfilehandle);

int _af_aifc_get_version (AFfilehandle);

#define _AF_AIFFC_NUM_COMPTYPES 2

typedef struct _AIFFInfo
{
	off_t	miscellaneousPosition;
	off_t	FVER_offset;
	off_t	COMM_offset;
	off_t	MARK_offset;
	off_t	INST_offset;
	off_t	AESD_offset;
	off_t	SSND_offset;
} _AIFFInfo;

#endif
