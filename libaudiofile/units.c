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
	units.c

	This file contains the file format units.
*/

#include "audiofile.h"
#include "afinternal.h"
#include "units.h"

#include "raw.h"
#include "aiff.h"
#include "wave.h"
#include "next.h"

#include "compression.h"

#include "modules/pcm.h"
#include "modules/g711.h"

extern _InstParamInfo _af_aiff_inst_params[];
extern _InstParamInfo _af_wave_inst_params[];

extern int _af_raw_compression_types[];
extern int _af_aiffc_compression_types[];
extern int _af_next_compression_types[];
extern int _af_wave_compression_types[];

_Unit _af_units[_AF_NUM_UNITS] =
{
	{
		AF_FILE_RAWDATA,
		"Raw Data", "Raw Sound Data", "raw",
		AF_TRUE, NULL, _af_raw_complete_setup,
		{_af_raw_recognize, _af_raw_read_init},
		{_af_raw_write_init, NULL, NULL},
		AF_SAMPFMT_TWOSCOMP, 16,
		_AF_RAW_NUM_COMPTYPES,
		_af_raw_compression_types,
		0,	/* maximum marker count */
		0,	/* maximum instrument count */
		0,	/* maxium number of loops per instrument */
		0, NULL,
	},
	{
		AF_FILE_AIFFC,
		"AIFF-C", "AIFF-C File Format", "aifc",
		AF_TRUE, _af_aifc_get_version, _af_aiff_complete_setup,
		{_af_aifc_recognize, _af_aiff_read_init},
		{_af_aiff_write_init, _af_aiff_instparam_valid, _af_aiff_update},
		AF_SAMPFMT_TWOSCOMP, 16,
		_AF_AIFFC_NUM_COMPTYPES,
		_af_aiffc_compression_types,
		65535,	/* maximum marker count */
		1,	/* maximum instrument count */
		2,	/* maximum number of loops per instrument */
		_AF_AIFF_NUM_INSTPARAMS,
		_af_aiff_inst_params
	},
	{
		AF_FILE_AIFF,
		"AIFF", "Audio Interchange File Format", "aiff",
		AF_TRUE, NULL, _af_aiff_complete_setup,
		{_af_aiff_recognize, _af_aiff_read_init},
		{_af_aiff_write_init, _af_aiff_instparam_valid, _af_aiff_update},
		AF_SAMPFMT_TWOSCOMP, 16,
		0,	/* supported compression types */
		NULL,
		65535,	/* maximum marker count */
		1,	/* maximum instrument count */
		2,	/* maximum number of loops per instrument */
		_AF_AIFF_NUM_INSTPARAMS,
		_af_aiff_inst_params
	},
	{
		AF_FILE_NEXTSND,
		"NeXT .snd/Sun .au", "NeXT .snd/Sun .au Format", "next",
		AF_TRUE, NULL, _af_next_complete_setup,
		{_af_next_recognize, _af_next_read_init},
		{_af_next_write_init, NULL, _af_next_update},
		AF_SAMPFMT_TWOSCOMP, 16,
		_AF_NEXT_NUM_COMPTYPES,
		_af_next_compression_types,
		0,	/* maximum marker count */
		0,	/* maximum instrument count */
		0,	/* maximum number of loops per instrument */
		0, NULL
	},
	{
		AF_FILE_WAVE,
		"MS RIFF WAVE", "Microsoft RIFF WAVE Format", "wave",
		AF_TRUE, NULL, _af_wave_complete_setup,
		{_af_wave_recognize, _af_wave_read_init},
		{_af_wave_write_init, _af_wave_instparam_valid, _af_wave_update},
		AF_SAMPFMT_TWOSCOMP, 16,
		_AF_WAVE_NUM_COMPTYPES,
		_af_wave_compression_types,
		AF_NUM_UNLIMITED,	/* maximum marker count */
		1,			/* maximum instrument count */
		AF_NUM_UNLIMITED,	/* maximum number of loops per instrument */
		_AF_WAVE_NUM_INSTPARAMS,
		_af_wave_inst_params
	}
};

_CompressionUnit _af_compression[_AF_NUM_COMPRESSION] =
{
	{
		AF_COMPRESSION_NONE,
		AF_TRUE,
		"none",	/* label */
		"none",	/* short name */
		"not compressed",
		1.0,
		AF_SAMPFMT_TWOSCOMP, 16,
		AF_FALSE,	/* needsRebuffer */
		AF_FALSE,	/* multiple_of */
		_af_pcm_format_ok,
		_AFpcminitcompress, _AFpcminitdecompress
	},
	{
		AF_COMPRESSION_G711_ULAW,
		AF_TRUE,
		"ulaw",	/* label */
		"ulaw",	/* shortname */
		"CCITT G.711 u-law",
		2.0,
		AF_SAMPFMT_TWOSCOMP, 16,
		AF_FALSE,	/* needsRebuffer */
		AF_FALSE,	/* multiple_of */
		_af_g711_format_ok,
		_AFg711initcompress, _AFg711initdecompress
	},
	{
		AF_COMPRESSION_G711_ALAW,
		AF_TRUE,
		"alaw",	/* label */
		"alaw",	/* short name */
		"CCITT G.711 A-law",
		2.0,
		AF_SAMPFMT_TWOSCOMP, 16,
		AF_FALSE,	/* needsRebuffer */
		AF_FALSE,	/* multiple_of */
		_af_g711_format_ok,
		_AFg711initcompress, _AFg711initdecompress
	}
};
