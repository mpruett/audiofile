/*
	Audio File Library
	Copyright (C) 1998-2000, Michael Pruett <michael@68k.org>

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
	next.h

	This file contains headers and constants related to the NeXT/Sun
	.snd audio file format.
*/

#include "afinternal.h"

#ifndef NEXTSND_H
#define NEXTSND_H

enum
{
	_AU_FORMAT_UNSPECIFIED = 0,
	_AU_FORMAT_MULAW_8 = 1,		/* CCITT G.711 mu-law 8-bit */
	_AU_FORMAT_LINEAR_8 = 2,
	_AU_FORMAT_LINEAR_16 = 3,
	_AU_FORMAT_LINEAR_24 = 4,
	_AU_FORMAT_LINEAR_32 = 5,
	_AU_FORMAT_FLOAT = 6,
	_AU_FORMAT_DOUBLE = 7,
	_AU_FORMAT_INDIRECT = 8,
	_AU_FORMAT_NESTED = 9,
	_AU_FORMAT_DSP_CORE = 10,
	_AU_FORMAT_DSP_DATA_8 = 11,	/* 8-bit fixed point */
	_AU_FORMAT_DSP_DATA_16 = 12,	/* 16-bit fixed point */
	_AU_FORMAT_DSP_DATA_24 = 13,	/* 24-bit fixed point */
	_AU_FORMAT_DSP_DATA_32 = 14,	/* 32-bit fixed point */
	_AU_FORMAT_DISPLAY = 16,
	_AU_FORMAT_MULAW_SQUELCH = 17,	/* 8-bit mu-law, squelched */
	_AU_FORMAT_EMPHASIZED = 18,
	_AU_FORMAT_COMPRESSED = 19,
	_AU_FORMAT_COMPRESSED_EMPHASIZED = 20,
	_AU_FORMAT_DSP_COMMANDS = 21,
	_AU_FORMAT_DSP_COMMANDS_SAMPLES = 22,
	_AU_FORMAT_ADPCM_G721 = 23,	/* CCITT G.721 ADPCM 32 kbits/s */
	_AU_FORMAT_ADPCM_G722 = 24,	/* CCITT G.722 ADPCM */
	_AU_FORMAT_ADPCM_G723_3 = 25,	/* CCITT G.723 ADPCM 24 kbits/s */
	_AU_FORMAT_ADPCM_G723_5 = 26,	/* CCITT G.723 ADPCM 40 kbits/s */
	_AU_FORMAT_ALAW_8 = 27,		/* CCITT G.711 a-law */
	_AU_FORMAT_AES = 28,
	_AU_FORMAT_DELTA_MULAW_8 = 29
};

#define _AF_NEXT_NUM_COMPTYPES 2

bool _af_next_recognize (AFvirtualfile *fh);
status _af_next_read_init (AFfilesetup, AFfilehandle);
status _af_next_write_init (AFfilesetup, AFfilehandle);
status _af_next_update (AFfilehandle);
AFfilesetup _af_next_complete_setup (AFfilesetup);

#endif
