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
	pcm.h
*/

#ifndef MODULES_PCM_H
#define MODULES_PCM_H

#include <audiofile.h>
#include "afinternal.h"
#include "compression.h"
#include "modules.h"

bool _af_pcm_format_ok (_AudioFormat *f);

_AFmoduleinst _AFpcminitcompress (_Track *trk, AFvirtualfile *fh, bool seekok,
	bool headerless, AFframecount *chunkframes);

_AFmoduleinst _AFpcminitdecompress (_Track *trk, AFvirtualfile *fh, bool seekok,
	bool headerless, AFframecount *chunkframes);

#endif /* MODULES_PCM_H */
