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
	modules.c
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <assert.h>

#include <audiofile.h>
#include "afinternal.h"
#include "modules.h"
#include "pcm.h"
#include "util.h"
#include "units.h"
#include "compression.h"
#include "byteorder.h"

#include "modules/rebuffer.h"

#define CHNK(X)
#define DEBG(X)

extern _PCMInfo _af_default_signed_integer_pcm_mappings[];
extern _PCMInfo _af_default_unsigned_integer_pcm_mappings[];
extern _PCMInfo _af_default_float_pcm_mapping;
extern _PCMInfo _af_default_double_pcm_mapping;

extern _CompressionUnit _af_compression[];

/* Define rebuffering modules. */
extern _AFmodule int2rebufferv2f, int2rebufferf2v;

/*
	module utility routines
*/

/*
	_AFnewmodinst creates a module instance from a module.
	It returns a structure, not a pointer to a structure.
*/
_AFmoduleinst _AFnewmodinst (_AFmodule *mod)
{
	_AFmoduleinst ret;

	ret.inc = ret.outc = NULL;
	ret.modspec = NULL;
	ret.u.pull.source = NULL;
	ret.mod = mod;
	ret.free_on_close = AF_FALSE;
	ret.valid = AF_FALSE;

	return(ret);
}

/*
	_AFfreemodspec:	useful routine for mod.free function pointer
*/
void _AFfreemodspec (_AFmoduleinst *i)
{
	if (i->modspec)
		free(i->modspec);
	i->modspec = NULL;
}

/*
	_AFpull: used a lot -- see comments in README.modules
*/
AFframecount _AFpull (_AFmoduleinst *i, AFframecount nframes2pull)
{
	_AFmoduleinst *src = i->u.pull.source;

	i->inc->nframes = nframes2pull;
	CHNK(printf("%s pulling %d frames from %s\n",
		i->mod->name, i->inc->nframes, src->mod->name));
	(*src->mod->run_pull)(src);
	CHNK(_af_print_chunk(i->inc));

	CHNK(printf("%s received %d frames from %s\n",
		i->mod->name, i->inc->nframes, src->mod->name));
	return i->inc->nframes;
}

/*
	_AFsimplemodrun
*/
void _AFsimplemodrun_pull (_AFmoduleinst *i)
{
	_AFpull(i, i->outc->nframes);
	(*i->mod->run)(i->inc, i->outc, i->modspec);
}

/*
	_AFpush
*/
void _AFpush (_AFmoduleinst *i, AFframecount nframes2push)
{
	_AFmoduleinst *snk = i->u.push.sink;
	i->outc->nframes = nframes2push;
	CHNK(printf("%s pushing %d frames into %s\n",
		i->mod->name, i->outc->nframes, snk->mod->name));
	CHNK(_af_print_chunk(i->outc));
	(*(snk->mod->run_push))(snk);
}

/*
	_AFpushat
*/
void _AFpushat (_AFmoduleinst *i, AFframecount startframe, bool stretchint,
	AFframecount nframes2push)
{
	_AFmoduleinst *snk = i->u.push.sink;

	void *saved_buf = i->outc->buf;
	i->outc->buf = ((char *)i->outc->buf) +
		(_af_format_frame_size_uncompressed(&i->outc->f,stretchint) * startframe);

	i->outc->nframes = nframes2push;
	CHNK(printf("%s pushing %d frames into %s with OFFSET %d frames\n",
		i->mod->name, i->outc->nframes, snk->mod->name, startframe));
	CHNK(_af_print_chunk(i->outc));
	(*(snk->mod->run_push))(snk);

	i->outc->buf = saved_buf;
}

/*
	_AFsimplemodrun
*/
void _AFsimplemodrun_push (_AFmoduleinst *i)
{
	i->outc->nframes = i->inc->nframes;
	(*(i->mod->run))(i->inc, i->outc, i->modspec);
	_AFpush(i, i->outc->nframes);
}

/*
	These macros each declare a module.

	The module uses _AFsimplemodrun_pull and _AFsimplemodrun_push
	(see comments in README.modules).  Thus we only have to define
	one routine that does the actual processing.

	The arguments to the macros are as follows:

	name - name of module
	desc - code for module's "describe" function (see README.modules)
	intype - type of elements of input buffer
	outtype - type of elements of output buffer
	action - action to take in inner loop -- indexes "ip" and "op" with "i"

	modspectype - (MODULEM) this will initialize a pointer "m"
	to this instance's modspec data, which is of type modspectype

	Don't use _MODULE directly.

	The code in "desc" is executed once, after the module is
	initialized.  It can reference "i->modspec" and should modify
	"i->outc->f".  A pointer "f" initialized to "&i->outc->f" is
	emitted prior to "desc"; use this to to keep the code cleaner.

	Note that the generated "run" routine shouldn't set outc->nframes since

	* outc->nframes is set to inc->nframes by _AFsimplemodrun_push
	* inc->nframes is set to outc->nframes by _AFsimplemodrun_pull

	The whole point of the simplified "run" routine is that you don't
	have to worry about push or pull.

	See README.modules for more info on how modules work.
*/

#define _MODULE( name, desc, \
	intype, outtype, chans, preamble, action, postamble )\
void name##run(_AFchunk *inc, _AFchunk *outc, void *modspec)\
{\
	intype *ip = inc->buf;\
	outtype *op = outc->buf;\
	int count = inc->nframes * (chans);\
	int i;\
	\
	preamble;\
	for(i=0; i < count; ++i) \
	action;\
	postamble;\
}\
\
void name##describe(struct _AFmoduleinst *i)\
{\
	_AudioFormat *f = &i->outc->f; \
	desc;\
}\
\
_AFmodule name =\
{ \
	#name,\
	name##describe, \
	AF_NULL, AF_NULL, \
	_AFsimplemodrun_pull, AF_NULL, AF_NULL, \
	_AFsimplemodrun_push, AF_NULL, AF_NULL, \
	name##run, \
	_AFfreemodspec \
};

#define MODULE( name, desc, intype, outtype, action )\
	_MODULE( name, desc, intype, outtype, inc->f.channelCount, , action, )

#define MODULEM( name, desc, intype, outtype, modspectype, action )\
	_MODULE( name, desc, intype, outtype, inc->f.channelCount, \
	modspectype *m = (modspectype *) modspec, action, )

/*
	Byte-order-swapping modules.
*/

#define MODULESWAP(name, type, action) \
MODULE(name, \
	f->byteOrder = (f->byteOrder==AF_BYTEORDER_LITTLEENDIAN) ?\
	AF_BYTEORDER_BIGENDIAN : AF_BYTEORDER_LITTLEENDIAN,\
	type, type,\
	action)

MODULESWAP(swap2, uchar2,
	{ char3u u; uchar1 c; u.uchar2.s0 = ip[i];
	c = u.uchar1.c1; u.uchar1.c1 = u.uchar1.c0; u.uchar1.c0 = c;
	op[i] = u.uchar2.s0; })

MODULESWAP(swap3, real_char3,
	{ char3u u; uchar1 c; u.real_char3_low.c3 = ip[i];
	c = u.uchar1.c3; u.uchar1.c3 = u.uchar1.c1; u.uchar1.c1 = c;
	op[i] = u.real_char3_low.c3; })

MODULESWAP(swap4, uchar4,
	{ char3u u; uchar1 c; u.uchar4.i = ip[i];
	c = u.uchar1.c3; u.uchar1.c3 = u.uchar1.c0; u.uchar1.c0 = c;
	c = u.uchar1.c1; u.uchar1.c1 = u.uchar1.c2; u.uchar1.c2 = c;
	op[i] = u.uchar4.i; })

MODULESWAP(swap8, real_char8,
	{ real_char8 *i8 = &ip[i]; real_char8 *o8 = &op[i];
	o8->c0 = i8->c7;
	o8->c1 = i8->c6;
	o8->c2 = i8->c5;
	o8->c3 = i8->c4;
	o8->c4 = i8->c3;
	o8->c5 = i8->c2;
	o8->c6 = i8->c1;
	o8->c7 = i8->c0; })

/*
	modules for dealing with 3-byte integers
*/

/* convert 0xaabbcc to 0xssaabbcc */
MODULE(real_char3_to_schar3, f /* NOTUSED */,
	real_char3, schar3,
	{
		char3u u;
		u.real_char3_high.c3 = ip[i];
		u.real_char3_high.pad = 0;
		op[i] = u.schar3.i >> 8;
	})

/* convert 0xaabbcc to 0x00aabbcc */
MODULE(real_char3_to_uchar3, f /*NOTUSED*/,
	real_char3, uchar3,
	{
		char3u u;
		u.real_char3_high.c3 = ip[i];
		u.real_char3_high.pad = 0;
		op[i] = u.uchar3.i >> 8;
	})

/* convert 0x??aabbcc to 0xaabbcc */
MODULE(char3_to_real_char3, f /*NOTUSED*/,
	uchar3, real_char3,
	{
		char3u u;
		u.uchar3.i = ip[i];
		op[i] = u.real_char3_low.c3;
	})

/*
	float <--> double ; CASTS
*/

MODULE(float2double, f->sampleFormat = AF_SAMPFMT_DOUBLE,
	float, double, op[i] = ip[i] )
MODULE(double2float, f->sampleFormat = AF_SAMPFMT_FLOAT,
	double, float, op[i] = ip[i] )

/*
	int2floatN - expects 8N-bit 2's comp ints, outputs floats ; CASTS
*/

MODULE(int2float1, f->sampleFormat = AF_SAMPFMT_FLOAT,
	schar1, float, op[i] = ip[i])
MODULE(int2float2, f->sampleFormat = AF_SAMPFMT_FLOAT,
	schar2, float, op[i] = ip[i])
MODULE(int2float3, f->sampleFormat = AF_SAMPFMT_FLOAT,
	schar3, float, op[i] = ip[i])
MODULE(int2float4, f->sampleFormat = AF_SAMPFMT_FLOAT,
	schar4, float, op[i] = ip[i])

/*
	int2doubleN - expects 8N-bit 2's comp ints, outputs doubles ; CASTS
*/

MODULE(int2double1, f->sampleFormat = AF_SAMPFMT_DOUBLE,
	schar1, double, op[i] = ip[i])
