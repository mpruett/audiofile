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
	afinternal.h

	This file defines the internal structures for the Audio File Library.
*/

#ifndef AFINTERNAL_H
#define AFINTERNAL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include "audiofile.h"
#include "af_vfs.h"
#include "error.h"

typedef int bool;
#define AF_TRUE (1)
#define AF_FALSE (0)

typedef int status;
#define AF_SUCCEED (0)
#define AF_FAIL (-1)

struct _codec
{
	AFframecount (*readFrames) (const AFfilehandle file, int track,
		void *samples, const int count);
	AFframecount (*writeFrames) (const AFfilehandle file, int track,
		void *samples, const int count);
};

typedef struct _Compression
{
	int     type;   /* symbolic constant */
	char    *name;  /* human-readable name */

	struct _codec   *codec;
} _Compression;

typedef union AFPVu
{
	long	l;
	double	d;
	void	*v;
} AFPVu;

typedef struct _SuppMiscInfo
{
	int	type;	/* AF_MISC_... */
	int	count;	/* 0 = unlimited */
} _SuppMiscInfo;

typedef struct _InstParamInfo
{
	int	id;
	int	type;
	char	*name;
	AFPVu	defaultValue;
} _InstParamInfo;

typedef struct _MarkerSetup
{
	int	id;
	char	*name, *comment;
} _MarkerSetup;

typedef struct _Marker
{
	short		id;
	unsigned long	position;
	char		*name, *comment;
} _Marker;

typedef struct _Loop
{
	int	id;
	int	mode;	/* AF_LOOP_MODE_... */
	int	count;	/* how many times the loop is played */
	int	beginMarker, endMarker;
	int	trackid;
} _Loop;

typedef struct _PCMInfo
{
	double	slope, intercept, minClip, maxClip;
} _PCMInfo;

typedef struct _AudioFormat
{
	double	sampleRate;		/* sampling rate in Hz */
	int	sampleFormat;		/* AF_SAMPFMT_... */
	int	sampleWidth;		/* sample width in bits */
	int	byteOrder;		/* AF_BYTEORDER_... */

	_PCMInfo	pcm;		/* parameters of PCM data */

	int	channelCount;		/* number of channels */

	int	compressionType;	/* AF_COMPRESSION_... */
	void	*compressionParams;	/* NULL if no compression */
} _AudioFormat;

/* modules */
struct _AFmoduleinst;
struct _AFchunk;

typedef void (*_AFfnpmod) (struct _AFmoduleinst *i);
typedef void (*_AFfnpsimplemod) (struct _AFchunk *inc,
	struct _AFchunk *outc, void *modspec);

typedef struct _AFmodule
{
	char *name;
	_AFfnpmod describe;
	_AFfnpmod max_pull;
	_AFfnpmod max_push;
	_AFfnpmod run_pull;
	_AFfnpmod reset1;
	_AFfnpmod reset2;
	_AFfnpmod run_push;
	_AFfnpmod sync1;
	_AFfnpmod sync2;
	_AFfnpsimplemod run;
	_AFfnpmod free;
} _AFmodule;

typedef struct _AFchunk
{
	void		*buf;		/* chunk data */
	AFframecount	nframes;	/* # of frames in chunk */
	_AudioFormat	f;		/* format of data in chunk */
} _AFchunk;

typedef struct _AFmoduleinst
{
	_AFchunk *inc, *outc;
	void *modspec;
	union
	{
		struct { struct _AFmoduleinst *source; } pull;
		struct { struct _AFmoduleinst *sink; } push;
	} u;
	_AFmodule *mod;
	bool free_on_close;	/* AF_TRUE=don't free module until close */
	bool valid;	/* internal use only */
#ifdef AF_DEBUG		/* these are set in _AFsetupmodules */
	int margin;	/* margin for printing of CHNK messages */
	bool dump;	/* whether to dump chunks */
#endif
} _AFmoduleinst;

