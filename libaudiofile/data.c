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
	data.c
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "audiofile.h"
#include "afinternal.h"
#include "util.h"
#include "modules.h"

int afWriteFrames (AFfilehandle file, int trackid, void *samples, int nvframes2write)
{
	_AFmoduleinst	*firstmod;
	_AFchunk	*userc;
	_Track		*track;
	int		bytes_per_vframe;
	AFframecount	vframe;

	if (!_af_filehandle_ok(file))
		return -1;

	if (!_af_filehandle_can_write(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (track->ms.modulesdirty)
	{
		if (_AFsetupmodules(file, track) != AF_SUCCEED)
			return -1;
	}

	/*if (file->seekok) {*/

	if (af_fseek(file->fh, track->fpos_next_frame, SEEK_SET) < 0)
	{
		_af_error(AF_BAD_LSEEK, "unable to position write pointer at next frame");
		return -1;
	}

	/* } */

	bytes_per_vframe = _af_format_frame_size(&track->v, AF_TRUE);

	firstmod = &track->ms.module[0];
	userc = &track->ms.chunk[0];

	track->filemodhappy = AF_TRUE;

	vframe = 0;
#ifdef UNLIMITED_CHUNK_NVFRAMES
	/*
		OPTIMIZATION: see the comment at the very end of
		arrangemodules() in modules.c for an explanation of this:
	*/
	if (!trk->ms.mustuseatomicnvframes)
	{
		userc->buf = (char *)buf;
		userc->nframes = nvframes2write;

		(*firstmod->mod->run_push)(firstmod);

		/* Count this chunk if there was no i/o error. */
		if (trk->filemodhappy)
			vframe += userc->nframes;
	}
	else
#else
	/* Optimization must be off. */
	assert(track->ms.mustuseatomicnvframes);
#endif
	{
		while (vframe < nvframes2write)
		{
			userc->buf = (char *) samples + bytes_per_vframe * vframe;
			if (vframe <= nvframes2write - _AF_ATOMIC_NVFRAMES)
				userc->nframes = _AF_ATOMIC_NVFRAMES;
			else
				userc->nframes = nvframes2write - vframe;

			(*firstmod->mod->run_push)(firstmod);

			if (track->filemodhappy == AF_FALSE)
				break;

			vframe += userc->nframes;
		}
	}

	track->nextvframe += vframe;
	track->totalvframes += vframe;

	return vframe;
}

int afReadFrames (AFfilehandle file, int trackid, void *samples, int nvframeswanted)
{
	_Track	*track;
	_AFmoduleinst	*firstmod;
	_AFchunk	*userc;
	AFframecount	nvframesleft, nvframes2read;
	int		bytes_per_vframe;
	AFframecount	vframe;

	if (!_af_filehandle_ok(file))
		return -1;

	if (!_af_filehandle_can_read(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (track->ms.modulesdirty)
	{
		if (_AFsetupmodules(file, track) != AF_SUCCEED)
			return -1;
	}

	/*if (file->seekok) {*/

	if (af_fseek(file->fh, track->fpos_next_frame, SEEK_SET) < 0)
	{
		_af_error(AF_BAD_LSEEK, "unable to position read pointer at next frame");
		return -1;
	}

	/* } */

	if (track->totalvframes == -1)
		nvframes2read = nvframeswanted;
	else
	{
		nvframesleft = track->totalvframes - track->nextvframe;
		nvframes2read = (nvframeswanted > nvframesleft) ?
			nvframesleft : nvframeswanted;
	}
	bytes_per_vframe = _af_format_frame_size(&track->v, AF_FALSE);

	firstmod = &track->ms.module[track->ms.nmodules-1];
	userc = &track->ms.chunk[track->ms.nmodules];

	track->filemodhappy = AF_TRUE;

	vframe = 0;

	if (!track->ms.mustuseatomicnvframes)
	{
		assert(track->frames2ignore == 0);
		userc->buf = samples;
		userc->nframes = nvframes2read;

		(*firstmod->mod->run_pull)(firstmod);
		if (track->filemodhappy)
			vframe += userc->nframes;
	}
	else
	{
		bool	eof = AF_FALSE;

		if (track->frames2ignore != 0)
		{
			userc->nframes = track->frames2ignore;
			userc->buf = _af_malloc(track->frames2ignore * bytes_per_vframe);
			if (userc->buf == AF_NULL)
				return 0;

			(*firstmod->mod->run_pull)(firstmod);

			/* Have we hit EOF? */
			if (userc->nframes < track->frames2ignore)
				eof = AF_TRUE;

			track->frames2ignore = 0;

			free(userc->buf);
			userc->buf = NULL;
		}

		/*
			Now start reading useful frames, until EOF or
			premature EOF.
		*/

		while (track->filemodhappy && !eof && vframe < nvframes2read)
		{
			AFframecount	nvframes2pull;
			userc->buf = (char *) samples + bytes_per_vframe * vframe;

			if (vframe <= nvframes2read - _AF_ATOMIC_NVFRAMES)
				nvframes2pull = _AF_ATOMIC_NVFRAMES;
			else
				nvframes2pull = nvframes2read - vframe;

			userc->nframes = nvframes2pull;

			(*firstmod->mod->run_pull)(firstmod);

			if (track->filemodhappy)
			{
				vframe += userc->nframes;
				if (userc->nframes < nvframes2pull)
					eof = AF_TRUE;
			}
		}
	}

	track->nextvframe += vframe;

	return vframe;
}
