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
	print.h

	Declare format specifiers for off_t and size_t for use with printf.
*/

#ifndef PRINT_H
#define PRINT_H

#include <config.h>

#if (SIZEOF_OFF_T == SIZEOF_LONG)
#define AF_OFF_T_PRINT_FMT "ld"
#endif
#if (SIZEOF_OFF_T > SIZEOF_LONG)
#define AF_OFF_T_PRINT_FMT "lld"
#endif
#if (SIZEOF_OFF_T < SIZEOF_LONG)
#define AF_OFF_T_PRINT_FMT "d"
#endif

#if (SIZEOF_SIZE_T == SIZEOF_LONG)
#define AF_SIZE_T_PRINT_FMT "lu"
#endif
#if (SIZEOF_SIZE_T > SIZEOF_LONG)
#define AF_SIZE_T_PRINT_FMT "llu"
#endif
#if (SIZEOF_SIZE_T < SIZEOF_LONG)
#define AF_SIZE_T_PRINT_FMT "u"
#endif

#define AF_FRAMECOUNT_PRINT_FMT AF_OFF_T_PRINT_FMT
#define AF_FILEOFFSET_PRINT_FMT AF_OFF_T_PRINT_FMT

#endif /* PRINT_H */
