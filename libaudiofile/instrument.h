/*
	Audio File Library
	Copyright (C) 2000, Michael Pruett <michael@68k.org>

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
	instrument.h

	This file declares routines for dealing with instruments.
*/

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <audiofile.h>
#include <aupvlist.h>

void _af_instparam_get (AFfilehandle file, int instid, AUpvlist pvlist,
	int npv, bool forceLong);

void _af_instparam_set (AFfilehandle file, int instid, AUpvlist pvlist,
	int npv);

int _af_instparam_index_from_id (int fileFormat, int id);
int _af_handle_instrument_index_from_id (AFfilehandle file, int id);
int _af_setup_instrument_index_from_id (AFfilesetup setup, int id);

#endif /* INSTRUMENT_H */
