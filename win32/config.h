/*
 | $Id$
 | config.h for Win32, Copyright (c) 2001, Chris Wolf <af@starclass.com>
 | This file is specifically for the MSVC environment, it has not 
 | been tested using Cygwin GCC, yet.
 */

#if !defined(__HAVE_AF_CONFIG_H__) && defined(_MSC_VER)
#define __HAVE_AF_CONFIG_H__

#include <float.h>

#define isnan _isnan
#define vsnprintf _vsnprintf
#define snprintf _snprintf

typedef long           ssize_t;
typedef char           int8_t;
typedef unsigned char  u_int8_t;
typedef short          int16_t;
typedef unsigned short u_int16_t;
typedef long           int32_t;
typedef unsigned long  u_int32_t;





/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a off_t.  */
#define SIZEOF_OFF_T 4

/* The number of bytes in a size_t.  */
#define SIZEOF_SIZE_T 4

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <unistd.h> header file.  */
/* #define HAVE_UNISTD_H 1 */

/* Define if you have the m library (-lm).  */
#define HAVE_LIBM 1

/* Name of package */
#define PACKAGE "audiofile"

/* Version number of package */
#define VERSION "0.2.6"

#endif /* __HAVE_AF_CONFIG_H__ */