MODULE(int2double2, f->sampleFormat = AF_SAMPFMT_DOUBLE,
	schar2, double, op[i] = ip[i])
MODULE(int2double3, f->sampleFormat = AF_SAMPFMT_DOUBLE,
	schar3, double, op[i] = ip[i])
MODULE(int2double4, f->sampleFormat = AF_SAMPFMT_DOUBLE,
	schar4, double, op[i] = ip[i])

/*
	The following modules perform the transformation between one
	pcm mapping and another.

	The modules all use MODULETRANS; some of them also perform
	clipping.

	Use initpcmmod() to create an instance of any of these modules.
	initpcmmod() takes an _PCMInfo describing the desired output
	pcm mapping.
*/

typedef struct pcmmodspec
{
	/* These are the computed parameters of the transformation. */
	double m, b;
	double maxv, minv;

	/* This is what goes in i->outc->f. */
	_PCMInfo output_mapping;
} pcmmodspec;

/*
	initpcmmod
*/
_AFmoduleinst initpcmmod (_AFmodule *mod,
	_PCMInfo *input_mapping, _PCMInfo *output_mapping)
{
	_AFmoduleinst ret = _AFnewmodinst(mod);
	pcmmodspec *m = _af_malloc(sizeof (pcmmodspec));
	ret.modspec = m;

	/* Remember output mapping for use in the describe function. */
	m->output_mapping = *output_mapping;

	/*
		Compute values needed to perform transformation if the module
		being initialized does a transformation..
	*/
	if (input_mapping)
	{
		m->m = output_mapping->slope / input_mapping->slope;
		m->b = output_mapping->intercept - m->m * input_mapping->intercept;
	}

	/* Remember clip values. */
	m->minv = output_mapping->minClip;
	m->maxv = output_mapping->maxClip;
	return ret;
}

#define MODULETRANS( name, xtradesc, intype, outtype, action ) \
MODULEM(name, \
	{ \
		f->pcm = ((pcmmodspec *) i->modspec)->output_mapping; \
		xtradesc; \
	}, \
	intype, outtype, pcmmodspec, \
	action)


MODULETRANS(floattransform, , float, float, op[i]=(m->b + m->m * ip[i]))
MODULETRANS(doubletransform, , double, double, op[i]=(m->b + m->m * ip[i]))

/*
	float2intN_clip - expects floats,
		outputs CLIPped, 8N-bit, transformed 2's comp ints
	double2intN_clip - same deal with doubles
*/

#define TRANS_CLIP(type) \
{\
	double d=(m->b + m->m * ip[i]); \
	op[i] = \
		(((type)((d>(m->maxv)) ? (m->maxv) : ((d<(m->minv))?(m->minv):d)))); \
}

MODULETRANS(float2int1_clip,
	{ f->sampleFormat = AF_SAMPFMT_TWOSCOMP; f->sampleWidth = 8; },
	float, schar1, TRANS_CLIP(schar1))
MODULETRANS(float2int2_clip,
	{ f->sampleFormat = AF_SAMPFMT_TWOSCOMP; f->sampleWidth = 16; },
	float, schar2, TRANS_CLIP(schar2))
MODULETRANS(float2int3_clip,
	{ f->sampleFormat = AF_SAMPFMT_TWOSCOMP; f->sampleWidth = 24; },
	float, schar3, TRANS_CLIP(schar3))
MODULETRANS(float2int4_clip,
	{ f->sampleFormat = AF_SAMPFMT_TWOSCOMP; f->sampleWidth = 32; },
	float, schar4, TRANS_CLIP(schar4))

MODULETRANS(double2int1_clip,
	{ f->sampleFormat = AF_SAMPFMT_TWOSCOMP; f->sampleWidth = 8; },
	double, schar1, TRANS_CLIP(schar1))
MODULETRANS(double2int2_clip,
	{ f->sampleFormat = AF_SAMPFMT_TWOSCOMP; f->sampleWidth = 16; },
	double, schar2, TRANS_CLIP(schar2))
MODULETRANS(double2int3_clip,
	{ f->sampleFormat = AF_SAMPFMT_TWOSCOMP; f->sampleWidth = 24; },
	double, schar3, TRANS_CLIP(schar3))
MODULETRANS(double2int4_clip,
	{ f->sampleFormat = AF_SAMPFMT_TWOSCOMP; f->sampleWidth = 32; },
	double, schar4, TRANS_CLIP(schar4))

/*
	clipping modules - use initpcmmod() to make one of these

	clips to range given as argument to init function.
*/

#define MODULECLIP(name, type)\
MODULEM(name, \
	{ f->pcm = ((pcmmodspec *)i->modspec)->output_mapping; }, \
	type, type, pcmmodspec, \
	{ \
		type d=ip[i]; \
		type min=(type)(m->minv); \
		type max=(type)(m->maxv); \
		op[i] = ((d>max) ? max : ((d<min) ? min : d)); \
	} )

MODULECLIP(clipfloat, float)
MODULECLIP(clipdouble, double)
MODULECLIP(clip1, schar1)
MODULECLIP(clip2, schar2)
MODULECLIP(clip3, schar3)
MODULECLIP(clip4, schar4)

/*
	unsigned2signedN - expects 8N-bit unsigned ints, outputs 2's comp
*/

MODULE(unsigned2signed1,
	{
		double shift = (double) MIN_INT8;
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->pcm.intercept += shift;
		f->pcm.minClip += shift;
		f->pcm.maxClip += shift;
	},
	uchar1, schar1,
	op[i] = ip[i] + MIN_INT8)
MODULE(unsigned2signed2,
	{
		double shift = (double) MIN_INT16;
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->pcm.intercept += shift;
		f->pcm.minClip += shift;
		f->pcm.maxClip += shift;
	},
	uchar2, schar2,
	op[i] = ip[i] + MIN_INT16)
MODULE(unsigned2signed3,
	{
		double shift = (double) MIN_INT24;
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->pcm.intercept += shift;
		f->pcm.minClip += shift;
		f->pcm.maxClip += shift;
	},
	uchar3, schar3,
	op[i] = ip[i] + MIN_INT24)
MODULE(unsigned2signed4,
	{
		double shift = (double) MIN_INT32;
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->pcm.intercept += shift;
		f->pcm.minClip += shift;
		f->pcm.maxClip += shift;
	},
	uchar4, schar4,
	op[i] = ip[i] + MIN_INT32)

/* !! unsigned2signed4 shouldn't work, but it does !! */


/*
	signed2unsignedN - expects 8N-bit 2's comp ints, outputs unsigned
*/

MODULE(signed2unsigned1,
	{
		double shift = -(double) MIN_INT8;
		f->sampleFormat = AF_SAMPFMT_UNSIGNED;
		f->pcm.intercept += shift;
		f->pcm.minClip += shift;
		f->pcm.maxClip += shift;
	},
	schar1, uchar1,
	op[i] = ip[i] - MIN_INT8)
MODULE(signed2unsigned2,
	{
		double shift = -(double) MIN_INT16;
		f->sampleFormat = AF_SAMPFMT_UNSIGNED;
		f->pcm.intercept += shift;
		f->pcm.minClip += shift;
		f->pcm.maxClip += shift;
	},
	schar2, uchar2,
	op[i] = ip[i] - MIN_INT16)
MODULE(signed2unsigned3,
	{
		double shift = -(double) MIN_INT24;
		f->sampleFormat = AF_SAMPFMT_UNSIGNED;
		f->pcm.intercept += shift;
		f->pcm.minClip += shift;
		f->pcm.maxClip += shift;
	},
	schar3, uchar3,
	op[i] = ip[i] - MIN_INT24)
MODULE(signed2unsigned4,
	{
		double shift = -(double) MIN_INT32;
		f->sampleFormat = AF_SAMPFMT_UNSIGNED;
		f->pcm.intercept += shift;
		f->pcm.minClip += shift;
		f->pcm.maxClip += shift;
	},
	schar4, uchar4,
	op[i] = ip[i] - MIN_INT32)

/* !! signed2unsigned4 shouldn't work, but it does !! */


/*
	These convert between different 2's complement integer formats
	with no roundoff/asymmetric errors.  They should also work faster
	than converting integers to floats and back to other integers.

	They are only meant to be used when the input and output integers
	have the default PCM mapping; otherwise, arrangemodules will
	make the conversion go through floating point and these modules
	will not be used.
*/

#define intmap _af_default_signed_integer_pcm_mappings /* shorthand */

MODULE(int1_2, { f->sampleWidth = 16; f->pcm=intmap[2]; },
	schar1, schar2, op[i] = ip[i] << 8)
MODULE(int1_3, { f->sampleWidth = 24; f->pcm=intmap[3]; },
	schar1, schar3, op[i] = ip[i] << 16)
MODULE(int1_4, { f->sampleWidth = 32; f->pcm=intmap[4]; },
	schar1, schar4, op[i] = ip[i] << 24)

MODULE(int2_1, { f->sampleWidth = 8; f->pcm=intmap[1]; },
	schar2, schar1, op[i] = ip[i] >> 8)
MODULE(int2_3, { f->sampleWidth = 24; f->pcm=intmap[3]; },
	schar2, schar3, op[i] = ip[i] << 8)
MODULE(int2_4, { f->sampleWidth = 32; f->pcm=intmap[4]; },
	schar2, schar4, op[i] = ip[i] << 16)

MODULE(int3_1, { f->sampleWidth = 8; f->pcm=intmap[1]; },
	schar3, schar1, op[i] = ip[i] >> 16)
MODULE(int3_2, { f->sampleWidth = 16; f->pcm=intmap[2]; },
	schar3, schar2, op[i] = ip[i] >> 8)
MODULE(int3_4, { f->sampleWidth = 32; f->pcm=intmap[4]; },
	schar3, schar4, op[i] = ip[i] << 8)

MODULE(int4_1, { f->sampleWidth = 8; f->pcm=intmap[1]; },
	schar4, schar1, op[i] = ip[i] >> 24)
MODULE(int4_2, { f->sampleWidth = 16; f->pcm=intmap[2]; },
	schar4, schar2, op[i] = ip[i] >> 16)
MODULE(int4_3, { f->sampleWidth = 24; f->pcm=intmap[3]; },
	schar4, schar3, op[i] = ip[i] >> 8)

#undef intmap

