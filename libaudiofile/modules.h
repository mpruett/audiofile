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
	modules.h
*/

#ifndef MODULES_H
#define MODULES_H

#include <sys/types.h>
#include <audiofile.h>
#include "afinternal.h"

typedef u_int8_t uchar1;
typedef u_int16_t uchar2;
typedef u_int32_t uchar3;
typedef u_int32_t uchar4;

typedef int8_t schar1;
typedef int16_t schar2;
typedef int32_t schar3;
typedef int32_t schar4;

typedef struct real_char3 { uchar1 c0; uchar1 c1; uchar1 c2; } real_char3;
typedef union char3u
{
	struct { schar4 i; } schar4;
	struct { uchar4 i; } uchar4;
	struct { schar3 i; } schar3;
	struct { uchar3 i; } uchar3;
	struct { real_char3 c3; schar1 pad; } real_char3_high;
	struct { schar1 pad; real_char3 c3; } real_char3_low;
	struct { uchar2 s0; uchar2 s1; } uchar2;
	struct { schar2 s0; schar2 s1; } schar2;
	struct { uchar1 c0; uchar1 c1; uchar1 c2; uchar1 c3; } uchar1;
	struct { schar1 c0; schar1 c1; schar1 c2; schar1 c3; } schar1;
} char3u;

typedef struct real_char8
{
	uchar1 c0, c1, c2, c3, c4, c5, c6, c7;
} real_char8;

typedef union char8u
{
	struct { schar4 i0, i1; } schar4;
	struct { uchar4 i0, i1; } uchar4;
	struct { schar2 s0, s1, s2, s3; } schar2;
	struct { uchar2 s0, s1, s2, s3; } uchar2;
	struct { schar1 c0, c1, c2, c3, c4, c5, c6, c7; } schar1;
	struct { uchar1 c0, c1, c2, c3, c4, c5, c6, c7; } uchar1;
} char8u;

#define AF_NULL ((void *) 0)

/*
	_AF_ATOMIC_NVFRAMES is NOT the maximum number of frames a module
	can be requested to produce.

	This IS the maximum number of virtual (user) frames that will
	be produced or processed per run of the modules.

	Modules can be requested more frames than this because of rate
	conversion and rebuffering.
*/

#define _AF_ATOMIC_NVFRAMES 1024

AFframecount _AFpull (_AFmoduleinst *i, AFframecount nframes2pull);
void _AFpush (_AFmoduleinst *i, AFframecount nframes2push);
void _AFpushat (_AFmoduleinst *i, AFframecount startframe, bool stretchint,
	AFframecount nframes2push);
void _AFsimplemodrun_pull (_AFmoduleinst *i);
void _AFsimplemodrun_push (_AFmoduleinst *i);
void _AFfreemodspec (_AFmoduleinst *i);

/* _AFnewmodinst returns a structure, not a pointer. */
_AFmoduleinst _AFnewmodinst (_AFmodule *mod);

status _AFinitmodules (AFfilehandle h, _Track *trk);
status _AFsetupmodules (AFfilehandle h, _Track *trk);
status _AFsyncmodules (AFfilehandle h, _Track *trk);
void _AFfreemodules (_Track *trk);

#endif /* MODULES_H */
