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
	loop.c

	All routines that operate on loops.
*/

#include "audiofile.h"
#include "afinternal.h"
#include "util.h"
#include "setup.h"
#include "instrument.h"

void afInitLoopIDs (AFfilesetup setup, int instid, int *loopids, int nloops)
{
	int instno;

	if (!_af_filesetup_ok(setup))
		return;

	if (!_af_unique_ids(loopids, nloops, "loop", AF_BAD_LOOPID))
		return;

	if ((instno = _af_setup_instrument_index_from_id(setup, instid)) == -1)
		return;

	_af_setup_free_loops(setup, instno);

	setup->instruments[instno].loopCount = nloops;
	setup->instruments[instno].loopSet = AF_TRUE;

	if (nloops == 0)
		setup->instruments[instno].loops = NULL;
	else
	{
		int i;

		if ((setup->instruments[instno].loops = _af_calloc(nloops, sizeof (_LoopSetup))) == NULL)
			return;

		for (i=0; i < nloops; i++)
			setup->instruments[instno].loops[i].id = loopids[i];
	}
}

int afGetLoopIDs (AFfilehandle file, int instid, int *loopids)
{
	int instno;
	int i;

	if (!_af_filehandle_ok(file))
		return AF_FAIL;

	if ((instno = _af_handle_instrument_index_from_id(file, instid)) == -1)
		return AF_FAIL;

	if (loopids)
		for (i=0; i < file->instruments[instno].loopCount; i++)
			loopids[i] = file->instruments[instno].loops[i].id;

	return file->instruments[instno].loopCount;
}

int _af_handle_loop_index_from_id (AFfilehandle file, int instno, int loopid)
{
	int	i;
	for (i=0; i<file->instruments[instno].loopCount; i++)
		if (file->instruments[instno].loops[i].id == loopid)
			return i;

	_af_error(AF_BAD_LOOPID, "no loop with id %d for instrument %d",
		loopid, file->instruments[instno].id);

	return -1;
}

/*
	getLoop returns pointer to requested loop if it exists, and if
	mustWrite is true, only if handle is writable.
*/

_Loop *getLoop (AFfilehandle handle, int instid, int loopid, bool mustWrite)
{
	int	loopno, instno;

	if (!_af_filehandle_ok(handle))
		return NULL;

	if (mustWrite && !_af_filehandle_can_write(handle))
		return NULL;

	if ((instno = _af_handle_instrument_index_from_id(handle, instid)) == -1)
		return NULL;

	if ((loopno = _af_handle_loop_index_from_id(handle, instno, loopid)) == -1)
		return NULL;

	return &handle->instruments[instno].loops[loopno];
}

/*
	Set loop mode (as in AF_LOOP_MODE_...).
*/
void afSetLoopMode (AFfilehandle file, int instid, int loopid, int mode)
{
	_Loop	*loop = getLoop(file, instid, loopid, AF_TRUE);

	if (!loop)
		return;

	if (mode != AF_LOOP_MODE_NOLOOP &&
		mode != AF_LOOP_MODE_FORW &&
		mode != AF_LOOP_MODE_FORWBAKW)
	{
		_af_error(AF_BAD_LOOPMODE, "unrecognized loop mode %d", mode);
		return;
	}

	loop->mode = mode;
}

/*
	Get loop mode (as in AF_LOOP_MODE_...).
*/
int afGetLoopMode (AFfilehandle file, int instid, int loopid)
{
	_Loop	*loop = getLoop(file, instid, loopid, AF_FALSE);

	if (loop == NULL)
		return -1;

	return loop->mode;
}

/*
	Set loop count.
*/
int afSetLoopCount (AFfilehandle file, int instid, int loopid, int count)
{
	_Loop	*loop = getLoop(file, instid, loopid, AF_TRUE);

	if (loop == NULL)
		return AF_FAIL;

	if (count < 1)
	{
		_af_error(AF_BAD_LOOPCOUNT, "invalid loop count: %d", count);
		return AF_FAIL;
	}

	loop->count = count;
	return AF_SUCCEED;
}