/*
	channel changer modules - convert channels using channel matrix

	The channel matrix is a two-dimensional array of doubles, the rows
	of which correspond to the virtual format, and the columns
	of which correspond to the file format.

	If the channel matrix is null (unspecified), then the default
	behavior occurs (see initchannelchange).

	Internally, the module holds a copy of the matrix in which the
	rows correspond to the output format, and the columns correspond
	to the input format (therefore, if reading==AF_FALSE, the matrix
	is transposed as it is copied).
*/

typedef struct channelchangedata
{
	int	outchannels;
	double	minClip;
	double	maxClip;
	double	*matrix;
} channelchangedata;

/*
	channelchangefree
*/
void channelchangefree (struct _AFmoduleinst *i)
{
	channelchangedata *d = i->modspec;

	assert(d);
	assert(d->matrix);

	free(d->matrix);
	free(d);

	i->modspec = AF_NULL;
}

/*
	channelchangedescribe
*/
void channelchangedescribe (struct _AFmoduleinst *i)
{
	channelchangedata *m = (channelchangedata *) i->modspec;
	i->outc->f.channelCount = m->outchannels;
	i->outc->f.pcm.minClip = m->minClip;
	i->outc->f.pcm.maxClip = m->maxClip;
}

#define CHANNELMOD( name, type, zero_op, action, afteraction ) \
void name##run(_AFchunk *inc, _AFchunk *outc, void *modspec) \
{ \
	type *ip = inc->buf; \
	type *op = outc->buf; \
	double *matrix = ((channelchangedata *)modspec)->matrix; \
	double *m; \
	int frame, inch, outch; \
	\
	for (frame=0; frame < outc->nframes; frame++) \
	{ \
		type *ipsave; \
		\
		m = matrix; \
		ipsave = ip; \
		\
		for (outch = 0; outch < outc->f.channelCount; outch++) \
		{ \
			zero_op; \
			ip = ipsave; \
			\
			for (inch = 0; inch < inc->f.channelCount; inch++) \
				action;\
			\
			afteraction; \
			op++;\
		}\
	}\
}\
\
_AFmodule name =\
{ \
	#name, \
	channelchangedescribe, \
	AF_NULL, AF_NULL, \
	_AFsimplemodrun_pull, AF_NULL, AF_NULL, \
	_AFsimplemodrun_push, AF_NULL, AF_NULL, \
	name##run, \
	channelchangefree \
};

CHANNELMOD(channelchangefloat,  float,  *op = 0.0, *op += *ip++ * *m++, )
CHANNELMOD(channelchangedouble, double, *op = 0.0, *op += *ip++ * *m++, )

#define CHANNELINTMOD(name, type) \
	CHANNELMOD(name, type, \
	double d=0.0, \
	d += *ip++ * *m++, \
	{ \
		double minv=outc->f.pcm.minClip; \
		double maxv=outc->f.pcm.maxClip; \
		*op = (type) ((d>maxv) ? maxv : ((d<minv) ? minv : d)); \
	} )

CHANNELINTMOD(channelchange1, schar1)
CHANNELINTMOD(channelchange2, schar2)
CHANNELINTMOD(channelchange3, schar3)
CHANNELINTMOD(channelchange4, schar4)

/*
	initchannelchange
*/
_AFmoduleinst initchannelchange (_AFmodule *mod,
	double *matrix, _PCMInfo *outpcm,
	int inchannels, int outchannels,
	bool reading)
{
	_AFmoduleinst ret;
	channelchangedata *d;
	int i, j;

	ret = _AFnewmodinst(mod);

	d = _af_malloc(sizeof (channelchangedata));
	ret.modspec = d;
	d->outchannels = outchannels;
	d->minClip = outpcm->minClip;
	d->maxClip = outpcm->maxClip;
	d->matrix = _af_malloc(sizeof (double) * inchannels * outchannels);

	/*
		Set d->matrix to a default matrix if a matrix was not specified.
	*/
	if (!matrix)
	{
		bool special=AF_FALSE;

		/* Handle many common special cases. */

		if (inchannels==1 && outchannels==2)
		{
			static double m[]={1,1};
			matrix=m;
			special=AF_TRUE;
		}
		else if (inchannels==1 && outchannels==4)
		{
			static double m[]={1,1,0,0};
			matrix=m;
			special=AF_TRUE;
		}
		else if (inchannels==2 && outchannels==1)
		{
			static double m[]={.5,.5};
			matrix=m;
			special=AF_TRUE;
		}
		else if (inchannels==2 && outchannels==4)
		{
			static double m[]={1,0,0,1,0,0,0,0};
			matrix=m;
			special=AF_TRUE;
		}
		else if (inchannels==4 && outchannels==1)
		{
			static double m[]={.5,.5,.5,.5};
			matrix=m;
			special=AF_TRUE;
		}
		else if (inchannels==4 && outchannels==2)
		{
			static double m[]={1,0,1,0,0,1,0,1};
			matrix=m;
			special=AF_TRUE;
		}
		else
		{
			/*
				Each input channel from 1 to N
				maps to output channel 1 to N where
				N=min(inchannels, outchannels).
			*/

			for(i=0; i < inchannels; i++)
				for(j=0; j < outchannels; j++)
					d->matrix[j*inchannels + i] =
						(i==j) ? 1.0 : 0.0;
		}

		if (special)
			memcpy(d->matrix, matrix,
				sizeof (double) * inchannels * outchannels);
	}
	/* Otherwise transfer matrix into d->matrix. */
	else
	{
		/* reading: copy matrix */
		if (reading)
		{
			memcpy(d->matrix, matrix, sizeof (double) * inchannels * outchannels);
		}
		/* writing: transpose matrix */
		else
		{
			for (i=0; i < inchannels; i++)
				for (j=0; j < outchannels; j++)
					d->matrix[j*inchannels + i] =
						matrix[i*outchannels + j];
		}
	}

	DEBG(printf("channelchange d->matrix="));
	DEBG(_af_print_channel_matrix(d->matrix, inchannels, outchannels));
	DEBG(printf("\n"));

	return(ret);
}

/* just used here */
typedef struct current_state
{
	_AFmoduleinst *modinst;	/* current mod instance we're creating */
	_AFchunk *inchunk;	/* current input chunk */
	_AFchunk *outchunk;	/* current output chunk */
} current_state;

/*
	addmod is called once per added module instance.  It does the
	work of putting the module instance in the list and assigning
	it an input and output chunk.
*/
void addmod (current_state *current, _AFmoduleinst modinst)
{
	*(current->modinst) = modinst;
	current->modinst->valid = AF_TRUE; /* at this point mod must be valid */

	/* Assign the new module instance an input and an output chunk. */

	current->modinst->inc = current->inchunk;
	current->modinst->outc = current->outchunk;

	/*
		The output chunk has the same format and number of frames
		as input chunk, except in whatever way the 'describe'
		method tells us (see README.modules).
	*/

	*(current->outchunk) = *(current->inchunk);

	if (current->modinst->mod->describe)
		(*current->modinst->mod->describe)(current->modinst);

	/*
		Advance to next module and next chunks.  Note that next
		moduleinst will have this module's out chunk as input.
	*/

	current->modinst++;
	current->inchunk = current->outchunk;
	current->outchunk++;
}

