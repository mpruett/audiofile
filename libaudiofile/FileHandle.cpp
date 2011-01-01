#include "config.h"

#include "afinternal.h"
#include "audiofile.h"
#include <stdlib.h>

#include "AIFF.h"
#include "AVR.h"
#include "IFF.h"
#include "IRCAM.h"
#include "NIST.h"
#include "NeXT.h"
#include "Raw.h"
#include "WAVE.h"

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
		case AF_FILE_NIST_SPHERE:
			return new NISTFile();
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
}

_AFfilehandle::~_AFfilehandle()
{
}
