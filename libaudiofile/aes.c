/*
	Audio File Library
	Copyright (C) 1998-1999, Michael Pruett <michael@68k.org>
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
	aes.c

	This file contains routines for dealing with AES recording data.
*/

#include <string.h>
#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"
#include "util.h"

void afInitAESChannelData (AFfilesetup setup, int trackid)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	track->aesDataSet = AF_TRUE;
}

void afInitAESChannelDataTo (AFfilesetup setup, int trackid, int willBeData)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	track->aesDataSet = willBeData;
}

/*
	What is with these return values?
*/
int afGetAESChannelData (AFfilehandle file, int trackid, unsigned char buf[24])
{
	_Track *track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (track->hasAESData == AF_FALSE)
	{
		if (buf)
			memset(buf, 0, 24);
		return 0;
	}

	if (buf)
		memcpy(buf, track->aesData, 24);

	return 1;
}

void afSetAESChannelData (AFfilehandle file, int trackid, unsigned char buf[24])
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return;

	if (!_af_filehandle_can_write(file))
		return;

	if (track->hasAESData)
	{
		memcpy(track->aesData, buf, 24);
	}
	else
	{
		_af_error(AF_BAD_NOAESDATA,
			"unable to store AES channel status data for track %d",
			trackid);
	}
}
