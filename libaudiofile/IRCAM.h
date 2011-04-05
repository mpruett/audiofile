/*
	Audio File Library

	Copyright (C) 2001, Silicon Graphics, Inc.

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
	IRCAM.h

	This file contains constants and function prototypes related to
	the Berkeley/IRCAM/CARL Sound File format.
*/

#ifndef IRCAM_H
#define IRCAM_H

#include "FileHandle.h"
#include <stdint.h>

#define SF_SHORT 2
#define SF_FLOAT 4

#define SF_MAXCHAN 4
#define SF_MAXCOMMENT 512
#define SF_MINCOMMENT 256

enum
{
	SF_END,
	SF_MAXAMP,
	SF_COMMENT,
	SF_LINKCODE
};

#define SIZEOF_BSD_HEADER 1024

extern const uint8_t _af_ircam_vax_le_magic[4],
	_af_ircam_vax_be_magic[4],
	_af_ircam_sun_be_magic[4],
	_af_ircam_sun_le_magic[4],
	_af_ircam_mips_le_magic[4],
	_af_ircam_mips_be_magic[4],
	_af_ircam_next_be_magic[4];

class IRCAMFile : public _AFfilehandle
{
public:
	static bool recognize(File *fh);
	static AFfilesetup completeSetup(AFfilesetup);

	status readInit(AFfilesetup);
	status writeInit(AFfilesetup);
	status update();
};

#endif /* IRCAM_H */