/*
	initfilemods:

	Functions that deal with extended-lifetime file read / file write
	modules and their extended-lifetime rebuffer modules.
	called once in the lifetime of an AFfilehandle.

	If h->access == _AF_READ_ACCESS:

	Create the module which will be the first module in the chain,
	the one which reads the file.  This module does the decompression
	if necessary, or it could just be a PCM file reader.

	If h->access == _AF_WRITE_ACCESS:

	Create the module which will be the last module in the chain,
	the one which writes the file.	This module does the compression
	if necessary, or it could just be a PCM file writer.

	Also creates a rebuffer module for these modules if necessary.
*/
status initfilemods (_Track *track, AFfilehandle h)
{
	int compressionIndex;
	_CompressionUnit *compunit;
	AFframecount chunkframes;

	compressionIndex = _af_compression_index_from_id(track->f.compressionType);
	compunit = &_af_compression[compressionIndex];

	/* Invalidate everything. */

	track->ms.filemodinst.valid = AF_FALSE;
	track->ms.filemod_rebufferinst.valid = AF_FALSE;

	/*
		Seek to beginning of sound data in the track.

		This is needed ONLY for those modules which have to
		read/write some kind of pre-data header or table in the
		sound data chunk of the file (such as aware).  This is NOT
		the seek that sets the file at the beginning of the data.
	*/
	if (h->seekok && af_fseek(h->fh, track->fpos_first_frame, SEEK_SET) < 0)
	{
		_af_error(AF_BAD_LSEEK, "unable to position file handle at beginning of sound data");
		return AF_FAIL;
	}

	/* Create file read/write module. */

	track->filemodhappy = AF_TRUE;

	if (h->access == _AF_READ_ACCESS)
		track->ms.filemodinst =
			(*compunit->initdecompress)(track, h->fh, h->seekok,
				(h->fileFormat==AF_FILE_RAWDATA), &chunkframes);
	else
		track->ms.filemodinst =
			(*compunit->initcompress)(track, h->fh, h->seekok,
				(h->fileFormat==AF_FILE_RAWDATA), &chunkframes);

	if (!track->filemodhappy)
		return AF_FAIL;

	track->ms.filemodinst.valid = AF_TRUE;

	/*
		WHEN DOES THE FILE GET LSEEKED ?

		Somebody sometime has got to lseek the file to the
		beginning of the audio data.  Similarly, somebody
		has to lseek the file at the time of reset or sync.
		Furthermore, we have to make sure that we operate
		correctly if more than one track is being read or written.
		This is handled differently based on whether we are
		reading or writing, and whether the ONE_TRACK_ONLY lseek
		optimization is engaged.

		READING:

		If we are reading, the file needs to be positioned once
		before we start reading and then once per seek.

		If ONE_TRACK_ONLY is not defined, then there can
		be multiple tracks in the file.  Thus any call to
		afReadFrames could cause the file pointer to be
		put anywhere: we can not rely on the file pointer
		tracking only one track in the file, thus we must seek
		to the current position in track N whenever we begin
		an AFreadframes on track N.  Thus the lseek is done in
		afReadFrames.  When a reset occurs (including the initial
		one), we merely set trk->fpos_next_frame, and the next
		afReadFrames will seek the file there before proceeding.

		If ONE_TRACK_ONLY is defined, meaning there can only
		be 1 track in the file, we do not need to ever seek
		during normal sequential operation, because the file
		read module is the only module which ever accesses the
		file after _afOpenFile returns.  In this case, we do
		not need to do the expensive lseek at the beginning of
		every AFreadframes call.  We need only seek once when the
		file is first opened and once when the file is seeked.
		At both of these times, we reset the modules.  So we
		can do the lseek in resetmodules() right after it has
		called all of the modules' reset2 methods.

		WRITING:

		If we are writing, the file needs to be positioned once
		before we start writing and it needs to be positioned
		after every complete sync operation on the file.

		If ONE_TRACK_ONLY is not defined, then there can be
		multiple tracks in the file.  This assumes space for
		the tracks has been preallocated.  Thus any call to
		AFwriteframes could cause the file pointer to be
		put anywhere: we can not rely on the file pointer
		tracking only one track in the file, thus we must seek
		to the current position in track n whenever we begin
		an AFwriteframes on track n.  Thus the lseek is done
		in AFwriteframes.  When we first start, and when a sync
		occurs, we merely set trk->fpos_next_frame, and the next
		AFwriteframes will seek the file there before proceeding.

		If ONE_TRACK_ONLY is defined, meaning there can only
		be 1 track in the file, we do not need to ever seek
		during normal sequential operation, because the file
		write module is the only module which ever accesses
		the file after _AFopenfile returns.  In this case, we
		do not need to do the expensive lseek at the beginning
		of every AFwriteframes call.  We can do the lseek for
		the initial case right here (that's what you see below),
		and we can do the lseek for syncs in _AFsyncmodules right
		after it has called all of the modules' sync2 methods.

		One annoying exceptional case is _AFeditrate, which
		can get called at any time.  But it saves and restores
		the file position at its beginning and end, so it's
		no problem.

		WHY THE F(*#&@ DON'T YOU JUST HAVE MULTIPLE FILE DESCRIPTORS?

		The obviously and blatantly better way to do this would be
		to simply open one fd per track and then all the problems
		go away!  Too bad we offer afOpenFD() in the API, which
		makes it impossible for the AF to get more than 1 fd
		for the file.  afOpenFD() is unfortunately used far
		more often than afOpenFile(), so the benefit of doing
		the optimization the "right" way in the cases where
		afOpenFile() are used are not currently too great.
		But one day we will have to phase out afOpenFD().
		Too bad, it seemed like such a great idea when we put
		it in.
	*/

#ifdef ONE_TRACK_ONLY
	if (h->access == _AF_WRITE_ACCESS)
	{
		if (h->seekok && af_fseek(h->fh, track->fpos_next_frame, SEEK_SET) < 0)
		{
			_af_error(AF_BAD_LSEEK,
				"unable to position write ptr at first data frame");
			return AF_FAIL;
		}
	}
#endif

	/* Create its rebuffer module. */

	if (compunit->needsRebuffer)
	{
		/* We assume the following for now. */
		assert(compunit->nativeSampleFormat == AF_SAMPFMT_TWOSCOMP);
		assert(compunit->nativeSampleWidth == 16);

		if (h->access == _AF_WRITE_ACCESS)
			track->ms.filemod_rebufferinst =
				initint2rebufferv2f(chunkframes*track->f.channelCount,
					compunit->multiple_of);
		else
			track->ms.filemod_rebufferinst =
				initint2rebufferf2v(chunkframes*track->f.channelCount,
					compunit->multiple_of);

		track->ms.filemod_rebufferinst.valid = AF_TRUE;
	}
	else
		track->ms.filemod_rebufferinst.valid = AF_FALSE;

	/*
		These modules should not get freed until the file handle
		is destroyed (i.e. the file is closed).
	*/

	track->ms.filemodinst.free_on_close = AF_TRUE;
	track->ms.filemod_rebufferinst.free_on_close = AF_TRUE;

	return AF_SUCCEED;
}

/*
	addfilereadmods: called once per setup of the modules
	for a given AFfilehandle
*/
status addfilereadmods (current_state *current, _Track *track, AFfilehandle h)
{
	assert(track->ms.filemodinst.valid);

	/* Fail in case code is broken and NDEBUG is defined. */
	if (!track->ms.filemodinst.valid)
		return AF_FAIL;

	addmod(current, track->ms.filemodinst);
	if (track->ms.filemod_rebufferinst.valid)
		addmod(current, track->ms.filemod_rebufferinst);

	return AF_SUCCEED;
}

/*
	addfilewritemods is called once per setup of the modules
	for a given AFfilehandle.
*/
status addfilewritemods (current_state *current, _Track *track, AFfilehandle h)
{
	assert(track->ms.filemodinst.valid);

	/* Fail in case code is broken and NDEBUG is defined. */
	if (!track->ms.filemodinst.valid)
		return(AF_FAIL);

	if (track->ms.filemod_rebufferinst.valid)
		addmod(current, track->ms.filemod_rebufferinst);

	addmod(current, track->ms.filemodinst);

	return(AF_SUCCEED);
}

/*
	disposefilemods: called once in the lifetime of an AFfilehandle
*/
status disposefilemods (_Track *track)
{
	if (track->ms.filemodinst.valid &&
		track->ms.filemodinst.mod->free)
		(*track->ms.filemodinst.mod->free)(&track->ms.filemodinst);

	track->ms.filemodinst.valid = AF_FALSE;

	if (track->ms.filemod_rebufferinst.valid &&
		track->ms.filemod_rebufferinst.mod->free)
		(*track->ms.filemod_rebufferinst.mod->free)(&track->ms.filemod_rebufferinst);

	track->ms.filemod_rebufferinst.valid = AF_FALSE;

	return AF_SUCCEED;
}

/*
	useAP: rate conversion AP decision maker and warner and kludger
*/
bool useAP (double inrate, double outrate, double *inratep, double *outratep)
{
	bool instandard =
		(inrate==8000 || inrate==11025 || inrate==16000 ||
		inrate==22050 || inrate==32000 || inrate==44100 ||
		inrate==48000);
	bool outstandard =
		(outrate==8000 || outrate==11025 || outrate==16000 ||
		outrate==22050 || outrate==32000 || outrate==44100 ||
		outrate==48000);
	bool incodec;
	bool outcodec;

	incodec = (inrate==_AF_SRATE_CODEC || inrate==(long)_AF_SRATE_CODEC);
	outcodec = (outrate==_AF_SRATE_CODEC || outrate==(long)_AF_SRATE_CODEC);

	*inratep = inrate;
	*outratep = outrate;

	if (instandard && outstandard) return AF_TRUE;
	if (incodec && outstandard && outrate != 8000.00)
	{
		_af_error(AF_WARNING_CODEC_RATE,
			"WARNING using input rate 8 kHz instead of %.30g Hz "
			"to allow high-quality rate conversion",
			inrate);
		*inratep = 8000.00;
		return AF_TRUE;
	}
	if (instandard && inrate != 8000.00 && outcodec)
	{
		_af_error(AF_WARNING_CODEC_RATE,
			"WARNING using output rate 8 kHz instead of %.30g Hz "
			"to allow high-quality rate conversion",
			outrate);
		*outratep = 8000.00;
		return AF_TRUE;
	}

	if (!instandard && !outstandard)
		_af_error(AF_WARNING_RATECVT,
			"WARNING using lower quality rate conversion due to "
			"rates %.30g and %.30g -- "
			"output file may contain audible artifacts",
			inrate, outrate);
	else if (!instandard)
		_af_error(AF_WARNING_RATECVT,
			"WARNING using lower quality rate conversion due to "
			"input rate %.30g -- "
			"output file may contain audible artifacts",
			inrate);
	else	/* !outstandard */
		_af_error(AF_WARNING_RATECVT,
			"WARNING using lower quality rate conversion due to "
			"output rate %.30g -- "
			"output file may contain audible artifacts",
			outrate);

	return AF_FALSE;
}

/*
	initrateconvertmods handles the extended-life rate conversion
	module and its extended-life rebuffer module called once in the
	lifetime of an AFfilehandle.
*/
void initrateconvertmods (bool reading, _Track *track)
{
	/* no rate conversion initially */
	track->ms.rateconvertinst.valid = AF_FALSE;
	track->ms.rateconvert_rebufferinst.valid = AF_FALSE;
}

void disposerateconvertmods (_Track *);

/* XXXmpruett rate conversion is disabled for now */
#if 0
/*
	addrateconvertmods: called once per setup of the modules
	for a given AFfilehandle
*/
void addrateconvertmods (current_state *current, int nchannels,
	double inrate, double outrate,
	bool reading, _Track *track)
{
	AFframecount inframes, outframes;

	/* Check if we are no longer rate converting. */
	if (inrate == outrate)
	{
		disposerateconvertmods(track);
		track->ratecvt_filter_params_set = AF_FALSE; /* XXX HACK */
	}
	else
	{
		/*
			We need new rateconverter if we didn't have one
			or if rate has changed or rate conversion params
			have changed.
		*/
		if (!track->ms.rateconvertinst.valid ||
			inrate != track->ms.rateconvert_inrate ||
			outrate != track->ms.rateconvert_outrate ||
			track->ratecvt_filter_params_set /* HACK */)
		{
			bool usingAP = useAP(inrate, outrate, &inrate, &outrate);

			disposerateconvertmods(track);
			track->ratecvt_filter_params_set = AF_FALSE; /* HACK */

			if (usingAP)
			{
				track->ms.rateconvertinst = InitAFRateConvert(inrate, outrate,
				nchannels,
				track->taper, track->dynamic_range,
				&inframes, &outframes,
				track, reading);

				if (!reading)
					track->ms.rateconvert_rebufferinst =
						initfloatrebufferv2f(inframes*nchannels, AF_FALSE);
				else
					track->ms.rateconvert_rebufferinst =
						initfloatrebufferf2v(outframes*nchannels, AF_FALSE);

				track->ms.rateconvertinst.valid = AF_TRUE;
				track->ms.rateconvert_rebufferinst.valid = AF_TRUE;
			}
			else
			{
				track->ms.rateconvertinst = initpolyratecvt(track,
					inrate, outrate,
					nchannels, reading);

				track->ms.rateconvertinst.valid = AF_TRUE;
				track->ms.rateconvert_rebufferinst.valid = AF_FALSE;
			}

			track->ms.rateconvert_inrate = inrate;
			track->ms.rateconvert_outrate = outrate;

			track->ms.rateconvertinst.free_on_close = AF_TRUE;
			track->ms.rateconvert_rebufferinst.free_on_close = AF_TRUE;
		}

		/* Add the rate conversion modules. */

		if (!reading && track->ms.rateconvert_rebufferinst.valid)
			addmod(current, track->ms.rateconvert_rebufferinst);

		addmod(current, track->ms.rateconvertinst);

		if (reading && track->ms.rateconvert_rebufferinst.valid)
			addmod(current, track->ms.rateconvert_rebufferinst);
	}
}

