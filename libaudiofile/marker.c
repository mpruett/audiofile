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
	marker.c

	This file contains routines for dealing with loop markers.
*/

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"
#include "util.h"

static _Marker *findMarkerByID (_Track *track, int markerid)
{
	int	i;

	assert(track);

	for (i=0; i<track->markerCount; i++)
		if (track->markers[i].id == markerid)
			return &track->markers[i];

	_af_error(AF_BAD_MARKID, "no mark with id %d found in track %d",
		markerid, track->id);

	return NULL;
}

void afInitMarkIDs(AFfilesetup setup, int trackid, int markids[], int nmarks)
{
	int		i;
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	if (track->markers != NULL)
	{
		for (i=0; i<track->markerCount; i++)
		{
			if (track->markers[i].name != NULL)
				free(track->markers[i].name);
			if (track->markers[i].comment != NULL)
				free(track->markers[i].comment);
		}
		free(track->markers);
	}

	track->markers = _af_calloc(nmarks, sizeof (struct _MarkerSetup));
	track->markerCount = nmarks;

	for (i=0; i<nmarks; i++)
	{
		track->markers[i].id = markids[i];
		track->markers[i].name = _af_strdup("");
		track->markers[i].comment = _af_strdup("");
	}

	track->markersSet = AF_TRUE;
}

void afInitMarkName(AFfilesetup setup, int trackid, int markid,
	const char *namestr)
{
	int	markno;
	int	length;

	_TrackSetup	*track = NULL;

	assert(setup);
	assert(markid > 0);

	track = _af_filesetup_get_tracksetup(setup, trackid);
	assert(track);

	if (track == NULL)
	{
		_af_error(AF_BAD_TRACKID, "bad track id");
		return;
	}

	for (markno=0; markno<track->markerCount; markno++)
	{
		if (track->markers[markno].id == markid)
			break;
	}

	if (markno == track->markerCount)
	{
		_af_error(AF_BAD_MARKID, "no marker id %d for file setup", markid);
		return;
	}

	length = strlen(namestr);
	if (length > 255)
	{
		_af_error(AF_BAD_STRLEN,
			"warning: marker name truncated to 255 characters");
		length = 255;
	}

	if (track->markers[markno].name)
		free(track->markers[markno].name);
	if ((track->markers[markno].name = _af_malloc(length+1)) == NULL)
		return;
	strncpy(track->markers[markno].name, namestr, length);
	/*
		The null terminator is not set by strncpy if
		strlen(namestr) > length.  Set it here.
	*/
	track->markers[markno].name[length] = '\0';
}

void afInitMarkComment(AFfilesetup setup, int trackid, int markid,
	const char *commstr)
{
	int	markno;
	int	length;
	_TrackSetup	*track = NULL;

	assert(setup);
	assert(markid > 0);

	track = _af_filesetup_get_tracksetup(setup, trackid);
	assert(track);

	if (track == NULL)
	{
		_af_error(AF_BAD_TRACKID, "bad track id");
		return;
	}

	for (markno=0; markno<track->markerCount; markno++)
	{
		if (track->markers[markno].id == markid)
			break;
	}

	if (markno == track->markerCount)
	{
		_af_error(AF_BAD_MARKID, "no marker id %d for file setup", markid);
		return;
	}

	length = strlen(commstr);

	if (track->markers[markno].name)
		free(track->markers[markno].name);
	if ((track->markers[markno].name = _af_malloc(length+1)) == NULL)
		return;
	strcpy(track->markers[markno].comment, commstr);
}

char *afGetMarkName (AFfilehandle file, int trackid, int markid)
{
	_Track	*track;
	_Marker	*marker;

	assert(file != NULL);
	assert(markid > 0);

	if (!_af_filehandle_ok(file))
		return NULL;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return NULL;

	if ((marker = findMarkerByID(track, markid)) == NULL)
		return NULL;

	return marker->name;
}

char *afGetMarkComment (AFfilehandle file, int trackid, int markid)
{
	_Track	*track;
	_Marker	*marker;

	assert(file != NULL);
	assert(markid > 0);

	if (!_af_filehandle_ok(file))
		return NULL;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return NULL;

	if ((marker = findMarkerByID(track, markid)) == NULL)
		return NULL;

	return marker->comment;
}

void afSetMarkPosition (AFfilehandle file, int trackid, int markid,
	AFframecount pos)
{
	_Track	*track;
	_Marker	*marker;

	assert(file != NULL);
	assert(markid > 0);

	if (!_af_filehandle_ok(file))
		return;

	if (!_af_filehandle_can_write(file))
		return;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return;

	if ((marker = findMarkerByID(track, markid)) == NULL)
		return;

	if (pos < 0)
	{
		_af_error(AF_BAD_MARKPOS, "invalid marker position %d", pos);
		pos = 0;
	}

	marker->position = pos;
}

int afGetMarkIDs (AFfilehandle file, int trackid, int markids[])
{
	_Track	*track;

	assert(file);

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (markids != NULL)
	{
		int	i;

		for (i=0; i<track->markerCount; i++)
		{
			markids[i] = track->markers[i].id;
		}
	}

	return track->markerCount;
}

AFframecount afGetMarkPosition (AFfilehandle file, int trackid, int markid)
{
	_Track	*track;
	_Marker	*marker;

	assert(file);
	assert(markid > 0);

	if (!_af_filehandle_ok(file))
		return 0L;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return 0L;

	if ((marker = findMarkerByID(track, markid)) == NULL)
		return 0L;

	return marker->position;
}

_Marker *_af_marker_new (int count)
{
	_Marker	*markers = _af_calloc(count, sizeof (_Marker));
	if (markers == NULL)
		return NULL;

	return markers;
}
