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
	byteorder.c

	This file defines functions which swap bytes.
*/

#include <sys/types.h>
#include "byteorder.h"
#include "util.h"

u_int16_t _af_byteswapint16 (u_int16_t x)
{
	return ((x << 8) | (x >> 8));
}

u_int32_t _af_byteswapint32 (u_int32_t x)
{
	u_int8_t	b1, b2, b3, b4;

	b1 = x>>24;
	b2 = (x>>16) & 0xff;
	b3 = (x>>8) & 0xff;
	b4 = x & 0xff;

	return b1 | (b2 << 8) | (b3 << 16) | (b4 << 24);
}

float _af_byteswapfloat32 (float x)
{
	float		f = x;
	u_int32_t	*l = (u_int32_t *) &f;

	*l = _af_byteswapint32(*l);

	return f;
}

/*
uint64_t _af_byteswapint64 (uint64_t x)
{
	u_int8_t	b1, b2, b3, b4, b5, b6, b7, b8;

	b1 = (x>>56) & 0xff;
	b2 = (x>>48) & 0xff;
	b3 = (x>>40) & 0xff;
	b4 = (x>>32) & 0xff;
	b5 = (x>>24) & 0xff;
	b6 = (x>>16) & 0xff;
	b7 = (x>>8) & 0xff;
	b8 = x & 0xff;

	return b1 | (b2 << 8) | (b3 << 16) | (b4 << 24) |
		(b5 << 32) | (b6 << 40) | (b7 << 48) | (b8 << 56);
}

main ()
{
	long				ldata = '1234';
	unsigned long long	data = 0x1122334455667788;
	printf("%llx\n", data);
	printf("%llx\n", _af_byteswapint64(data));
	printf("%x\n", ldata);
	printf("%x\n", _af_byteswapint32(ldata));
}
*/
