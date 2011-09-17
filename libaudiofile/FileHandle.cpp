/*
	Audio File Library
	Copyright (C) 2010-2011, Michael Pruett <michael@68k.org>

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

#include "AIFF.h"
#include "AVR.h"
#include "CAF.h"
#include "IFF.h"
#include "IRCAM.h"
#include "NeXT.h"
#include "NIST.h"
#include "Raw.h"
#include "VOC.h"
#include "WAVE.h"

#include "File.h"
#include "Instrument.h"
#include "Tag.h"
#include "Track.h"

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
