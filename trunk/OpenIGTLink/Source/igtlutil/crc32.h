/* crc32.h -- compute the CRC-32 of a data stream
  Copyright (C) 1995-1998 Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu
*/

#ifndef __CRC32_H__
#define __CRC32_H__

#ifndef _ANSI_ARGS_
#ifdef PROTOTYPES
#define _ANSI_ARGS_(c)  c
#else
#define _ANSI_ARGS_(c)  ()
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long crc32_t;
#define Z_NULL  0

#define crc32 uulib_crc32

crc32_t crc32 _ANSI_ARGS_((crc32_t crc, const unsigned char *buf, unsigned int len));
/*
     Update a running crc with the bytes buf[0..len-1] and return the updated
   crc. If buf is NULL, this function returns the required initial value
   for the crc. Pre- and post-conditioning (one's complement) is performed
   within this function so it shouldn't be done by the application.
   Usage example:

     uLong crc = crc32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       crc = crc32(crc, buffer, length);
     }
     if (crc != original_crc) error();
*/

#ifdef __cplusplus
}
#endif
#endif