/*
	disposerateconvertmods is called once in the lifetime of an
	AFfilehandle.
*/
void disposerateconvertmods (_Track *track)
{
	/*
		Neither module is necessarily valid--there could have been
		an error, or the modules could possibly never have been set up.
	*/
	if (track->ms.rateconvertinst.valid &&
		track->ms.rateconvertinst.mod->free)
	{
		(*track->ms.rateconvertinst.mod->free)
			(&track->ms.rateconvertinst);
	}

	track->ms.rateconvertinst.valid = AF_FALSE;

	if (track->ms.rateconvert_rebufferinst.valid &&
		track->ms.rateconvert_rebufferinst.mod->free)
	{
		(*track->ms.rateconvert_rebufferinst.mod->free)
			(&track->ms.rateconvert_rebufferinst);
	}

	track->ms.rateconvert_rebufferinst.valid = AF_FALSE;
}
#endif /* XXXmpruett rate conversion is disabled for now */

/* -------------------------------------------------------------- */

/* The stuff in this section is used by arrangemodules(). */

_AFmodule *unsigned2signed[5] =
{
	NULL,
	&unsigned2signed1, &unsigned2signed2,
	&unsigned2signed3, &unsigned2signed4
};

_AFmodule *signed2unsigned[5] =
{
	NULL,
	&signed2unsigned1, &signed2unsigned2,
	&signed2unsigned3, &signed2unsigned4
};

_AFmodule *swapbytes[9] =
{
	NULL, NULL, &swap2, &swap3, &swap4,
	NULL, NULL, NULL, &swap8
};

/* don't forget int24_fmt is really 24 bits right-justified in 32 bits */

typedef enum format_code
{
	int8_fmt,
	int16_fmt,
	int24_fmt,
	int32_fmt,
	float_fmt,
	double_fmt
} format_code;

#define isinteger(fc) ((fc) <= int32_fmt)
#define isfloating(fc) ((fc) >= float_fmt)

/*
	get_format_code
*/
format_code get_format_code (_AudioFormat *fmt)
{
	if (fmt->sampleFormat == AF_SAMPFMT_FLOAT)
		return float_fmt;
	if (fmt->sampleFormat == AF_SAMPFMT_DOUBLE)
		return double_fmt;

	if (fmt->sampleFormat == AF_SAMPFMT_TWOSCOMP ||
		fmt->sampleFormat == AF_SAMPFMT_UNSIGNED)
	{
		switch (_af_format_sample_size_uncompressed(fmt, AF_FALSE))
		{
			case 1: return int8_fmt;
			case 2: return int16_fmt;
			case 3: return int24_fmt;
			case 4: return int32_fmt;
		}
	}

	assert(0);
	return -1;
}

_AFmodule *to_flt[6] =
{
	&int2float1, &int2float2, &int2float3, &int2float4,
	NULL, &double2float
};

_AFmodule *to_dbl[6] =
{
	&int2double1, &int2double2, &int2double3, &int2double4,
	&float2double, NULL
};

_AFmodule *clip[6] =
{
	&clip1, &clip2, &clip3, &clip4,
	&clipfloat, &clipdouble
};

_AFmodule *channelchanges[6] =
{
	&channelchange1, &channelchange2, &channelchange3, &channelchange4,
	&channelchangefloat, &channelchangedouble
};

/* indices are of type format_code: matrix[infmtcode][outfmtcode] */
_AFmodule *convertmatrix[6][6] =
{
	/* TO:
	{
		int8_fmt, int16_fmt,
		int24_fmt, int32_fmt,
		float_fmt, double_fmt
	}
	*/

	/* FROM int8_fmt */
	{
		NULL, &int1_2,
		&int1_3, &int1_4,
		&int2float1, &int2double1
	},

	/* FROM int16_fmt */
	{
		&int2_1, NULL,
		&int2_3, &int2_4,
		&int2float2, &int2double2
	},

	/* FROM int24_fmt */
	{
		&int3_1, &int3_2,
		NULL, &int3_4,
		&int2float3, &int2double3
	},

	/* FROM int32_fmt */
	{
		&int4_1, &int4_2,
		&int4_3, NULL,
		&int2float4, &int2double4
	},

	/* FROM float_fmt */
	{
		&float2int1_clip, &float2int2_clip,
		&float2int3_clip, &float2int4_clip,
		NULL, &float2double
	},

	/* FROM double_fmt */
	{
		&double2int1_clip, &double2int2_clip,
		&double2int3_clip, &double2int4_clip,
		&double2float, NULL
	}
};

_PCMInfo *intmappings[6] =
{
	&_af_default_signed_integer_pcm_mappings[1],
	&_af_default_signed_integer_pcm_mappings[2],
	&_af_default_signed_integer_pcm_mappings[3],
	&_af_default_signed_integer_pcm_mappings[4],
	AF_NULL, AF_NULL
};

/*
	trivial_int_clip
*/
bool trivial_int_clip (_AudioFormat *f, format_code code)
{
	return (intmappings[code] != NULL &&
		f->pcm.minClip == intmappings[code]->minClip   &&
		f->pcm.maxClip == intmappings[code]->maxClip);
}

/*
	trivial_int_mapping
*/
bool trivial_int_mapping (_AudioFormat *f, format_code code)
{
	return (intmappings[code] != NULL &&
		f->pcm.slope == intmappings[code]->slope &&
		f->pcm.intercept == intmappings[code]->intercept);
}

