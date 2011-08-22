/*
	Audio File Library
	Copyright (C) 1998, Michael Pruett <michael@68k.org>

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
	track.c

	This file contains functions for dealing with tracks within an
	audio file.
*/

#include "config.h"
#include "Track.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "audiofile.h"
#include "afinternal.h"
#include "util.h"
#include "Marker.h"
#include "modules/Module.h"

void afInitTrackIDs (AFfilesetup file, const int *trackids, int trackCount)
{
	assert(file);
	assert(trackids);
	assert(trackCount == 1);
	assert(trackids[0] == AF_DEFAULT_TRACK);
}

int afGetTrackIDs (AFfilehandle file, int *trackids)
{
	assert(file);

	if (trackids != NULL)
		trackids[0] = AF_DEFAULT_TRACK;

	return 1;
}

Track *_af_track_new (void)
{
	Track *t = (Track *) _af_malloc(sizeof (Track));

	t->id = AF_DEFAULT_TRACK;

	t->f.compressionParams = NULL;
	t->v.compressionParams = NULL;

	t->channelMatrix = NULL;

	t->markerCount = 0;
	t->markers = NULL;

	t->hasAESData = false;
	memset(t->aesData, 0, 24);

	t->totalfframes = 0;
	t->nextfframe = 0;
	t->frames2ignore = 0;
	t->fpos_first_frame = 0;
	t->fpos_next_frame = 0;
	t->fpos_after_data = 0;
	t->totalvframes = 0;
	t->nextvframe = 0;
	t->data_size = 0;

	t->ms = NULL;

	return t;
}

void Track::print()
{
	fprintf(stderr, "totalfframes %jd\n", (intmax_t) totalfframes);
	fprintf(stderr, "nextfframe %jd\n", (intmax_t) nextfframe);
	fprintf(stderr, "frames2ignore %jd\n", (intmax_t) frames2ignore);
	fprintf(stderr, "fpos_first_frame %jd\n", (intmax_t) fpos_first_frame);
	fprintf(stderr, "fpos_next_frame %jd\n", (intmax_t) fpos_next_frame);
	fprintf(stderr, "fpos_after_data %jd\n", (intmax_t) fpos_after_data);
	fprintf(stderr, "totalvframes %jd\n", (intmax_t) totalvframes);
	fprintf(stderr, "nextvframe %jd\n", (intmax_t) nextvframe);
	fprintf(stderr, "data_size %jd\n", (intmax_t) data_size);
}

Marker *Track::getMarker(int markerID)
{
	for (int i=0; i<markerCount; i++)
		if (markers[i].id == markerID)
			return &markers[i];

	_af_error(AF_BAD_MARKID, "no marker with id %d found in track %d",
		markerID, id);

	return NULL;
}
