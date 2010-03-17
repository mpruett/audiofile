/*
	Audio File Library
	Copyright (C) 2001, Silicon Graphics, Inc.

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
	ima.c

	This module implements IMA ADPCM compression for the Audio File
	Library.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <string.h>
#include <assert.h>

#include <audiofile.h>

#include "afinternal.h"
#include "modules.h"
#include "units.h"
#include "compression.h"
#include "byteorder.h"
#include "util.h"

#include "adpcm.h"

#define CHNK(X)

static const _AFmodule ima_adpcm_decompress;

typedef struct
{
	_Track		*track;
	AFvirtualfile	*fh;

	int		blockAlign, framesPerBlock;
	AFframecount	framesToIgnore;
} ima_adpcm_data;

static int ima_adpcm_decode_block (ima_adpcm_data *ima, const uint8_t *encoded,
	int16_t *decoded)
{
	int channelCount = ima->track->f.channelCount;
	struct adpcm_state state[channelCount];

	for (int c=0; c<channelCount; c++)
	{
		state[c].valprev = (encoded[1]<<8) | encoded[0];
		if (encoded[1] & 0x80)
			state[c].valprev -= 0x10000;

		state[c].index = encoded[2];

		*decoded++ = state[c].valprev;

		encoded += 4;
	}

	_af_adpcm_decoder(encoded, decoded, ima->framesPerBlock - 1, channelCount, state);

	return ima->framesPerBlock * channelCount * sizeof (int16_t);
}

bool _af_ima_adpcm_format_ok (_AudioFormat *f)
{
	if (f->channelCount != 1)
	{
		_af_error(AF_BAD_COMPRESSION,
		       "IMA ADPCM compression requires 1 channel");
		return false;
	}

	if (f->sampleFormat != AF_SAMPFMT_TWOSCOMP || f->sampleWidth != 16)
	{
		_af_error(AF_BAD_COMPRESSION,
		       "IMA ADPCM compression requires 16-bit signed integer format");
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->sampleWidth = 16;
		/* non-fatal */
	}

	if (f->byteOrder != AF_BYTEORDER_BIGENDIAN)
	{
		_af_error(AF_BAD_COMPRESSION,
		       "IMA ADPCM compression requires big endian format");
		f->byteOrder = AF_BYTEORDER_BIGENDIAN;
		/* non-fatal */
	}

	return true;
}

static void ima_adpcm_decompress_describe (_AFmoduleinst *i)
{
/*	XXXmpruett this is probably the correct way to go, but other things
	need to be changed first.

	i->outc->f.byteOrder = _AF_BYTEORDER_NATIVE;
*/
	i->outc->f.compressionType = AF_COMPRESSION_NONE;
	i->outc->f.compressionParams = AU_NULL_PVLIST;
}

_AFmoduleinst _af_ima_adpcm_init_decompress (_Track *track, AFvirtualfile *fh,
	bool seekok, bool headerless, AFframecount *chunkframes)
{
	_AFmoduleinst	ret = _AFnewmodinst(&ima_adpcm_decompress);
	ima_adpcm_data	*d;
	AUpvlist	pv;
	long		l;

	assert(af_ftell(fh) == track->fpos_first_frame);

	d = (ima_adpcm_data *) _af_malloc(sizeof (ima_adpcm_data));

	d->track = track;
	d->fh = fh;

	d->track->frames2ignore = 0;
	d->track->fpos_next_frame = d->track->fpos_first_frame;

	pv = d->track->f.compressionParams;

	if (_af_pv_getlong(pv, _AF_FRAMES_PER_BLOCK, &l))
		d->framesPerBlock = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "samples per block not set");

	if (_af_pv_getlong(pv, _AF_BLOCK_SIZE, &l))
		d->blockAlign = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "block size not set");

	*chunkframes = d->framesPerBlock;

	ret.modspec = d;
	return ret;
}

static void ima_adpcm_run_pull (_AFmoduleinst *module)
{
	ima_adpcm_data	*d = (ima_adpcm_data *) module->modspec;
	AFframecount	frames2read = module->outc->nframes;
	AFframecount	nframes = 0;

	int framesPerBlock = d->framesPerBlock;
	assert(module->outc->nframes % framesPerBlock == 0);
	int blockCount = module->outc->nframes / framesPerBlock;

	/* Read the compressed frames. */
	ssize_t blocksRead = af_fread(module->inc->buf, d->blockAlign, blockCount, d->fh);

	/* This condition would indicate that the file is bad. */
	if (blocksRead < 0)
	{
		if (d->track->filemodhappy)
		{
			_af_error(AF_BAD_READ, "file missing data");
			d->track->filemodhappy = false;
		}
	}

	if (blocksRead < blockCount)
		blockCount = blocksRead;

	/* Decompress into module->outc. */
	for (int i=0; i<blockCount; i++)
	{
		ssize_t bytesDecoded = ima_adpcm_decode_block(d,
			(const uint8_t *) module->inc->buf + i * d->blockAlign,
			(int16_t *) module->outc->buf + i * d->framesPerBlock * d->track->f.channelCount);

		nframes += framesPerBlock;
	}

	d->track->nextfframe += nframes;

	if (blocksRead > 0)
		d->track->fpos_next_frame += blocksRead * d->blockAlign;

	assert(af_ftell(d->fh) == d->track->fpos_next_frame);

	/*
		If we got EOF from read, then we return the actual amount read.

		Complain only if there should have been more frames in the file.
	*/

	if (d->track->totalfframes != -1 && nframes != frames2read)
	{
		/* Report error if we haven't already */
		if (d->track->filemodhappy)
		{
			_af_error(AF_BAD_READ,
				"file missing data -- read %d frames, should be %d",
				d->track->nextfframe,
				d->track->totalfframes);
			d->track->filemodhappy = false;
		}
	}

	module->outc->nframes = nframes;
}

static void ima_adpcm_reset1 (_AFmoduleinst *i)
{
	ima_adpcm_data	*d = (ima_adpcm_data *) i->modspec;

	int framesPerBlock = d->framesPerBlock;
	AFframecount nextTrackFrame = d->track->nextfframe;
	d->track->nextfframe = (nextTrackFrame / framesPerBlock) *
		framesPerBlock;

	d->framesToIgnore = nextTrackFrame - d->track->nextfframe;
	/* postroll = frames2ignore */
}

static void ima_adpcm_reset2 (_AFmoduleinst *i)
{
	ima_adpcm_data	*d = (ima_adpcm_data *) i->modspec;

	int framesPerBlock = d->framesPerBlock;

	d->track->fpos_next_frame = d->track->fpos_first_frame +
		d->blockAlign * (d->track->nextfframe / framesPerBlock);
	d->track->frames2ignore += d->framesToIgnore;

	assert(d->track->nextfframe % framesPerBlock == 0);
}

static const _AFmodule ima_adpcm_decompress =
{
	"ima_adpcm_decompress",
	ima_adpcm_decompress_describe,
	AF_NULL, AF_NULL,
	ima_adpcm_run_pull, ima_adpcm_reset1, ima_adpcm_reset2,
	AF_NULL, AF_NULL, AF_NULL,
	AF_NULL,
	_AFfreemodspec
};