/*
	arrangemodules decides which modules to use and creates instances
	of them.
*/
status arrangemodules (_AFfilehandle *h, _Track *track)
{
	bool reading = (h->access == _AF_READ_ACCESS);

	current_state current;

	bool rateconverting, transforming;
	bool already_clipped_output, already_transformed_output;

	int insampbytes, outsampbytes;
	int chans;

	format_code infc, outfc;

	/*
		in and out are the formats at the start and end of the
		chain of modules, respectively.
	*/

	_AudioFormat in, out;

	/* in==FILE, out==virtual (user) */
	if (reading)
	{
		in = track->f;
		out = track->v;
	}
	/* in==virtual (user), out==FILE */
	else
	{
		in = track->v;
		out = track->f;
	}

	infc = get_format_code(&in);
	outfc = get_format_code(&out);

	/* flags */

	rateconverting = (in.sampleRate != out.sampleRate);

	/*
		throughout routine:

		current.modinst points to current module
		current.inchunk points to current in chunk, always outchunk-1
		current.outchunk points to current out chunk

		The addmod() function does most of the work.  It calls the
		"describe" module function, during which a module looks
		at inc->f and writes the format it will output in outc->f.
	*/

	current.modinst = track->ms.module;

	current.inchunk = track->ms.chunk;
	current.outchunk = track->ms.chunk + 1;

	current.inchunk->f = in;

	/*
		max # of modules that could be needed together
		may need to change this if you change this function
	*/
	#define MAX_MODULES 17

	/* Actually arrange the modules.  Call addmod() to add one. */

	/* Add file reader and possibly a decompressor. */

	if (reading)
		if (AF_FAIL == addfilereadmods(&current, track, h))
			return AF_FAIL;

	/* Make data native-endian. */

	if (in.byteOrder != _AF_BYTEORDER_NATIVE)
	{
		int bytes_per_samp = _af_format_sample_size_uncompressed(&in, !reading);

		if (bytes_per_samp > 1 &&
			in.compressionType == AF_COMPRESSION_NONE)
		{
			assert(swapbytes[bytes_per_samp]);
			addmod(&current, _AFnewmodinst(swapbytes[bytes_per_samp]));
		}
		else
			in.byteOrder = _AF_BYTEORDER_NATIVE;
	}

	/* Handle nasty 3-byte input cases. */

	insampbytes = _af_format_sample_size_uncompressed(&in, AF_FALSE);

	if (isinteger(infc) && insampbytes == 3)
	{
		if (reading || in.compressionType != AF_COMPRESSION_NONE)
		{
			/*
				We're reading 3-byte ints from a file.
				At this point stretch them to 4-byte ints
				by sign-extending or adding a zero-valued
				most significant byte.  We could also
				be reading/writing 3-byte samples output
				from a decompressor.
			*/
			if (in.sampleFormat == AF_SAMPFMT_UNSIGNED)
				addmod(&current, _AFnewmodinst(&real_char3_to_uchar3));
			else
				addmod(&current, _AFnewmodinst(&real_char3_to_schar3));
		}
		else /* writing, non-compressed */
		{
			/*
				We're processing 3-byte ints from the
				user, which come in as sign-extended
				4-byte quantities.  How convenient:
				this is what we want.
			*/
		}
	}

	/* Make data signed. */

	if (in.sampleFormat == AF_SAMPFMT_UNSIGNED)
	{
		addmod(&current, _AFnewmodinst(unsigned2signed[insampbytes]));
	}

	/* Standardize pcm mapping of "in" and "out". */

	/*
		Since they are used to compute transformations in the
		inner section of this routine (inside of sign conversion),
		we need in.pcm and out.pcm in terms of AF_SAMPFMT_TWOSCOMP
		numbers.
	*/
	in.pcm = current.inchunk->f.pcm; /* "in" is easy */

	if (out.sampleFormat == AF_SAMPFMT_UNSIGNED) /* "out": undo the unsigned shift */
	{
		double shift = intmappings[outfc]->minClip;
		out.pcm.intercept += shift;
		out.pcm.minClip += shift;
		out.pcm.maxClip += shift;
	}

	/* ------ CLIP user's input samples if necessary */

	if (in.pcm.minClip < in.pcm.maxClip && !trivial_int_clip(&in, infc))
		addmod(&current, initpcmmod(clip[infc], AF_NULL, &in.pcm));

	/*
		At this point, we assume we can have doubles, floats,
		and 1-, 2-, and 4-byte signed integers on the input and
		on the output (or 4-byte integers with 24 significant
		(low) bits, int24_fmt).

		Now we handle rate conversion and pcm transformation.
	*/

	/* If rate conversion will happen, we must have floats. */
	/*
		This may result in loss of precision.  This bug must be
		fixed eventually.
	*/
	if (rateconverting && infc != float_fmt)
	{
		addmod(&current, _AFnewmodinst(to_flt[infc]));
		infc = float_fmt;
	}

	/*
		We must make sure the output samples will get clipped
		to SOMETHING reasonable if we are rateconverting.
		The user cannot possibly expect to need to clip values
		just because rate conversion is on.
	*/

	if (out.pcm.minClip >= out.pcm.maxClip && rateconverting)
	{
		out.pcm.minClip = out.pcm.intercept - out.pcm.slope;
		out.pcm.maxClip = out.pcm.intercept + out.pcm.slope;
	}

	already_clipped_output = AF_FALSE;
	already_transformed_output = AF_FALSE;

	/*
		We need to perform a transformation (in floating point)
		if the input and output PCM mappings are different.

		The only exceptions are the trivial integer conversions
		(i.e., full-range integers of one # of bytes to full-range
		integers to another # of bytes).
	*/

	transforming = (in.pcm.slope != out.pcm.slope ||
		in.pcm.intercept != out.pcm.intercept) &&
		!(trivial_int_mapping(&in, infc) &&
		trivial_int_mapping(&out,outfc));

	/*
		If we have ints on input and the user is performing a
		change of mapping other than a trivial one, we must go
		to floats or doubles.
	*/

	if (isinteger(infc) && transforming)
	{
		/*
			Use doubles if either the in or out format has
			that kind of precision.
		*/
		if (infc == int32_fmt ||
			outfc == double_fmt || outfc == int32_fmt)
		{
			addmod(&current, _AFnewmodinst(to_dbl[infc]));
			infc = double_fmt;
		}
		else
		{
			addmod(&current, _AFnewmodinst(to_flt[infc]));
			infc = float_fmt;
		}
	}

	DEBG(printf("arrangemodules  in="); _af_print_audioformat(&in););
	DEBG(printf("arrangemodules out="); _af_print_audioformat(&out););
	DEBG(printf("arrangemodules transforming=%d\n", transforming));
	DEBG(printf("arrangemodules infc=%d outfc=%d\n", infc, outfc));

	/*
		invariant:

		At this point, if infc is an integer format, then we are
		not rate converting, nor are we perfoming any change of
		mapping other than possibly a trivial int->int conversion.
	*/

	/* ----- convert format infc to format outfc */

	/* change channels if appropriate now */

	if (in.channelCount != out.channelCount &&
		(infc > outfc || (infc==outfc && out.channelCount < in.channelCount)))
	{
		addmod(&current,
			initchannelchange(channelchanges[infc],
			track->channelMatrix, &in.pcm,
			in.channelCount, out.channelCount,
			reading));
		chans = out.channelCount;
	}
	else
		chans = in.channelCount;

	/* Transform floats if appropriate now. */

	if (transforming &&
		infc==double_fmt && isfloating(outfc))
	{
		addmod(&current, initpcmmod(&doubletransform, &in.pcm, &out.pcm));
	}

#if 0 /* XXXmpruett */
	/*
		Handle rate conversion (will do the right thing if
		not rate converting).
	*/

	addrateconvertmods(&current, chans, in.sampleRate, out.sampleRate, reading, track);
#endif

	/* Add format conversion, if needed */

	if (convertmatrix[infc][outfc])
	{
		/*
			for float/double -> int conversions, the module
			we use here also does the transformation and
			clipping.

			We use initpcmmod() in any case because it is harmless
			for the other modules in convertmatrix[][].
		*/
		if (isfloating(infc) && isinteger(outfc)) /* "float"->"int" */
		{
			already_clipped_output = AF_TRUE;
			already_transformed_output = AF_TRUE;
		}
		addmod(&current, initpcmmod(convertmatrix[infc][outfc],
			&in.pcm, &out.pcm));
	}

	/* Transform floats if appropriate now. */

	if (transforming && !already_transformed_output && infc != double_fmt)
	{
		if (outfc==double_fmt)
			addmod(&current, initpcmmod(&doubletransform,
				&in.pcm, &out.pcm));
		else if (outfc==float_fmt)
			addmod(&current, initpcmmod(&floattransform,
				&in.pcm, &out.pcm));
	}

	/* Change channels if appropriate now. */

	if (in.channelCount != out.channelCount &&
		(outfc > infc || (infc==outfc && in.channelCount < out.channelCount)))
	{
		addmod(&current,
			initchannelchange(channelchanges[outfc],
				track->channelMatrix, &out.pcm,
				in.channelCount, out.channelCount,
				reading));
	}

	/* ------ CLIP user's output samples if needed */

	if (!already_clipped_output)
	{
		if (out.pcm.minClip < out.pcm.maxClip &&
			!trivial_int_clip(&out, outfc))
		{
			addmod(&current, initpcmmod(clip[outfc], NULL, &out.pcm));
		}
	}

	/* Make data unsigned if neccessary. */

	outsampbytes = _af_format_sample_size_uncompressed(&out, AF_FALSE);

	if (out.sampleFormat == AF_SAMPFMT_UNSIGNED)
		addmod(&current, _AFnewmodinst(signed2unsigned[outsampbytes]));

	/* Handle nasty 3-byte output cases. */

	if (isinteger(outfc) && outsampbytes == 3)
	{
		if (!reading || out.compressionType != AF_COMPRESSION_NONE)
		{
			/*
				We're writing 3-byte ints into a file.
				We have 4-byte ints.  Squish them to
				3 by truncating the high byte off.
				we could also be reading/writing ints
				into a compressor.  note this works for
				signed and unsigned, and has to.
			*/
			addmod(&current, _AFnewmodinst(&char3_to_real_char3));
		}
		else /* reading, not compressed */
		{
			/*
				We're reading 3-byte ints into the
				user's buffer.

				The user expects
				1. 4-byte sign-extended ints (3 bytes
				sign extended in 4 bytes) or
				2. 4-byte unsigned ints (3 bytes in 4 bytes).

				How convenient: this is just what we have.
			*/
		}
	}

	if (out.byteOrder != _AF_BYTEORDER_NATIVE)
	{
		int bytes_per_samp = _af_format_sample_size_uncompressed(&out, reading);

		if (bytes_per_samp > 1 && out.compressionType == AF_COMPRESSION_NONE)
		{
			assert(swapbytes[bytes_per_samp]);
			addmod(&current, _AFnewmodinst(swapbytes[bytes_per_samp]));
		}
	}

	/* Add file writer, possibly a compressor. */

	if (!reading)
		if (AF_FAIL == addfilewritemods(&current, track, h))
			return(AF_FAIL);

	/* Now all modules are arranged! */

	track->ms.nmodules = current.modinst - track->ms.module;

#ifdef UNLIMITED_CHUNK_NVFRAMES
	/*
		OPTIMIZATION: normally, when we set up the modules, AFreadframes
		and AFwriteframes must pull and push chunks of size at most
		_AF_ATOMIC_NVFRAMES.

		For the simplest configurations of modules (1 module, no
		compression), no buffering at all needs to be done by the
		module system.	In these cases, afReadFrames/afWriteFrames
		can pull/push as many virtual frames as they want
		in one call.  This flag tells tells afReadFrames and
		afWriteFrames whether they can do so.

		Note that if this flag is set, file modules cannot rely
		on the intermediate working buffer which _AFsetupmodules
		usually allocates for them in their input or output chunk
		(for reading or writing, respectively).  This is why if
		we are reading/writing compressed data, this optimization
		is turned off.

		There are warnings to this effect in the pcm
		(uncompressed) file read/write module.	If you want to
		apply this optimization to other types, be sure to put
		similar warnings in the code.
	*/
	if (track->ms.nmodules == 1 &&
		track->v.compressionType == AF_COMPRESSION_NONE &&
		track->f.compressionType == AF_COMPRESSION_NONE)
		track->ms.mustuseatomicnvframes = AF_FALSE;
	else
		track->ms.mustuseatomicnvframes = AF_TRUE;
#else
	track->ms.mustuseatomicnvframes = AF_TRUE;
#endif

	return AF_SUCCEED;
}

/*
	disposemodules will free old buffers and free old modules, except
	those marked with free_on_close.

	The modules existing before we dispose them could be:

	1. none (we may have only called _AFinitmodules and not _AFsetupmodules)
	2. some invalid PARTIALLY ALLOCATED ones (e.g. the last _AFsetupmodules
	had an error) or
	3. a perfectly valid set of modules.

	disposemodules will deal with all three cases.
*/
void disposemodules (_Track *track)
{
	if (track->ms.module)
	{
		int i;

		for (i=0; i < MAX_MODULES; i++)
		{
			_AFmoduleinst *mod = &track->ms.module[i];

#ifdef AF_DEBUG
			if (!mod->valid && i < track->ms.nmodules)
				printf("disposemodules: WARNING in-range invalid module found '%s'\n", mod->mod->name);
#endif

			if (mod->valid && !mod->free_on_close && mod->mod->free)
			{
				(*mod->mod->free)(mod);
				mod->valid = AF_FALSE;
			}
		}

		free(track->ms.module);
		track->ms.module = AF_NULL;
	}
	track->ms.nmodules = 0;

	if (track->ms.chunk)
	{
		free(track->ms.chunk);
		track->ms.chunk = AF_NULL;
	}

	if (track->ms.buffer)
	{
		int i;
		for (i=0; i < (MAX_MODULES+1); i++)
		{
			if (track->ms.buffer[i] != AF_NULL)
			{
				free(track->ms.buffer[i]);
				track->ms.buffer[i] = AF_NULL;
			}
		}
		free(track->ms.buffer);
		track->ms.buffer = AF_NULL;
	}
}

