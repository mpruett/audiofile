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

#include "TestUtilities.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

bool createTemporaryFile(const std::string &prefix, std::string *path)
{
	*path = "/tmp/" + prefix + "-XXXXXX";
	int fd = ::mkstemp(const_cast<char *>(path->c_str()));
	if (fd < 0)
		return false;
	::close(fd);
	return true;
}

bool createTemporaryFile(const char *prefix, char *path)
{
	snprintf(path, PATH_MAX, "/tmp/%s-XXXXXX", prefix);
	int fd = ::mkstemp(path);
	if (fd < 0)
		return false;
	::close(fd);
	return true;
}