/*
	Get loop count.
*/
int afGetLoopCount(AFfilehandle file, int instid, int loopid)
{
	_Loop	*loop = getLoop(file, instid, loopid, AF_FALSE);

	if (loop == NULL)
		return -1;

	return loop->count;
}

/*
	Set loop start marker id in the file structure
*/
void
afSetLoopStart(AFfilehandle file, int instid, int loopid, int markid)
{
	_Loop	*loop = getLoop(file, instid, loopid, AF_TRUE);

	if (!loop)
		return;

	loop->beginMarker = markid;
}

/*
	Get loop start marker id.
*/
int afGetLoopStart (AFfilehandle file, int instid, int loopid)
{
	_Loop	*loop = getLoop(file, instid, loopid, AF_FALSE);

	if (loop == NULL)
		return -1;

	return loop->beginMarker;
}

/*
	Set loop start frame in the file structure.
*/
int afSetLoopStartFrame (AFfilehandle file, int instid, int loopid, AFframecount startFrame)
{
	int	trackid, beginMarker;
	_Loop	*loop = getLoop(file, instid, loopid, AF_TRUE);

	if (loop == NULL)
		return -1;

	if (startFrame < 0)
	{
		_af_error(AF_BAD_FRAME, "loop start frame must not be negative");
		return AF_FAIL;
	}

	trackid = loop->trackid;
	beginMarker = loop->beginMarker;

	afSetMarkPosition(file, trackid, beginMarker, startFrame);
	return AF_SUCCEED;
}

/*
	Get loop start frame.
*/
AFframecount afGetLoopStartFrame (AFfilehandle file, int instid, int loopid)
{
	int	trackid, beginMarker;
	_Loop	*loop = getLoop(file, instid, loopid, AF_FALSE);

	if (loop == NULL)
		return -1;

	trackid = loop->trackid;
	beginMarker = loop->beginMarker;

	return afGetMarkPosition(file, trackid, beginMarker);
}

/*
	Set loop track id.
*/
void afSetLoopTrack (AFfilehandle file, int instid, int loopid, int track)
{
	_Loop *loop = getLoop(file, instid, loopid, AF_TRUE);

	if (!loop) return;

	loop->trackid = track;
}

/*
	Get loop track.
*/
int afGetLoopTrack (AFfilehandle file, int instid, int loopid)
{
	_Loop	*loop = getLoop(file, instid, loopid, AF_FALSE);

	if (loop == NULL)
		return -1;

	return loop->trackid;
}

/*
	Set loop end frame marker id.
*/
void afSetLoopEnd (AFfilehandle file, int instid, int loopid, int markid)
{
	_Loop	*loop = getLoop(file, instid, loopid, AF_TRUE);

	if (!loop)
		return;

	loop->endMarker = markid;
}

/*
	Get loop end frame marker id.
*/
int afGetLoopEnd (AFfilehandle file, int instid, int loopid)
{
	_Loop	*loop = getLoop(file, instid, loopid, AF_FALSE);

	if (loop == NULL)
		return -1;

	return loop->endMarker;
}

/*
	Set loop end frame.
*/
int afSetLoopEndFrame (AFfilehandle file, int instid, int loopid, AFframecount endFrame)
{
	int	trackid, endMarker;
	_Loop	*loop = getLoop(file, instid, loopid, AF_TRUE);

	if (loop == NULL)
		return -1;

	if (endFrame < 0)
	{
		_af_error(AF_BAD_FRAME, "loop end frame must not be negative");
		return AF_FAIL;
	}

	trackid = loop->trackid;
	endMarker = loop->endMarker;

	afSetMarkPosition(file, trackid, endMarker, endFrame);
	return AF_SUCCEED;
}

/*
	Get loop end frame.
*/

AFframecount afGetLoopEndFrame (AFfilehandle file, int instid, int loopid)
{
	int	trackid, endMarker;
	_Loop	*loop = getLoop(file, instid, loopid, AF_FALSE);

	if (loop == NULL)
		return -1;

	trackid = loop->trackid;
	endMarker = loop->endMarker;

	return afGetMarkPosition(file, trackid, endMarker);
}