/*
	resetmodules: see advanced section in README.modules for more info
*/
status resetmodules (_AFfilehandle *h, _Track *track)
{
	int i;

	/*
		We should already have called _AFsetupmodules.
		(Actually this is called from the end of _AFsetupmodules
		but whatever).
	*/

	assert(!track->ms.modulesdirty);

	/* Assume all is well with track. */
	track->filemodhappy = AF_TRUE;

	CHNK(printf("resetmodules running reset1 routines\n"));

	/* Reset all modules. */
	for (i=track->ms.nmodules-1; i >= 0; i--)
	{
		/* reset1 */
		if (track->ms.module[i].mod->reset1 != AF_NULL)
			(*track->ms.module[i].mod->reset1)(&track->ms.module[i]);
	}

	/* Clear out frames2ignore here; the modules will increment it. */
	track->frames2ignore = 0;

	if (!track->filemodhappy)
		return AF_FAIL;

	CHNK(printf("resetmodules running reset2 routines\n"));

	for (i=0; i < track->ms.nmodules; i++)
	{
		/* reset2 */
		if (track->ms.module[i].mod->reset2 != AF_NULL)
			(*track->ms.module[i].mod->reset2)(&track->ms.module[i]);
	}

	CHNK(printf("resetmodules completed\n"));

	if (!track->filemodhappy)
		return AF_FAIL;

#ifdef ONE_TRACK_ONLY
	/*
		For an explanation of this, see the comment in
		initfilemods which explains how and when the file is
		lseek'ed.
	*/
	if (h->seekok)
		if (lseek(h->fd, track->fpos_next_frame, SEEK_SET) < 0)
		{
			_af_error(AF_BAD_LSEEK,
				"unable to position read pointer at next data frame");
			return AF_FAIL;
		}
#endif

	return AF_SUCCEED;
}

/*
	_AFsyncmodules
*/
status _AFsyncmodules (AFfilehandle h, _Track *track)
{
	int i;

	/* We should already have called _AFsetupmodules. */
	assert(!track->ms.modulesdirty);

	/* Assume all is well with track. */
	track->filemodhappy = AF_TRUE;

	CHNK(printf("_AFsyncmodules running sync1 routines\n"));

	/* Sync all modules. */
	for(i=track->ms.nmodules-1; i >= 0; i-- )
	{
		/* sync1 */
		if (AF_NULL != track->ms.module[i].mod->sync1)
			(*track->ms.module[i].mod->sync1)(&track->ms.module[i]);
	}

	if (!track->filemodhappy)
		return AF_FAIL;

	CHNK(printf("_AFsyncmodules running sync2 routines\n"));

	for (i=0; i < track->ms.nmodules; i++)
	{
		/* sync2 */
		if (AF_NULL != track->ms.module[i].mod->sync2)
			(*track->ms.module[i].mod->sync2)(&track->ms.module[i]);
	}

	CHNK(printf("_AFsyncmodules completed\n"));

	if (!track->filemodhappy)
		return AF_FAIL;

#ifdef ONE_TRACK_ONLY
	/*
		For an explanation of this, see the comment in
		initfilemods which explains how and when the file is
		lseek'ed.
	*/
	if (h->seekok)
		if (lseek( h->fd, track->fpos_next_frame, SEEK_SET) < 0 )
		{
			_af_error(AF_BAD_LSEEK,
			"unable to position write ptr at next data frame");
			return(AF_FAIL);
		}
#endif

	return AF_SUCCEED;
}

