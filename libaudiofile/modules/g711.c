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
	g711.c
*/

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <audiofile.h>
#include "afinternal.h"
#include "modules.h"
#include "units.h"
#include "compression.h"
#include "g711.h"
#include "byteorder.h"

#define CHNK(X)

void ulaw2linear_buf (unsigned char *ulaw, signed short int *linear,
	int nsamples)
{
	int i;
	for (i=0; i < nsamples; i++)
	{
		linear[i] = _af_ulaw2linear(ulaw[i]);
	}
}

void linear2ulaw_buf (signed short int *linear, unsigned char *ulaw,
	int nsamples)
{
	int i;
	for (i=0; i < nsamples; i++)
	{
		ulaw[i] = _af_linear2ulaw(linear[i]);
	}
}

void alaw2linear_buf (unsigned char *alaw, signed short int *linear,
	int nsamples)
{
	int i;
	for (i=0; i < nsamples; i++)
	{
		linear[i] = _af_alaw2linear(alaw[i]);
	}
}

void linear2alaw_buf (signed short int *linear, unsigned char *alaw,
	int nsamples)
{
	int i;
	for (i=0; i < nsamples; i++)
	{
		alaw[i] = _af_linear2alaw(linear[i]);
	}
}

bool _af_g711_format_ok (_AudioFormat *f)
{
	if (f->sampleFormat != AF_SAMPFMT_TWOSCOMP || f->sampleWidth != 16)
	{
		_af_error(AF_BAD_COMPRESSION,
		       "G711 compression requires 16-bit signed integer format");
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->sampleWidth = 16;
		/* non-fatal */
	}

	if (f->byteOrder != AF_BYTEORDER_BIGENDIAN)
	{
		_af_error(AF_BAD_COMPRESSION,
		       "G711 compression requires big endian format");
		f->byteOrder = AF_BYTEORDER_BIGENDIAN;
		/* non-fatal */
	}

	return AF_TRUE;
}

_AFmodule g711compress;
_AFmodule g711decompress;

typedef unsigned char g711samp;

typedef struct g711_data
{
	_Track *trk;
	AFvirtualfile *fh;
	bool seekok;

	/* saved_fpos_next_frame and saved_nextfframe apply only to writing. */
	int saved_fpos_next_frame;
	int saved_nextfframe;
} g711_data;

void g711compressdescribe (_AFmoduleinst *i)
{
	g711_data *d = (g711_data *)i->modspec;
	i->outc->f.compressionType = d->trk->f.compressionType;
}

_AFmoduleinst _AFg711initcompress (_Track *trk, AFvirtualfile *fh, bool seekok,
	bool headerless, AFframecount *chunkframes)
{
	_AFmoduleinst ret = _AFnewmodinst(&g711compress);
	g711_data *d;

	d = (g711_data *) _af_malloc(sizeof (g711_data));

	d->trk = trk;
	d->fh = fh;
	d->seekok = seekok;

	d->trk->fpos_next_frame = d->trk->fpos_first_frame;

	ret.modspec = d;
	return ret;
}

void g711run_push (_AFmoduleinst *i)
{
	g711_data *d = (g711_data *)i->modspec;
	AFframecount frames2write = i->inc->nframes;
	AFframecount samps2write = i->inc->nframes * i->inc->f.channelCount;
	int framesize = sizeof (g711samp) * (i->inc->f.channelCount);
	AFframecount nfr;

	assert(d->trk->f.compressionType == AF_COMPRESSION_G711_ULAW ||
		d->trk->f.compressionType == AF_COMPRESSION_G711_ALAW);

	/* Compress frames into i->outc. */

	if (d->trk->f.compressionType == AF_COMPRESSION_G711_ULAW)
		linear2ulaw_buf(i->inc->buf, i->outc->buf, samps2write);
	else
		linear2alaw_buf(i->inc->buf, i->outc->buf, samps2write);

	/* Write the compressed data. */

	nfr = af_fwrite(i->outc->buf, framesize, frames2write, d->fh);

	CHNK(printf("writing %d frames to g711 file\n", frames2write));

	if (nfr != frames2write)
	{
		/* report error if we haven't already */
		if (d->trk->filemodhappy)
		{
			/* i/o error */
			if (nfr < 0)
				_af_error(AF_BAD_WRITE,
					"unable to write data (%s) -- "
					"wrote %d out of %d frames",
					strerror(errno),
					d->trk->nextfframe + nfr,
					d->trk->nextfframe + frames2write);

			/* usual disk full error */
			else
				_af_error(AF_BAD_WRITE,
					"unable to write data (disk full) -- "
					"wrote %d out of %d frames",
					d->trk->nextfframe + nfr,
					d->trk->nextfframe + frames2write);

			d->trk->filemodhappy = AF_FALSE;
		}
	}

	d->trk->nextfframe += nfr;
	d->trk->totalfframes = d->trk->nextfframe;
	d->trk->fpos_next_frame += (nfr>0) ? nfr/framesize : 0;

	assert(!d->seekok || (af_ftell(d->fh) == d->trk->fpos_next_frame));
}

