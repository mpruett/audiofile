#include "afinternal.h"
#include "byteorder.h"
#include "compression.h"
#include "units.h"
#include "util.h"
#include <assert.h>

size_t _AudioFormat::bytesPerSample(bool stretch3to4) const
{
	return _af_format_sample_size_uncompressed(this, stretch3to4);
}

size_t _AudioFormat::bytesPerFrame(bool stretch3to4) const
{
	return _af_format_frame_size_uncompressed(this, stretch3to4);
}

size_t _AudioFormat::bytesPerSample() const
{
	return bytesPerSample(!isPacked());
}

size_t _AudioFormat::bytesPerFrame() const
{
	return bytesPerFrame(!isPacked());
}

bool _AudioFormat::isInteger() const
{
	return sampleFormat == AF_SAMPFMT_TWOSCOMP ||
		sampleFormat == AF_SAMPFMT_UNSIGNED;
}

bool _AudioFormat::isSigned() const
{
	return sampleFormat == AF_SAMPFMT_TWOSCOMP;
}

bool _AudioFormat::isUnsigned() const
{
	return sampleFormat == AF_SAMPFMT_UNSIGNED;
}

bool _AudioFormat::isFloat() const
{
	return sampleFormat == AF_SAMPFMT_FLOAT ||
		sampleFormat == AF_SAMPFMT_DOUBLE;
}

bool _AudioFormat::isCompressed() const
{
	return compressionType != AF_COMPRESSION_NONE;
}

bool _AudioFormat::isUncompressed() const
{
	return compressionType == AF_COMPRESSION_NONE;
}

std::string _AudioFormat::description() const
{
	std::string d;
	char s[1024];
	/* sampleRate, channelCount */
	sprintf(s, "{ %7.2f Hz %d ch ", sampleRate, channelCount);
	d += s;

	/* sampleFormat, sampleWidth */
	switch (sampleFormat)
	{
		case AF_SAMPFMT_TWOSCOMP:
			sprintf(s, "%db 2 ", sampleWidth);
			break;
		case AF_SAMPFMT_UNSIGNED:
			sprintf(s, "%db u ", sampleWidth);
			break;
		case AF_SAMPFMT_FLOAT:
			sprintf(s, "flt ");
			break;
		case AF_SAMPFMT_DOUBLE:
			sprintf(s, "dbl ");
			break;
		default:
			assert(false);
			break;
	}

	d += s;

	/* pcm */
	sprintf(s, "(%.30g+-%.30g [%.30g,%.30g]) ",
		pcm.intercept, pcm.slope,
		pcm.minClip, pcm.maxClip);
	d += s;

	/* byteOrder */
	switch (byteOrder)
	{
		case AF_BYTEORDER_BIGENDIAN:
			d += "big ";
			break;
		case AF_BYTEORDER_LITTLEENDIAN:
			d += "little ";
			break;
		default:
			assert(false);
			break;
	}

	if (isCompressed())
	{
		int compressionIndex = _af_compression_index_from_id(compressionType);
		assert(compressionIndex >= 0);
		d += "compression: ";
		d += _af_compression[compressionIndex].label;
	}

	return d;
}
