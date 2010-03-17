/*
** adpcm.h - include file for adpcm coder.
**
** Version 1.0, 7-Jul-92.
*/

#ifndef ADPCM_H
#define ADPCM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>

struct adpcm_state
{
	int16_t	valprev;	/* Previous output value */
	uint8_t	index;		/* Index into step size table */
};

void _af_adpcm_coder (const int16_t *src, uint8_t *dst, int frameCount,
	int channelCount, struct adpcm_state *);
void _af_adpcm_decoder (const uint8_t *src, int16_t *dst, int frameCount,
	int channelCount, struct adpcm_state *);

#endif /* ADPCM_H */
