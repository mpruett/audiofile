/*
	Audio File Library
	Copyright (C) 1998-1999, Michael Pruett <michael@68k.org>

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
	byteorder.h

	This file declares functions useful for dealing with byte
	swapping.
*/

#ifndef BYTEORDER_H
#define BYTEORDER_H

#include <config.h>

#if WORDS_BIGENDIAN
	#define __BIGENDIAN__
	#define _AF_BYTEORDER_NATIVE (AF_BYTEORDER_BIGENDIAN)
#else
	#define __LITTLEENDIAN__
	#define _AF_BYTEORDER_NATIVE (AF_BYTEORDER_LITTLEENDIAN)
#endif

#ifndef uint16
typedef u_int16_t uint16;
#endif

#ifndef uint32
typedef u_int32_t uint32;
#endif

#ifdef __LITTLEENDIAN__

#define HOST_TO_LENDIAN_INT16(x)		((uint16) (x))
#define HOST_TO_LENDIAN_INT32(x)		((uint32) (x))
#define HOST_TO_LENDIAN_FLOAT32(x)		((float) (x))
#define HOST_TO_LENDIAN_DOUBLE64(x)		((double) (x))

#define LENDIAN_TO_HOST_INT16(x)		((uint16) (x))
#define LENDIAN_TO_HOST_INT32(x)		((uint32) (x))
#define LENDIAN_TO_HOST_FLOAT32(x)		((float) (x))
#define LENDIAN_TO_HOST_DOUBLE64(x)		((double) (x))

#else

#define HOST_TO_LENDIAN_INT16(x)		_af_byteswapint16(x)
#define HOST_TO_LENDIAN_INT32(x)		_af_byteswapint32(x)
#define HOST_TO_LENDIAN_FLOAT32(x)		_af_byteswapfloat32(x)
#define HOST_TO_LENDIAN_DOUBLE64(x)		_af_byteswapdouble64(x)

#define LENDIAN_TO_HOST_INT16(x)		_af_byteswapint16(x)
#define LENDIAN_TO_HOST_INT32(x)		_af_byteswapint32(x)
#define LENDIAN_TO_HOST_FLOAT32(x)		_af_byteswapfloat32(x)
#define LENDIAN_TO_HOST_DOUBLE64(x)		_af_byteswapdouble64(x)

#endif

#ifdef __BIGENDIAN__

#define HOST_TO_BENDIAN_INT16(x)		((uint16) (x))
#define HOST_TO_BENDIAN_INT32(x)		((uint32) (x))
#define HOST_TO_BENDIAN_FLOAT32(x)		((float) (x))
#define HOST_TO_BENDIAN_DOUBLE64(x)		((double) (x))

#define BENDIAN_TO_HOST_INT16(x)		((uint16) (x))
#define BENDIAN_TO_HOST_INT32(x)		((uint32) (x))
#define BENDIAN_TO_HOST_FLOAT32(x)		((float) (x))
#define BENDIAN_TO_HOST_DOUBLE64(x)		((double) (x))

#else

#define HOST_TO_BENDIAN_INT16(x)		_af_byteswapint16(x)
#define HOST_TO_BENDIAN_INT32(x)		_af_byteswapint32(x)
#define HOST_TO_BENDIAN_FLOAT32(x)		_af_byteswapfloat32(x)
#define HOST_TO_BENDIAN_DOUBLE64(x)		_af_byteswapdouble64(x)

#define BENDIAN_TO_HOST_INT16(x)		_af_byteswapint16(x)
#define BENDIAN_TO_HOST_INT32(x)		_af_byteswapint32(x)
#define BENDIAN_TO_HOST_FLOAT32(x)		_af_byteswapfloat32(x)
#define BENDIAN_TO_HOST_DOUBLE64(x)		_af_byteswapdouble64(x)

#endif

u_int16_t _af_byteswapint16 (u_int16_t x);
u_int32_t _af_byteswapint32 (u_int32_t x);

#endif