/* information private to module routines */
typedef struct _AFmodulestate
{
	bool modulesdirty;
	int nmodules;

	/* See comment at very end of arrangemodules(). */
	bool mustuseatomicnvframes;

	/* previous rates before user changed them */
	double old_f_rate, old_v_rate;

	_AFchunk *chunk;
	_AFmoduleinst *module;

	/* array of pointers to buffers, one for each module */
	void **buffer;

	/* These modules have extended lifetimes. */

	/* file read / write */
	_AFmoduleinst filemodinst;

	/* file module's rebuffer */
	_AFmoduleinst filemod_rebufferinst;

	/* rate conversion */
	_AFmoduleinst rateconvertinst;

	/* old rates */
	double rateconvert_inrate, rateconvert_outrate;

	/* rate conversion's rebuffer */
	_AFmoduleinst rateconvert_rebufferinst;
} _AFmodulestate;

typedef struct _Track
{
	int	id;	/* usually AF_DEFAULT_TRACKID */

	_AudioFormat	f, v;	/* file and virtual audio formats */

	double	*channelMatrix;

	int	markerCount;
	_Marker	*markers;

	bool		hasAESData;	/* Is AES nonaudio data present? */
	unsigned char	aesData[24];	/* AES nonaudio data */

	AFframecount	totalfframes;		/* frameCount */
	AFframecount	nextfframe;		/* currentFrame */
	AFframecount	frames2ignore;
	AFfileoffset	fpos_first_frame;	/* dataStart */
	AFfileoffset	fpos_next_frame;
	AFfileoffset	fpos_after_data;
	AFframecount	totalvframes;
	AFframecount	nextvframe;
	AFfileoffset	data_size;		/* trackBytes */

	_AFmodulestate ms;

	double	taper, dynamic_range;
	bool ratecvt_filter_params_set;

	bool filemodhappy;
} _Track;

typedef struct _TrackSetup
{
	int	id;

	_AudioFormat	f;

	bool	rateSet, sampleFormatSet, sampleWidthSet, byteOrderSet,
		channelCountSet, compressionSet, aesDataSet, markersSet,
		dataOffsetSet, frameCountSet;

	int		markerCount;
	_MarkerSetup	*markers;

	off_t	dataOffset;
	off_t	frameCount;
} _TrackSetup;

typedef struct _LoopSetup
{
	int	id;
} _LoopSetup;

typedef struct _InstrumentSetup
{
	int	id;

	int		loopCount;
	_LoopSetup	*loops;
	bool		loopSet;
} _InstrumentSetup;

typedef struct _Instrument
{
	int	id;

	int	loopCount;
	_Loop	*loops;

	AFPVu	*values;
} _Instrument;

typedef struct _Miscellaneous
{
	int		id;
	int		type;
	int		size;

	void		*buffer;

	AFfileoffset	position;	/* offset within the miscellaneous chunk */
} _Miscellaneous;

typedef struct _MiscellaneousSetup
{
	int	id;
	int	type;
	int	size;
} _MiscellaneousSetup;

typedef struct _AFfilesetup
{
	int	valid;

	int	fileFormat;

	bool	trackSet, instrumentSet, miscellaneousSet;

	int			trackCount;
	_TrackSetup		*tracks;

	int			instrumentCount;
	_InstrumentSetup	*instruments;

	int			miscellaneousCount;
	_MiscellaneousSetup	*miscellaneous;
} _AFfilesetup;

typedef struct _AFfilehandle
{
	int	valid;	/* _AF_VALID_FILEHANDLE */
	int	access;	/* _AF_READ_ACCESS or _AF_WRITE_ACCESS */

	bool	seekok;

        AFvirtualfile	*fh;

	int	fileFormat;

	int	trackCount;
	_Track	*tracks;

	int		instrumentCount;
	_Instrument	*instruments;

	int		miscellaneousCount;
	_Miscellaneous	*miscellaneous;

	void	*formatSpecific;	/* format-specific data */
} _AFfilehandle;

enum
{
	AIFC_VERSION_1 = 0xa2805140
};

enum
{
	_AF_VALID_FILEHANDLE = 38212,
	_AF_VALID_FILESETUP = 38213
};

enum
{
	_AF_READ_ACCESS = 1,
	_AF_WRITE_ACCESS = 2
};

/* NeXT/Sun sampling rate */
#define _AF_SRATE_CODEC (8012.8210513)

#endif
