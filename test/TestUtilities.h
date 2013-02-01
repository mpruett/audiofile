/*
	Audio File Library
	Copyright (C) 2012, Michael Pruett <michael@68k.org>

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

#ifndef TEST_UTILITIES_H
#define TEST_UTILITIES_H

#include <audiofile.h>

#ifdef __cplusplus

#include <string>

bool createTemporaryFile(const std::string &prefix, std::string *path);

class IgnoreErrors
{
public:
	IgnoreErrors()
	{
		m_oldErrorHandler = afSetErrorHandler(NULL);
	}
	~IgnoreErrors()
	{
		afSetErrorHandler(m_oldErrorHandler);
	}

private:
	AFerrfunc m_oldErrorHandler;
};

#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool createTemporaryFile(const char *prefix, char *path);

#ifdef __cplusplus
}
#endif

#endif