/*
	_AFsetupmodules:
	- frees any old modules, chunks, and buffers
	- looks at the input and output format and sets up a whole new
	set of input and output modules (using arrangemodules())
	- assigns those modules chunks
	- allocates buffers and assigns the buffers to the chunks
	- initializes various track fields pertaining to the module system

	It returns AF_FAIL on any kind of error.

	It sets modulesdirty to AF_FALSE if it was able to clean the
	modules (although an error still could have occurred after
	cleaning them).
*/
status _AFsetupmodules (AFfilehandle h, _Track *track)
{
	_AFmoduleinst *modules;
	_AFchunk *chunks;
	void **buffers;
	int maxbufsize, bufsize, i;
	double rateratiof2v, fframepos;

	/*
		The purpose of this function is to "clean" the modules:

		* All of the fields in trk->ms are completely set
		and valid.

		* track->totalvframes and track->next[fv]frame are set
		and valid and trk->modulesdirty will be set to AF_FALSE
		if this function succeeds.

		This function also resets the modules on files open for read.
		it will return AF_FAIL if either cleaning the modules fails,
		or this reset fails.

		The comments will tell you which part does what.
	*/

	/*
		NOTE: we cannot trust any value in track->ms until we
		have called disposemodules(), at which time things are
		cleared to reasonable "zero" values.

		It is possible for track->ms to be in an illegal state
		at this point, if the last _AFsetupmodules failed with
		an error.

		We can trust track->totalvframes and track->next[fv]frame
		because they are only modified after successfully building
		the modules.
	*/

	/*
		Disallow compression in virtual format for now.
	*/
	if (track->v.compressionType != AF_COMPRESSION_NONE)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED,
			"library does not support compression in virtual format yet");
		return AF_FAIL;
	}

	/*
		Check that virtual compression parameters are ok.
	*/
	{
		int idx = _af_compression_index_from_id(track->v.compressionType);
		if ((*_af_compression[idx].fmtok)(&track->v) == AF_FALSE)
		{
			return AF_FAIL;
		}
	}

	/*
		track->nextvframe and track->nextfframe:

		At this point, only track->nextvframe contains useful
		information, since track->nextfframe may be swayed by
		currently buffered frames.

		Also track->nextvframe is currently in the scale of the
		old sampling rate, not the new one we are setting up.

		So at this point we remember where we are in the file
		(in floating point) in terms of the file sampling rate.

		We will use this later in this function to set both
		track->nextfframe (for reading) and track->nextvframe
		(for reading and writing).

		We must be careful to use the old rates, not the ones
		in track->{f,v}.
	*/

	/* If modules have been set up at all */
	if (track->ms.old_v_rate > 0)
	{
		assert(track->ms.old_f_rate > 0);
		rateratiof2v = track->ms.old_f_rate / track->ms.old_v_rate;
		fframepos = track->nextvframe * rateratiof2v;
	}
	else
		/* We start at frame zero. */
		fframepos = 0;

	/*
		Dispose the existing modules (except extended-life ones).

		See the function for info on what the module state could
		be at this time.
	*/

	disposemodules(track);

	/*
		Here we allocate the highest number of module instances
		(and chunks) chained together we could possibly need.

		This is how the chunks are used:

		module[n]'s input chunk is chunk[n]
		module[n]'s output chunk is chunk[n+1]

		chunk[n]'s buffer, if it is not the user's buffer, is buffer[n].

		For reading chunk[0] is not usually used.
		For writing chunk[nmodules] is not usually used.

		We allocate a buffer for chunk[0] on reading and
		chunk[nmodules] when writing because the file reading
		or file writing module, if it does compression or
		decompression, may need extra space in which to place
		the result of its processing before reading or writing it.

		Also note that chunk[0].f and chunk[nmodules].f are used in
		arrangemodules().
	*/
	modules = _af_malloc(sizeof (_AFmoduleinst) * MAX_MODULES);
	if (modules == AF_NULL)
		return AF_FAIL;
	for (i=0; i < MAX_MODULES; i++)
		modules[i].valid = AF_FALSE;

	chunks = _af_malloc(sizeof (_AFchunk) * (MAX_MODULES+1));
	if (chunks == AF_NULL)
		return AF_FAIL;

	buffers = _af_malloc(sizeof (void *) * (MAX_MODULES+1));
	if (buffers == AF_NULL)
		return AF_FAIL;
	/*
		It is very important to initialize each buffers[i] to NULL;
		dispose frees them all if !NULL.
	*/

	for (i=0; i < (MAX_MODULES+1); i++)
		buffers[i] = AF_NULL;

	track->ms.module = modules;
	/*
		nmodules is a bogus value here, set just for sanity
		(in case of broken code).
	*/

	track->ms.nmodules = 0;
	track->ms.chunk = chunks;
	track->ms.buffer = buffers;

	/*
		Figure out the best modules to use to convert the
		data and initialize instances of those modules.
		Fills "track->ms.module" and most of "track->ms.chunk"
		arrays (all but the buffers) as it goes.  Sets
		"track->ms.nmodules" As a side benefit, this function
		also leaves information about the data format at each
		stage in the "f" field of each chunk.
	*/
	if (arrangemodules(h, track) == AF_FAIL)
	{
		/*
			At this point the modules are in an incompletely
			initialized and probably illegal state.  nmodules
			could be meaningful or not.  Things are nasty.

			But as long as any API call that uses the
			modules calls _AFsetupmodules() first (which
			then calls disposemodules(), which can handle
			this nastiness), we can restore the modules to
			a sane initial state and things will be ok.
		*/

		return AF_FAIL;
	}

	/*
		At this point modules and nmodules are almost completely
		filled in (modules aren't actually connected to one
		another), but buffer[n] and chunk[n].buf are still in
		a null state.

		track->totalvframes and track->next[fv]frame have not yet been
		set to a valid state.
	*/

	/*
		Now go through the modules:

		1. Connect up the source/sink fields properly.
		2. Use the information left in the _AudioFormat field
		of each chunk by setupmodules() along with the
		"max_pull"/"max_push" module function to figure
		out the biggest buffer size that could be needed.
	*/

	/* filemod reports error here */
	track->filemodhappy = AF_TRUE;
	maxbufsize = 0;

	if (h->access == _AF_READ_ACCESS)
	{
		track->ms.chunk[track->ms.nmodules].nframes = _AF_ATOMIC_NVFRAMES;

		for (i=track->ms.nmodules-1; i >= 0; i--)
		{
			_AFchunk *inc = &track->ms.chunk[i];
			_AFchunk *outc = &track->ms.chunk[i+1];

			/* check bufsize needed for current output chunk */

			bufsize = outc->nframes * _af_format_frame_size(&outc->f, AF_TRUE);
			if (bufsize > maxbufsize)
			maxbufsize = bufsize;

			if (i != 0)
			{
				/* Connect source pointer for this module. */

				track->ms.module[i].u.pull.source = &track->ms.module[i-1];
			}

			/*
				Determine inc->nframes from outc->nframes.
				If the max_pull function is present, we use it,
				otherwise we assume module does no weird
				buffering or rate conversion.
			*/
			if (track->ms.module[i].mod->max_pull)
				(*track->ms.module[i].mod->max_pull)(&track->ms.module[i]);
			else
				inc->nframes = outc->nframes;
		}

		if (!track->filemodhappy)
			return AF_FAIL;

		/*
			Check bufsize needed for filemod's input chunk
			(intermediate buffer) based on an uncompressed
			(output chunk) framesize.
		*/

		{
			_AFmoduleinst *filemod = &track->ms.module[0];
			bufsize = filemod->inc->nframes *
				_af_format_frame_size(&filemod->outc->f, AF_TRUE);
			if (bufsize > maxbufsize)
				maxbufsize = bufsize;
		}
	}
	else
	{
		track->ms.chunk[0].nframes = _AF_ATOMIC_NVFRAMES;

		for (i=0; i < track->ms.nmodules; i++)
		{
			_AFchunk *inc = &track->ms.chunk[i];
			_AFchunk *outc = &track->ms.chunk[i+1];

			/* Check bufsize needed for current input chunk. */

			bufsize = inc->nframes * _af_format_frame_size(&inc->f, AF_TRUE);
			if (bufsize > maxbufsize)
				maxbufsize = bufsize;

			if (i != track->ms.nmodules-1)
			{
				/* Connect sink pointer. */

				track->ms.module[i].u.push.sink = &track->ms.module[i+1];
			}

			/*
				Determine outc->nframes from inc->nframes.
				If the max_push function is present, we use it,
				otherwise we assume module does no weird
				buffering or rate conversion.
			*/
			if (track->ms.module[i].mod->max_push)
				(*track->ms.module[i].mod->max_push)(&track->ms.module[i]);
			else
				outc->nframes = inc->nframes;
		}

		if (!track->filemodhappy)
			return AF_FAIL;

		/*
			Check bufsize needed for filemod's output chunk
			(intermediate buffer) based on an uncompressed (input
			chunk) framesize.
		*/

		{
			_AFmoduleinst *filemod = &track->ms.module[track->ms.nmodules-1];
			bufsize = filemod->outc->nframes *
			_af_format_frame_size(&filemod->inc->f, AF_TRUE);
			if (bufsize > maxbufsize)
				maxbufsize = bufsize;
		}
	}

	/*
		At this point everything is totally set up with the
		modules except that the chunk buffers have not been
		allocated, and thus buffer[n] and chunk[n].buf have
		not been set.  But now we know how big they should be
		(maxbufsize).

		track->totalvframes and track->next[fv]frame have not
		yet been set to a valid state.
	*/
	DEBG(printf("_AFsetupmodules: maxbufsize=%d\n", maxbufsize));

	/*
		One of these will get overwritten to point to user's
		buffer.  The other one will be allocated below (for file
		read/write module).
	*/

	track->ms.chunk[track->ms.nmodules].buf = AF_NULL;
	track->ms.chunk[0].buf = AF_NULL;

	/*
		One of these will be allocated for the file read/write
		module The other will be completely unused.
	*/
	track->ms.buffer[track->ms.nmodules] = AF_NULL;
	track->ms.buffer[0] = AF_NULL;

	/*
		Now that we know how big buffers have to be, allocate
		buffers and assign them to the module instances.

		Note that track->ms.chunk[nmodules].buf (reading) or
		track->ms.chunk[0].buf (writing) will get overwritten
		in _AFreadframes or _AFwriteframes to point to the
		user's buffer.

		We allocate a buffer for track->ms.chunk[0].buf (reading)
		or track->ms.chunk[nmodules].buf (writing) not because
		it is needed for the modules to work, but as a working
		buffer for the file reading / file writing modules.

		Also note that some modules may change their inc->buf or
		outc->buf to point to something internal to the module
		before calling their source or sink.

		So module code must be careful not to assume that a buffer
		address will not change.  Only for chunk[nmodules]
		(reading) or chunk[0] (writing) is such trickery
		disallowed.
	*/

	if (h->access == _AF_READ_ACCESS)
		for (i=track->ms.nmodules-1; i >= 0; i--)
		{
			if ((track->ms.buffer[i] = _af_malloc(maxbufsize)) == AF_NULL)
				return AF_FAIL;
			track->ms.chunk[i].buf = track->ms.buffer[i];
		}
	else
		for (i=1; i <= track->ms.nmodules; i++)
		{
			if ((track->ms.buffer[i] = _af_malloc(maxbufsize)) == AF_NULL)
				return AF_FAIL;
			track->ms.chunk[i].buf = track->ms.buffer[i];
		}

	/*
		Hooray!  The modules are now in a completely valid state.
		But we can't set track->ms.modulesdirty to AF_FALSE yet...

		track->totalvframes and track->next[fv]frame have not yet been
		set to a valid state.
	*/
	if (h->access == _AF_READ_ACCESS)
	{
		/*
			Set total number of virtual frames based on new rate.
		*/
		if (track->totalfframes == -1)
			track->totalvframes = -1;
		else
			track->totalvframes = track->totalfframes *
				(track->v.sampleRate / track->f.sampleRate);

		/*
			track->nextvframe and track->nextfframe:

			Currently our only indication of where we were
			in the file is the variable fframepos, which
			contains (in floating point) our offset in file
			frames based on the old track->nextvframe.

			Now we get as close as we can to that original
			position, given the new sampling rate.
		*/

		track->nextfframe = (AFframecount) fframepos;
		track->nextvframe = (AFframecount) (fframepos * (track->v.sampleRate / track->f.sampleRate));

		/*
			Now we can say the module system is in a
			clean state.  Any errors we get from here on
			are reported but not critical.
		*/

		track->ms.modulesdirty = AF_FALSE;

		/* Set up for next time. */
		track->ms.old_f_rate = track->f.sampleRate;
		track->ms.old_v_rate = track->v.sampleRate;

		/*
			Now we reset all the modules.

			If we are here because the user did afSeekFrame,
			the actual seek will be performed here.
			Otherwise this reset will set things up so that
			we are at the same file offset we were at before
			(or as close as possible given a change in
			rate conversion).
		*/

		/* Report error, but we're still clean. */
		if (AF_SUCCEED != resetmodules(h, track))
			return AF_FAIL;
	}
	/* Handle the case of _AF_WRITE_ACCESS. */
	else
	{
		/*
			Don't mess with track->nextfframe or
			track->totalfframes.  Scale virtual frame position
			relative to old virtual position.
		*/

		track->nextvframe = track->totalvframes =
			(AFframecount) (fframepos * (track->v.sampleRate / track->f.sampleRate));

		/*
			Now we can say the module system is in a
			clean state.  Any errors we get from here on
			are reported but not critical.
		*/

		track->ms.modulesdirty = AF_FALSE;

		/* Set up for next time. */
		track->ms.old_f_rate = track->f.sampleRate;
		track->ms.old_v_rate = track->v.sampleRate;
	}

	DEBG(_af_print_filehandle(h));

#ifdef DEBUG
	for (i=track->ms.nmodules-1; i >= 0; i--)
	{
		_AFmoduleinst *inst = &track->ms.module[i];
		inst->dump = AF_TRUE;
	}

	{
		/* Print format summary. */

		printf("%s ->\n", (h->access == _AF_READ_ACCESS) ? "file" : "user");
		for (i=0; i < track->ms.nmodules; i++)
		{
			_AFmoduleinst *inst = &track->ms.module[i];
			_af_print_audioformat(&inst->inc->f);
			printf(" -> %s(%d) ->\n", inst->mod->name, i);
		}
		_af_print_audioformat(&track->ms.chunk[track->ms.nmodules].f);
		printf(" -> %s\n", (h->access != _AF_READ_ACCESS) ? "file" : "user");
	}
#endif

	/*
		If we get here, then not only are the modules clean, but
		whatever we did after the modules became clean succeeded.
		So we gloat about our success.
	*/
	return AF_SUCCEED;
}

/*
	_AFinitmodules: this routine sets the initial value of the module-
	related fields of the track when the track is first created.

	It also initializes the file read or file write modules.
	See README.modules for info on this.

	Set "modulesdirty" flag on each track, so that the first
	read/write/seek will set up the modules.
*/
status _AFinitmodules (AFfilehandle h, _Track *track)
{
	track->channelMatrix = NULL;

	/* HACK: see private.h for a description of this hack */
	track->taper = 10;
	track->dynamic_range = 100;
	track->ratecvt_filter_params_set = AF_TRUE;

	track->ms.nmodules = 0;
	track->ms.module = NULL;
	track->ms.chunk = NULL;
	track->ms.buffer = NULL;

	track->ms.modulesdirty = AF_TRUE;

	track->ms.filemodinst.valid = AF_FALSE;
	track->ms.filemod_rebufferinst.valid = AF_FALSE;

	track->ms.rateconvertinst.valid = AF_FALSE;
	track->ms.rateconvert_rebufferinst.valid = AF_FALSE;

	/* bogus value in case of bad code */
	track->ms.mustuseatomicnvframes = AF_TRUE;

	/* old_f_rate and old_v_rate MUST be set to <= 0 here. */
	track->ms.old_f_rate = -1;
	track->ms.old_v_rate = -1;

	/*
		Initialize extended-life file read or file write modules.
	*/
	if (AF_FAIL == initfilemods(track, h))
		return AF_FAIL;

	/*
		Initialize extended-life rate convert modules (to NULL).
	*/
	initrateconvertmods(h->access == _AF_READ_ACCESS, track);

	/*
		NOTE: Only now that we have initialized filemods is
		track->totalfframes guaranteed to be ready.  (The unit
		cannot always tell how many frames are in the file.)
	*/

	/* totalfframes could be -1. */
	track->totalvframes = track->totalfframes;
	track->nextvframe = 0;
	track->frames2ignore = 0;

	return AF_SUCCEED;
}

/*
	_AFfreemodules:
	called once when filehandle is being freed
	opposite of initmodules
	free all modules, even the active ones
*/
void _AFfreemodules (_Track *track)
{
	disposemodules(track);
	disposefilemods(track);
#if 0 /* XXXmpruett rate conversion is deactivated for now */
	disposerateconvertmods(track);
#endif
}
