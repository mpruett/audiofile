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
	nist.h

	This file declares code for reading and writing NIST SPHERE files.
*/

#ifndef NIST_H
#define NIST_H

#include "audiofile.h"

#define NIST_SPHERE_HEADER_LENGTH 1024
#define NIST_SPHERE_MAX_FIELD_LENGTH 80

bool _af_nist_recognize (AFvirtualfile *fh);
AFfilesetup _af_nist_complete_setup (AFfilesetup setup);
status _af_nist_read_init (AFfilesetup setup, AFfilehandle handle);
status _af_nist_write_init (AFfilesetup setup, AFfilehandle handle);
status _af_nist_update (AFfilehandle file);

#endif /* NIST_H */
