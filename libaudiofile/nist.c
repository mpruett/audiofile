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
	nist.c

	This file contains code for recognizing NIST SPHERE files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "afinternal.h"
#include "audiofile.h"

#include "nist.h"

bool _af_nist_recognize (AFvirtualfile *fh)
{
	u_int8_t	buffer[16];

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(buffer, 16, 1, fh) != 1)
		return AF_FALSE;

	/* Check to see if the file's magic number matches. */
	if (memcmp(buffer, "NIST_1A\n   1024\n", 16) == 0)
		return AF_TRUE;

	return AF_FALSE;
}
