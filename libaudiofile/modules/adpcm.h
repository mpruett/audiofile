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

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

struct adpcm_state {
    short	valprev;	/* Previous output value */
    char	index;		/* Index into stepsize table */
};

void _af_adpcm_coder (int16_t [], u_int8_t [], int, struct adpcm_state *);
void _af_adpcm_decoder (u_int8_t [], int16_t [], int, struct adpcm_state *);

#endif /* ADPCM_H */
