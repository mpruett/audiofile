/*
	Audio File Library

	Copyright 1998-1999, Michael Pruett <michael@68k.org>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be
	useful, but WITHOUT ANY WARRANTY; without even the implied
	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this program; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
	MA 02111-1307, USA.
*/

/*
	sgi.c

	These routines are used in SGI-specific test programs.
*/

#include <stdio.h>
#include <unistd.h>
#include <dmedia/audio.h>

/*
	Set the sample width of an audio configuration.
*/
void setwidth (ALconfig config, int sampleWidth)
{
	if (sampleWidth <= 8)
	{
		printf("setting width to 8\n");
		alSetWidth(config, AL_SAMPLE_8);
	}
	else if (sampleWidth <= 16)
	{
		printf("setting width to 16\n");
		alSetWidth(config, AL_SAMPLE_16);
	}
	else if (sampleWidth <= 24)
	{
		printf("setting width to 24\n");
		alSetWidth(config, AL_SAMPLE_24);
	}
}

/*
	Set the sample rate of an audio port.
*/
void setrate (ALport port, double rate)
{
	int		rv;
	ALpv	params;

	rv = alGetResource(port);

	params.param = AL_RATE;
	params.value.ll = ((long long) rate) << 32;

	if (alSetParams(rv, &params, 1) < 0)
	{
		printf("alSetParams failed: %s\n", alGetErrorString(oserror()));
	}
}

/*
	Wait until the audio port has no more samples to play.
*/
void waitport (ALport port)
{
	while (alGetFilled(port) > 0)
		sginap(1);
}