void g711sync1 (_AFmoduleinst *i)
{
	g711_data *d = (g711_data *)i->modspec;

	d->saved_fpos_next_frame = d->trk->fpos_next_frame;
	d->saved_nextfframe = d->trk->nextfframe;
}

void g711sync2 (_AFmoduleinst *i)
{
	g711_data *d = (g711_data *) i->modspec;

	/* sanity check. */
	assert(!d->seekok || (af_ftell(d->fh) == d->trk->fpos_next_frame));

	/* We can afford to do an lseek just in case because sync2 is rare. */
	d->trk->fpos_after_data = af_ftell(d->fh);

	d->trk->fpos_next_frame = d->saved_fpos_next_frame;
	d->trk->nextfframe = d->saved_nextfframe;
}

void g711decompressdescribe(_AFmoduleinst *i)
{
/*	XXXmpruett this is probably the correct way to go, but other things
	need to be changed first.

	i->outc->f.byteOrder = _AF_BYTEORDER_NATIVE;
*/
	i->outc->f.compressionType = AF_COMPRESSION_NONE;
	i->outc->f.compressionParams = AU_NULL_PVLIST;
}

_AFmoduleinst _AFg711initdecompress (_Track *trk, AFvirtualfile *fh,
	bool seekok, bool headerless, AFframecount *chunkframes)
{
	_AFmoduleinst ret = _AFnewmodinst(&g711decompress);
	g711_data *d;

	d = (g711_data *) _af_malloc(sizeof(g711_data));

	d->trk = trk;
	d->fh = fh;
	d->seekok = seekok;

	d->trk->f.compressionParams = AU_NULL_PVLIST;

	d->trk->frames2ignore = 0;
	d->trk->fpos_next_frame = d->trk->fpos_first_frame;

	ret.modspec = d;
	return ret;
}

void g711run_pull (_AFmoduleinst *i)
{
	g711_data *d = (g711_data *) i->modspec;
	AFframecount frames2read = i->outc->nframes;
	AFframecount samps2read = i->outc->nframes * i->outc->f.channelCount;
	int framesize = sizeof(g711samp)*(i->outc->f.channelCount);
	AFframecount nfr;

	/* Read the compressed frames. */

	nfr = af_fread(i->inc->buf, framesize, frames2read, d->fh);

	/* Decompress into i->outc. */

	if (d->trk->f.compressionType == AF_COMPRESSION_G711_ULAW)
		ulaw2linear_buf(i->inc->buf, i->outc->buf, samps2read);
	else
		alaw2linear_buf(i->inc->buf, i->outc->buf, samps2read);

	CHNK(printf("reading %d frames from g711 file (got %d)\n",
		frames2read, nfr));

	d->trk->nextfframe += nfr;
	d->trk->fpos_next_frame += (nfr>0) ? nfr/framesize : 0;
	assert(!d->seekok || (af_ftell(d->fh) == d->trk->fpos_next_frame));

	/*
		If we got EOF from read, then we return the actual amount read.

		Complain only if there should have been more frames in the file.
	*/

	if (d->trk->totalfframes != -1 && nfr != frames2read)
	{
		/* Report error if we haven't already */
		if (d->trk->filemodhappy)
		{
			_af_error(AF_BAD_READ,
				"file missing data -- read %d frames, should be %d",
				d->trk->nextfframe,
				d->trk->totalfframes);
			d->trk->filemodhappy = AF_FALSE;
		}
	}

	i->outc->nframes = nfr;
}

void g711reset1 (_AFmoduleinst *i)
{
#ifdef DONE
	g711_data *d = (g711_data *) i->modspec;
#endif
	/* This function is supposed to be empty to fit into design. */
}

void g711reset2 (_AFmoduleinst *i)
{
	g711_data *d = (g711_data *) i->modspec;
	int framesize = sizeof (g711samp) * (i->inc->f.channelCount);

	d->trk->fpos_next_frame =
	d->trk->fpos_first_frame  +  framesize * d->trk->nextfframe;

	d->trk->frames2ignore = 0;
}

_AFmodule g711compress =
{
	"g711compress",
	g711compressdescribe,
	AF_NULL, AF_NULL,
	AF_NULL, AF_NULL, AF_NULL,
	g711run_push, g711sync1, g711sync2,
	AF_NULL,
	_AFfreemodspec
};

_AFmodule g711decompress =
{
	"g711decompress",
	g711decompressdescribe,
	AF_NULL, AF_NULL,
	g711run_pull, g711reset1, g711reset2,
	AF_NULL, AF_NULL, AF_NULL,
	AF_NULL,
	_AFfreemodspec
};
