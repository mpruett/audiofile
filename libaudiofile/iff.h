/*
	Audio File Library
	Copyright (C) 2004, Michael Pruett <michael@68k.org>

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
	iff.h

	This file declares constants and functions related to the
	IFF/8SVX file format.
*/

#ifndef IFF_H
#define IFF_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "audiofile.h"

bool _af_iff_recognize (AFvirtualfile *fh);
status _af_iff_read_init (AFfilesetup, AFfilehandle);
AFfilesetup _af_iff_complete_setup (AFfilesetup);
status _af_iff_write_init (AFfilesetup, AFfilehandle);
status _af_iff_update (AFfilehandle);

typedef struct
{
	AFfileoffset	miscellaneousPosition;
	AFfileoffset	VHDR_offset;
	AFfileoffset	BODY_offset;
} _IFFinfo;

#endif
