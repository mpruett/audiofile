/*
	Audio File Library
	Copyright (C) 2000, Michael Pruett <michael@68k.org>

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
	units.h

	This file defines the internal _Unit and _CompressionUnit
	structures for the Audio File Library.
*/

#ifndef UNIT_H
#define UNIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "audiofile.h"
#include "afinternal.h"

typedef struct _Unit
{
	int	fileFormat;	/* AF_FILEFMT_... */
	char	*name;		/* a 2-3 word name of the file format */
	char	*description;	/* a more descriptive name for the format */
	char	*label;		/* a 4-character label for the format */
	bool	implemented;	/* if implemented */

	int (*getversion) (AFfilehandle handle);
	AFfilesetup (*completesetup) (AFfilesetup setup);

	struct
	{
		bool (*recognize) (AFvirtualfile *fh);
		status (*init) (AFfilesetup, AFfilehandle);
	} read;

	struct
	{
		status (*init) (AFfilesetup, AFfilehandle);
		bool (*instparamvalid) (AFfilehandle, AUpvlist, int);
		status (*update) (AFfilehandle);
	} write;

	int defaultSampleFormat;
	int defaultSampleWidth;

	int compressionTypeCount;
	int *compressionTypes;

	int markerCount;

	int instrumentCount;
	int loopPerInstrumentCount;

	int instrumentParameterCount;
	_InstParamInfo *instrumentParameters;
} _Unit;

typedef struct _CompressionUnit
{
	int	compressionID;	/* AF_COMPRESSION_... */
	bool	implemented;
	char	*label;		/* 4-character (approximately) label */
	char	*shortname;	/* short name in English */
	char	*name;		/* long name in English */
	double	squishFactor;
	int	nativeSampleFormat;	/* AF_SAMPFMT_... */
	int	nativeSampleWidth;	/* sample width in bits */
	bool	needsRebuffer;	/* if there are chunk boundary requirements */
	bool	multiple_of;	/* can accept any multiple of chunksize */
	bool	(*fmtok) (_AudioFormat *format);

	_AFmoduleinst (*initcompress) (_Track *track, AFvirtualfile *fh,
		bool seekok, bool headerless, AFframecount *chunkframes);
	_AFmoduleinst (*initdecompress) (_Track *track, AFvirtualfile *fh,
		bool seekok, bool headerless, AFframecount *chunkframes);
} _CompressionUnit;

#define _AF_NUM_UNITS 5
#define _AF_NUM_COMPRESSION 3

#endif /* UNIT_H */
