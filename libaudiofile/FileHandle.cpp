/*
	Audio File Library
	Copyright (C) 2010-2012, Michael Pruett <michael@68k.org>
	Copyright (C) 2000-2001, Silicon Graphics, Inc.

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

#include "config.h"
#include "FileHandle.h"

#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include <stdlib.h>
#include <assert.h>

#include "AIFF.h"
#include "AVR.h"
#include "CAF.h"
#include "IFF.h"
#include "IRCAM.h"
#include "NeXT.h"
#include "NIST.h"
#include "Raw.h"
#include "SampleVision.h"
#include "VOC.h"
#include "WAVE.h"

#include "File.h"
#include "Instrument.h"
#include "Setup.h"
#include "Tag.h"
#include "Track.h"
#include "units.h"
#include "util.h"

static void freeInstParams (AFPVu *values, int fileFormat)
{
	if (!values)
		return;

	int parameterCount = _af_units[fileFormat].instrumentParameterCount;

	for (int i=0; i<parameterCount; i++)
	{
		if (_af_units[fileFormat].instrumentParameters[i].type == AU_PVTYPE_PTR)
			free(values[i].v);
	}

	free(values);
}

_AFfilehandle *_AFfilehandle::create(int fileFormat)
{
	switch (fileFormat)
	{
		case AF_FILE_RAWDATA:
			return new RawFile();
		case AF_FILE_AIFF:
		case AF_FILE_AIFFC:
			return new AIFFFile();
		case AF_FILE_NEXTSND:
			return new NeXTFile();
		case AF_FILE_WAVE:
			return new WAVEFile();
		case AF_FILE_BICSF:
			return new IRCAMFile();
		case AF_FILE_AVR:
			return new AVRFile();
		case AF_FILE_IFF_8SVX:
			return new IFFFile();
		case AF_FILE_SAMPLEVISION:
			return new SampleVisionFile();
		case AF_FILE_VOC:
			return new VOCFile();
		case AF_FILE_NIST_SPHERE:
			return new NISTFile();
		case AF_FILE_CAF:
			return new CAFFile();
		default:
			return NULL;
	}
}

_AFfilehandle::_AFfilehandle()
{
	valid = _AF_VALID_FILEHANDLE;
	fh = NULL;
	fileName = NULL;
	trackCount = 0;
	tracks = NULL;
	instrumentCount = 0;
	instruments = NULL;
	miscellaneousCount = 0;
	miscellaneous = NULL;
	m_formatByteOrder = 0;
}

_AFfilehandle::~_AFfilehandle()
{
	valid = 0;

	free(fileName);

	delete [] tracks;
	tracks = NULL;
	trackCount = 0;

	if (instruments)
	{
		for (int i=0; i<instrumentCount; i++)
		{
			free(instruments[i].loops);
			instruments[i].loops = NULL;
			instruments[i].loopCount = 0;

			freeInstParams(instruments[i].values, fileFormat);
			instruments[i].values = NULL;
		}

		free(instruments);
		instruments = NULL;
	}
	instrumentCount = 0;

	if (miscellaneous)
	{
		for (int i=0; i<miscellaneousCount; i++)
			free(miscellaneous[i].buffer);
		free(miscellaneous);
		miscellaneous = NULL;
	}
	miscellaneousCount = 0;
}

Track *_AFfilehandle::allocateTrack()
{
	assert(!trackCount);
	assert(!tracks);

	trackCount = 1;
	tracks = new Track[1];
	return tracks;
}

Track *_AFfilehandle::getTrack(int trackID)
{
	for (int i=0; i<trackCount; i++)
		if (tracks[i].id == trackID)
			return &tracks[i];

	_af_error(AF_BAD_TRACKID, "bad track id %d", trackID);

	return NULL;
}

bool _AFfilehandle::checkCanRead()
{
	if (access != _AF_READ_ACCESS)
	{
		_af_error(AF_BAD_NOREADACC, "file not opened for read access");
		return false;
	}

	return true;
}

bool _AFfilehandle::checkCanWrite()
{
	if (access != _AF_WRITE_ACCESS)
	{
		_af_error(AF_BAD_NOWRITEACC, "file not opened for write access");
		return false;
	}

	return true;
}

Instrument *_AFfilehandle::getInstrument(int instrumentID)
{
	for (int i = 0; i < instrumentCount; i++)
		if (instruments[i].id == instrumentID)
			return &instruments[i];

	_af_error(AF_BAD_INSTID, "invalid instrument id %d", instrumentID);
	return NULL;
}

Miscellaneous *_AFfilehandle::getMiscellaneous(int miscellaneousID)
{
	for (int i=0; i<miscellaneousCount; i++)
	{
		if (miscellaneous[i].id == miscellaneousID)
			return &miscellaneous[i];
	}

	_af_error(AF_BAD_MISCID, "bad miscellaneous id %d", miscellaneousID);

	return NULL;
}

status _AFfilehandle::initFromSetup(AFfilesetup setup)
{
	if (copyTracksFromSetup(setup) == AF_FAIL)
		return AF_FAIL;
	if (copyInstrumentsFromSetup(setup) == AF_FAIL)
		return AF_FAIL;
	if (copyMiscellaneousFromSetup(setup) == AF_FAIL)
		return AF_FAIL;
	return AF_SUCCEED;
}

status _AFfilehandle::copyTracksFromSetup(AFfilesetup setup)
{
	if ((trackCount = setup->trackCount) == 0)
	{
		tracks = NULL;
		return AF_SUCCEED;
	}

	tracks = new Track[trackCount];
	if (!tracks)
		return AF_FAIL;

	for (int i=0; i<trackCount; i++)
	{
		Track *track = &tracks[i];
		TrackSetup *trackSetup = &setup->tracks[i];

		track->id = trackSetup->id;
		track->f = trackSetup->f;

		if (track->copyMarkers(trackSetup) == AF_FAIL)
			return AF_FAIL;

		track->hasAESData = trackSetup->aesDataSet;
	}

	return AF_SUCCEED;
}

status _AFfilehandle::copyInstrumentsFromSetup(AFfilesetup setup)
{
	if ((instrumentCount = setup->instrumentCount) == 0)
	{
		instruments = NULL;
		return AF_SUCCEED;
	}

	instruments = static_cast<Instrument *>(_af_calloc(instrumentCount,
		sizeof (Instrument)));
	if (!instruments)
		return AF_FAIL;

	for (int i=0; i<instrumentCount; i++)
	{
		instruments[i].id = setup->instruments[i].id;

		// Copy loops.
		if ((instruments[i].loopCount = setup->instruments[i].loopCount) == 0)
		{
			instruments[i].loops = NULL;
		}
		else
		{
			instruments[i].loops =
				static_cast<Loop *>(_af_calloc(instruments[i].loopCount,
					sizeof (Loop)));
			if (!instruments[i].loops)
				return AF_FAIL;
			for (int j=0; j<instruments[i].loopCount; j++)
			{
				Loop *loop = &instruments[i].loops[j];
				loop->id = setup->instruments[i].loops[j].id;
				loop->mode = AF_LOOP_MODE_NOLOOP;
				loop->count = 0;
				loop->trackid = AF_DEFAULT_TRACK;
				loop->beginMarker = 2*j + 1;
				loop->endMarker = 2*j + 2;
			}
		}

		int instParamCount;
		// Copy instrument parameters.
		if ((instParamCount = _af_units[setup->fileFormat].instrumentParameterCount) == 0)
		{
			instruments[i].values = NULL;
		}
		else
		{
			instruments[i].values =
				static_cast<AFPVu *>(_af_calloc(instParamCount, sizeof (AFPVu)));
			if (!instruments[i].values)
				return AF_FAIL;
			for (int j=0; j<instParamCount; j++)
			{
				instruments[i].values[j] = _af_units[setup->fileFormat].instrumentParameters[j].defaultValue;
			}
		}
	}

	return AF_SUCCEED;
}

status _AFfilehandle::copyMiscellaneousFromSetup(AFfilesetup setup)
{
	if ((miscellaneousCount = setup->miscellaneousCount) == 0)
	{
		miscellaneous = NULL;
		return AF_SUCCEED;
	}

	miscellaneous = static_cast<Miscellaneous *>(_af_calloc(miscellaneousCount,
		sizeof (Miscellaneous)));
	if (!miscellaneous)
		return AF_FAIL;

	for (int i=0; i<miscellaneousCount; i++)
	{
		miscellaneous[i].id = setup->miscellaneous[i].id;
		miscellaneous[i].type = setup->miscellaneous[i].type;
		miscellaneous[i].size = setup->miscellaneous[i].size;
		miscellaneous[i].position = 0;
		miscellaneous[i].buffer = NULL;
	}

	return AF_SUCCEED;
}

template <typename T>
static bool readValue(File *f, T *value)
{
	return f->read(value, sizeof (*value)) == sizeof (*value);
}

template <typename T>
static bool writeValue(File *f, const T *value)
{
	return f->write(value, sizeof (*value)) == sizeof (*value);
}

template <typename T>
static T swapValue(T value, int order)
{
	if (order == AF_BYTEORDER_BIGENDIAN)
		return bigToHost(value);
	else if (order == AF_BYTEORDER_LITTLEENDIAN)
		return littleToHost(value);
	return value;
}

template <typename T>
static bool readSwap(File *f, T *value, int order)
{
	if (!readValue(f, value)) return false;
	*value = swapValue(*value, order);
	return true;
}

template <typename T>
static bool writeSwap(File *f, const T *value, int order)
{
	T t = swapValue(*value, order);
	return writeValue(f, &t);
}

bool _AFfilehandle::readU8(uint8_t *v) { return readValue(fh, v); }
bool _AFfilehandle::readS8(int8_t *v) { return readValue(fh, v); }

bool _AFfilehandle::readU16(uint16_t *v)
{
	return readSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::readS16(int16_t *v)
{
	return readSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::readU32(uint32_t *v)
{
	return readSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::readS32(int32_t *v)
{
	return readSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::readU64(uint64_t *v)
{
	return readSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::readS64(int64_t *v)
{
	return readSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::readFloat(float *v)
{
	return readSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::readDouble(double *v)
{
	return readSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::writeU8(const uint8_t *v) { return writeValue(fh, v); }
bool _AFfilehandle::writeS8(const int8_t *v) { return writeValue(fh, v); }

bool _AFfilehandle::writeU16(const uint16_t *v)
{
	return writeSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::writeS16(const int16_t *v)
{
	return writeSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::writeU32(const uint32_t *v)
{
	return writeSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::writeS32(const int32_t *v)
{
	return writeSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::writeU64(const uint64_t *v)
{
	return writeSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::writeS64(const int64_t *v)
{
	return writeSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::writeFloat(const float *v)
{
	return writeSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::writeDouble(const double *v)
{
	return writeSwap(fh, v, m_formatByteOrder);
}

bool _AFfilehandle::readTag(Tag *t)
{
	uint32_t v;
	if (fh->read(&v, sizeof (v)) == sizeof (v))
	{
		*t = Tag(v);
		return true;
	}
	return false;
}

bool _AFfilehandle::writeTag(const Tag *t)
{
	uint32_t v = t->value();
	return fh->write(&v, sizeof (v)) == sizeof (v);
}
