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
	pcm.c - read and file write module for uncompressed data
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <audiofile.h>
#include "afinternal.h"
#include "compression.h"
#include "modules.h"

#ifdef DEBUG
#define CHNK(X) (X)
#else
#define CHNK(X)
#endif

bool _af_pcm_format_ok (_AudioFormat *f)
{
	assert(!isnan(f->pcm.slope));
	assert(!isnan(f->pcm.intercept));
	assert(!isnan(f->pcm.minClip));
	assert(!isnan(f->pcm.maxClip));

	return AF_TRUE;
}

/*
	The pcm module does both reading and writing.
*/

_AFmodule pcm;

typedef struct pcm_data
{
	_Track *trk;
	AFvirtualfile *fh;
	bool seekok;
	int bytes_per_frame;

	/* saved_fpos_next_frame and saved_nextfframe apply only to writing. */
	int saved_fpos_next_frame;
	int saved_nextfframe;
} pcm_data;

_AFmoduleinst _AFpcminitcompress (_Track *trk, AFvirtualfile *fh, bool seekok,
	bool headerless, AFframecount *chunkframes)
{
	_AFmoduleinst ret = _AFnewmodinst(&pcm);
	pcm_data *d;

	d = (pcm_data *) _af_malloc(sizeof (pcm_data));

	d->trk = trk;
	d->fh = fh;
	d->seekok = seekok;
	d->bytes_per_frame = _af_format_frame_size(&trk->f, AF_FALSE);

	d->trk->fpos_next_frame = d->trk->fpos_first_frame;

	ret.modspec = d;
	return ret;
}

void pcmrun_push (_AFmoduleinst *i)
{
	pcm_data *d = (pcm_data *) i->modspec;
	AFframecount frames2write = i->inc->nframes;
	AFframecount n;

	/*
	WARNING: due to the optimization explained at the end of arrangemodules(),
	the pcm file module cannot depend on the presence of the intermediate
	working buffer which _AFsetupmodules usually allocates for file modules in
	their input or output chunk (for reading or writing, respectively).

	Fortunately, the pcm module has no need for such a buffer.
	*/

	n = af_fwrite(i->inc->buf, d->bytes_per_frame, frames2write, d->fh);

	CHNK(printf("writing %d frames to pcm file\n", frames2write));

	if (n != frames2write)
	{
		/* Report error if we haven't already. */
		if (d->trk->filemodhappy)
		{
			/* I/O error */
			if (n < 0)
				_af_error(AF_BAD_WRITE,
					"unable to write data (%s) -- "
					"wrote %d out of %d frames",
					strerror(errno),
					d->trk->nextfframe + n,
					d->trk->nextfframe + frames2write);
			/* usual disk full error */
			else
				_af_error(AF_BAD_WRITE,
					"unable to write data (disk full) -- "
					"wrote %d out of %d frames",
					d->trk->nextfframe + n,
					d->trk->nextfframe + frames2write);
			d->trk->filemodhappy = AF_FALSE;
		}
	}

	d->trk->nextfframe += n;
	d->trk->totalfframes = d->trk->nextfframe;
	d->trk->fpos_next_frame += (n>0) ? n*d->bytes_per_frame : 0;
	assert(!d->seekok || (af_ftell(d->fh) == d->trk->fpos_next_frame));
}

void pcmsync1 (_AFmoduleinst *i)
{
	pcm_data *d = (pcm_data *)i->modspec;

	d->saved_fpos_next_frame = d->trk->fpos_next_frame;
	d->saved_nextfframe = d->trk->nextfframe;
}

void pcmsync2 (_AFmoduleinst *i)
{
	pcm_data *d = (pcm_data *)i->modspec;

	/* sanity check */
	assert(!d->seekok || (af_ftell(d->fh) == d->trk->fpos_next_frame));

	/* We can afford to do an lseek just in case cuz sync2 is rare. */
	d->trk->fpos_after_data = af_ftell(d->fh);

	d->trk->fpos_next_frame = d->saved_fpos_next_frame;
	d->trk->nextfframe = d->saved_nextfframe;
}

_AFmoduleinst _AFpcminitdecompress (_Track *trk, AFvirtualfile *fh, bool seekok,
	bool headerless, AFframecount *chunkframes)
{
	_AFmoduleinst ret = _AFnewmodinst(&pcm);
	pcm_data *d;

	d = (pcm_data *) _af_malloc(sizeof (pcm_data));

	d->trk = trk;
	d->fh = fh;
	d->seekok = seekok;

	d->trk->f.compressionParams = AU_NULL_PVLIST;

	d->bytes_per_frame = _af_format_frame_size(&trk->f, AF_FALSE);

	ret.modspec = d;
	return ret;
}

void pcmrun_pull (_AFmoduleinst *i)
{
	pcm_data *d = (pcm_data *) i->modspec;
	AFframecount n, frames2read = i->outc->nframes;

	/*
		WARNING: Due to the optimization explained at the end of
		arrangemodules(), the pcm file module cannot depend on
		the presence of the intermediate working buffer which
		_AFsetupmodules usually allocates for file modules in
		their input or output chunk (for reading or writing,
		respectively).

		Fortunately, the pcm module has no need for such a buffer.
	*/

	/*
		Limit the number of frames to be read to the number of
		frames left in the track.
	*/
	if (d->trk->totalfframes != -1 &&
		d->trk->nextfframe + frames2read > d->trk->totalfframes)
	{
		frames2read = d->trk->totalfframes - d->trk->nextfframe;
	}

	n = af_fread(i->outc->buf, d->bytes_per_frame, frames2read, d->fh);

	CHNK(printf("reading %d frames from pcm file (got %d)\n",
		frames2read, n));

	d->trk->nextfframe += n;
	d->trk->fpos_next_frame += (n>0) ? n*d->bytes_per_frame : 0;
	assert(!d->seekok || (af_ftell(d->fh) == d->trk->fpos_next_frame));

	/*
		If we got EOF from read, then we return the actual amount read.

		Complain only if there should have been more frames in the file.
	*/

	if (n != frames2read && d->trk->totalfframes != -1)
	{
		/* Report error if we haven't already. */
		if (d->trk->filemodhappy)
		{
			_af_error(AF_BAD_READ,
				"file missing data -- read %d frames, "
				"should be %d",
				d->trk->nextfframe,
				d->trk->totalfframes);
			d->trk->filemodhappy = AF_FALSE;
		}
	}

	i->outc->nframes = n;
}

void pcmreset1 (_AFmoduleinst *i)
{
#ifdef DONE
	pcm_data *d = (pcm_data *) i->modspec;
#endif
	/* This function is supposed to be empty to fit into design. */
}

void pcmreset2 (_AFmoduleinst *i)
{
	pcm_data *d = (pcm_data *) i->modspec;

	d->trk->fpos_next_frame = d->trk->fpos_first_frame +
		d->bytes_per_frame * d->trk->nextfframe;

	d->trk->frames2ignore = 0;
}

_AFmodule pcm =
{
	"pcm",
	AF_NULL,
	AF_NULL, AF_NULL,
	pcmrun_pull, pcmreset1, pcmreset2,
	pcmrun_push, pcmsync1, pcmsync2,
	AF_NULL,
	_AFfreemodspec
};
