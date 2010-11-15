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
#include <stdbool.h>
#include "audiofile.h"
#include "af_vfs.h"
#include "error.h"

#ifdef __cplusplus
#include <string>

extern "C" {
#endif

typedef int status;
#define AF_SUCCEED (0)
#define AF_FAIL (-1)

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

	bool packed : 1;

#ifdef __cplusplus
	size_t bytesPerSample(bool stretch3to4) const;
	size_t bytesPerFrame(bool stretch3to4) const;
	size_t bytesPerSample() const;
	size_t bytesPerFrame() const;
	bool isInteger() const;
	bool isSigned() const;
	bool isUnsigned() const;
	bool isFloat() const;
	bool isCompressed() const;
	bool isUncompressed() const;
	bool isPacked() const { return packed; }
	std::string description() const;
#endif
} _AudioFormat;

typedef struct Module Module;
typedef struct ModuleState ModuleState;

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

	ModuleState *ms;

	double	taper, dynamic_range;
	bool ratecvt_filter_params_set;

	bool filemodhappy;

#ifdef __cplusplus
	void print();
#endif
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

	AFfileoffset	dataOffset;
	AFframecount	frameCount;
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

	char	*fileName;

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
	_AF_VALID_FILEHANDLE = 38212,
	_AF_VALID_FILESETUP = 38213
};

enum
{
	_AF_READ_ACCESS = 1,
	_AF_WRITE_ACCESS = 2
};

/* The following are tokens for compression parameters in PV lists. */
enum
{
	_AF_FRAMES_PER_BLOCK = 700,		/* type: long */
	_AF_BLOCK_SIZE = 701,			/* type: long */
	_AF_MS_ADPCM_NUM_COEFFICIENTS = 800,	/* type: long */
	_AF_MS_ADPCM_COEFFICIENTS = 801		/* type: array of int16_t[2] */
};

/* NeXT/Sun sampling rate */
#define _AF_SRATE_CODEC (8012.8210513)

#ifdef __cplusplus
}
#endif

#endif
