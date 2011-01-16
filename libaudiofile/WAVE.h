/*
	Audio File Library
	Copyright (C) 1998, Michael Pruett <michael@68k.org>
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
	WAVE.h

	This file contains structures and constants pertinent to the RIFF
	WAVE file format.
*/

#ifndef WAVE_H
#define WAVE_H

#include "FileHandle.h"
#include <stdint.h>

/* These constants are from RFC 2361. */
enum
{
	WAVE_FORMAT_UNKNOWN = 0x0000,	/* Microsoft Unknown Wave Format */
	WAVE_FORMAT_PCM = 0x0001,	/* Microsoft PCM Format */
	WAVE_FORMAT_ADPCM = 0x0002,	/* Microsoft ADPCM Format */
	WAVE_FORMAT_IEEE_FLOAT = 0x0003,	/* IEEE Float */
	WAVE_FORMAT_VSELP = 0x0004,	/* Compaq Computer's VSELP */
	WAVE_FORMAT_IBM_CVSD = 0x0005,	/* IBM CVSD */
	WAVE_FORMAT_ALAW = 0x0006,	/* Microsoft ALAW */
	WAVE_FORMAT_MULAW = 0x0007,	/* Microsoft MULAW */
	WAVE_FORMAT_OKI_ADPCM = 0x0010,	/* OKI ADPCM */
	WAVE_FORMAT_DVI_ADPCM = 0x0011,	/* Intel's DVI ADPCM */
	WAVE_FORMAT_MEDIASPACE_ADPCM = 0x0012,	/* Videologic's MediaSpace ADPCM */
	WAVE_FORMAT_SIERRA_ADPCM = 0x0013,	/* Sierra ADPCM */
	WAVE_FORMAT_G723_ADPCM = 0x0014,	/* G.723 ADPCM */
	WAVE_FORMAT_DIGISTD = 0x0015,	/* DSP Solutions' DIGISTD */
	WAVE_FORMAT_DIGIFIX = 0x0016,	/* DSP Solutions' DIGIFIX */
	WAVE_FORMAT_DIALOGIC_OKI_ADPCM = 0x0017,	/* Dialogic OKI ADPCM */
	WAVE_FORMAT_MEDIAVISION_ADPCM = 0x0018,	/* MediaVision ADPCM */
	WAVE_FORMAT_CU_CODEC = 0x0019,	/* HP CU */
	WAVE_FORMAT_YAMAHA_ADPCM = 0x0020,	/* Yamaha ADPCM */
	WAVE_FORMAT_SONARC = 0x0021,	/* Speech Compression's Sonarc */
	WAVE_FORMAT_DSP_TRUESPEECH = 0x0022,	/* DSP Group's True Speech */
	WAVE_FORMAT_ECHOSC1 = 0x0023,	/* Echo Speech's EchoSC1 */
	WAVE_FORMAT_AUDIOFILE_AF36 = 0x0024,	/* Audiofile AF36 */
	WAVE_FORMAT_APTX = 0x0025,	/* APTX */
	WAVE_FORMAT_DOLBY_AC2 = 0x0030,	/* Dolby AC2 */
	WAVE_FORMAT_GSM610 = 0x0031,	/* GSM610 */
	WAVE_FORMAT_MSNAUDIO = 0x0032,	/* MSNAudio */
	WAVE_FORMAT_ANTEX_ADPCME = 0x0033,	/* Antex ADPCME */

	WAVE_FORMAT_MPEG = 0x0050,		/* MPEG */
	WAVE_FORMAT_MPEGLAYER3 = 0x0055,	/* MPEG layer 3 */
	WAVE_FORMAT_LUCENT_G723 = 0x0059,	/* Lucent G.723 */
	WAVE_FORMAT_G726_ADPCM = 0x0064,	/* G.726 ADPCM */
	WAVE_FORMAT_G722_ADPCM = 0x0065,	/* G.722 ADPCM */

	IBM_FORMAT_MULAW = 0x0101,
	IBM_FORMAT_ALAW = 0x0102,
	IBM_FORMAT_ADPCM = 0x0103,

	WAVE_FORMAT_CREATIVE_ADPCM = 0x0200
};

#define _AF_WAVE_NUM_INSTPARAMS 7
extern const InstParamInfo _af_wave_inst_params[_AF_WAVE_NUM_INSTPARAMS];
#define _AF_WAVE_NUM_COMPTYPES 2
extern const int _af_wave_compression_types[_AF_WAVE_NUM_COMPTYPES];

class WAVEFile : public _AFfilehandle
{
public:
	static bool recognize(File *fh);
	static AFfilesetup completeSetup(AFfilesetup);

	WAVEFile();

	status readInit(AFfilesetup);
	status writeInit(AFfilesetup);

	status update();

	bool isInstrumentParameterValid(AUpvlist, int);

private:
	AFfileoffset	factOffset;	/* start of fact (frame count) chunk */
	AFfileoffset	miscellaneousStartOffset;
	AFfileoffset	totalMiscellaneousSize;
	AFfileoffset	markOffset;
	AFfileoffset	dataSizeOffset;

	/*
		The following information is specified in the format
		chunk and is for use with compressed data formats.
	*/
	uint32_t	blockAlign, samplesPerBlock;

	/*
		The index into the coefficient array is of type
		uint8_t, so we can safely limit msadpcmCoefficients to
		be 256 coefficient pairs.
	*/
	int16_t		msadpcmCoefficients[256][2];

	status parseFrameCount(uint32_t type, uint32_t size);
	status parseFormat(uint32_t type, uint32_t size);
	status parseData(uint32_t type, uint32_t size);
	status parsePlayList(uint32_t type, uint32_t size);
	status parseCues(uint32_t type, uint32_t size);
	status parseADTLSubChunk(uint32_t type, uint32_t size);
	status parseINFOSubChunk(uint32_t type, uint32_t size);
	status parseList(uint32_t type, uint32_t size);
	status parseInstrument(uint32_t type, uint32_t size);

	status writeFormat();
	status writeFrameCount();
	status writeMiscellaneous();
	status writeCues();
	status writeData();
};

#endif
