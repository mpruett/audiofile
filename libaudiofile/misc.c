/*
	Audio File Library
	Copyright (C) 1998, Michael Pruett <michael@68k.org>

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
	misc.c

	This file contains routines for dealing with the Audio File
	Library's internal miscellaneous data types.
*/

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "audiofile.h"
#include "afinternal.h"
#include "util.h"

_Miscellaneous *find_misc_by_id (AFfilehandle file, int id)
{
	int	i;

	for (i=0; i<file->miscellaneousCount; i++)
	{
		if (file->miscellaneous[i].id == id)
			return &file->miscellaneous[i];
	}

	_af_error(AF_BAD_MISCID, "bad miscellaneous id %d", id);

	return NULL;
}

_MiscellaneousSetup *find_miscsetup_by_id (AFfilesetup setup, int id)
{
	int	i;

	for (i=0; i<setup->miscellaneousCount; i++)
	{
		if (setup->miscellaneous[i].id == id)
			return &setup->miscellaneous[i];
	}

	_af_error(AF_BAD_MISCID, "bad miscellaneous id %d", id);

	return NULL;
}

void afInitMiscIDs (AFfilesetup setup, int *ids, int nids)
{
	int	i;

	if (!_af_filesetup_ok(setup))
		return;

	if (setup->miscellaneous != NULL)
	{
		free(setup->miscellaneous);
	}

	setup->miscellaneousCount = nids;

	if (nids == 0)
		setup->miscellaneous = NULL;
	else
	{
		setup->miscellaneous = _af_calloc(nids,
			sizeof (_Miscellaneous));

		if (setup->miscellaneous == NULL)
			return;

		for (i=0; i<nids; i++)
		{
			setup->miscellaneous[i].id = ids[i];
			setup->miscellaneous[i].type = 0;
			setup->miscellaneous[i].size = 0;
		}
	}

	setup->miscellaneousSet = AF_TRUE;
}

int afGetMiscIDs (AFfilehandle file, int *ids)
{
	int	i;

	if (!_af_filehandle_ok(file))
		return -1;

	if (ids != NULL)
	{
		for (i=0; i<file->miscellaneousCount; i++)
		{
			ids[i] = file->miscellaneous[i].id;
		}
	}

	return file->miscellaneousCount;
}

void afInitMiscType (AFfilesetup setup, int miscellaneousid, int type)
{
	_MiscellaneousSetup	*miscellaneous;

	if (!_af_filesetup_ok(setup))
		return;

	miscellaneous = find_miscsetup_by_id(setup, miscellaneousid);

	if (miscellaneous)
		miscellaneous->type = type;
	else
		_af_error(AF_BAD_MISCID, "bad miscellaneous id");
}

int afGetMiscType (AFfilehandle file, int miscellaneousid)
{
	_Miscellaneous	*miscellaneous;

	if (!_af_filehandle_ok(file))
		return -1;

	miscellaneous = find_misc_by_id(file, miscellaneousid);

	if (miscellaneous)
	{
		return miscellaneous->type;
	}
	else
	{
		_af_error(AF_BAD_MISCID, "bad miscellaneous id");
		return -1;
	}
}

void afInitMiscSize (AFfilesetup setup, int miscellaneousid, int size)
{
	_MiscellaneousSetup	*miscellaneous;

	if (!_af_filesetup_ok(setup))
		return;

	miscellaneous = find_miscsetup_by_id(setup, miscellaneousid);

	if (miscellaneous)
	{
		miscellaneous->size = size;
	}
	else
		_af_error(AF_BAD_MISCID, "bad miscellaneous id");
}

int afGetMiscSize (AFfilehandle file, int miscellaneousid)
{
	_Miscellaneous	*miscellaneous;

	if (!_af_filehandle_ok(file))
		return -1;

	miscellaneous = find_misc_by_id(file, miscellaneousid);

	if (miscellaneous)
	{
		return miscellaneous->size;
	}
	else
	{
		_af_error(AF_BAD_MISCID, "bad miscellaneous id");
		return -1;
	}
}

int afWriteMisc (AFfilehandle file, int miscellaneousid, void *buf, int bytes)
{
	_Miscellaneous	*miscellaneous;
	int		localsize;

	if (!_af_filehandle_ok(file))
		return -1;

	if (!_af_filehandle_can_write(file))
		return -1;

	if ((miscellaneous = find_misc_by_id(file, miscellaneousid)) == NULL)
		return -1;

	if (bytes <= 0)
	{
		_af_error(AF_BAD_MISCSIZE, "invalid size (%d) for miscellaneous chunk", bytes);
	}

	if (miscellaneous->buffer == NULL && miscellaneous->size != 0)
	{
		miscellaneous->buffer = _af_malloc(miscellaneous->size);
		memset(miscellaneous->buffer, 0, miscellaneous->size);
		if (miscellaneous->buffer == NULL)
			return -1;
	}

	if (bytes + miscellaneous->position > miscellaneous->size)
		localsize = miscellaneous->size - miscellaneous->position;
	else
		localsize = bytes;

	memcpy((char *) miscellaneous->buffer + miscellaneous->position,
		buf, localsize);
	miscellaneous->position += localsize;
	return localsize;
}

int afReadMisc (AFfilehandle file, int miscellaneousid, void *buf, int bytes)
{
	int		localsize;
	_Miscellaneous	*miscellaneous;

	if (!_af_filehandle_ok(file))
		return -1;

	if (!_af_filehandle_can_read(file))
		return -1;

	if ((miscellaneous = find_misc_by_id(file, miscellaneousid)) == NULL)
		return -1;

	if (bytes <= 0)
	{
		_af_error(AF_BAD_MISCSIZE, "invalid size (%d) for miscellaneous chunk", bytes);
		return -1;
	}

	if (bytes + miscellaneous->position > miscellaneous->size)
		localsize = miscellaneous->size - miscellaneous->position;
	else
		localsize = bytes;

	memcpy(buf, (char *) miscellaneous->buffer + miscellaneous->position,
		localsize);
	miscellaneous->position += localsize;
	return localsize;
}

int afSeekMisc (AFfilehandle file, int miscellaneousid, int offset)
{
	_Miscellaneous	*miscellaneous;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((miscellaneous = find_misc_by_id(file, miscellaneousid)) == NULL)
		return -1;

	if (offset >= miscellaneous->size)
	{
		_af_error(AF_BAD_MISCSEEK,
			"offset %d too big for miscellaneous chunk %d "
			"(%d data bytes)",
			offset, miscellaneousid, miscellaneous->size);
		return -1;
	}

	miscellaneous->position = offset;

	return offset;
}
