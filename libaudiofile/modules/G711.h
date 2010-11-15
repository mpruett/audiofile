/*
	Audio File Library
	Copyright (C) 2000, Silicon Graphics, Inc.
	Copyright (C) 2010, Michael Pruett <michael@68k.org>

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
	g711.h
*/

#ifndef MODULES_G711_H
#define MODULES_G711_H

#include <audiofile.h>
#include "afinternal.h"

#ifdef __cplusplus
extern "C" {
#endif

bool _af_g711_format_ok (_AudioFormat *f);

Module *_AFg711initcompress (_Track *, AFvirtualfile *, bool canSeek,
	bool headerless, AFframecount *chunkframes);

Module *_AFg711initdecompress (_Track *, AFvirtualfile *, bool canSeek,
	bool headerless, AFframecount *chunkframes);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_G711_H */
