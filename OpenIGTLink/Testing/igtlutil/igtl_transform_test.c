/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $RCSfile: $
  Language:  C
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "igtl_header.h"
#include "igtl_transform.h"
#include <string.h>
#include <stdio.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* The decimal is rounded when it is converted to IEEE 754 floating point */

char barray[] = {
  0x00, 0x01,                                     /* Version number */
  0x54, 0x52, 0x41, 0x4e, 0x53, 0x46, 0x4f, 0x52,
  0x4d, 0x00, 0x00, 0x00,                         /* TRANSFORM */ 
  0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x4e, 0x61,
  0x6d, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,                         /* Device name */
  0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x02, 0xd2, /* Time stamp */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, /* Body size */
  0x5a, 0xa6, 0xa1, 0x10, 0x50, 0xbf, 0x62, 0x49, /* CRC */

  0xBF, 0x74, 0x73, 0xCD, 0x3E, 0x49, 0x59, 0xE6, /* tx, ty */
  0xBE, 0x63, 0xDD, 0x98, 0xBE, 0x49, 0x59, 0xE6, /* tz, sx */
  0x3E, 0x12, 0x49, 0x1B, 0x3F, 0x78, 0x52, 0xD6, /* sy, sz */
  0x3E, 0x63, 0xDD, 0x98, 0x3F, 0x78, 0x52, 0xD6, /* nx, ny */
  0xBD, 0xC8, 0x30, 0xAE, 0x42, 0x38, 0x36, 0x60, /* nz, px */
  0x41, 0x9B, 0xC4, 0x67, 0x42, 0x38, 0x36, 0x60, /* py, pz */
};


int main( int argc, char * argv [] )
{

  char* message = malloc(IGTL_HEADER_SIZE+IGTL_TRANSFORM_SIZE);

  igtl_header*  header = (igtl_header*)message;
  igtl_float32* transform = (igtl_float32*)&message[IGTL_HEADER_SIZE];
  

  transform[0] = -0.954892;
  transform[1] = 0.196632;
  transform[2] = -0.222525;

  transform[3] = -0.196632;
  transform[4] = 0.142857;
  transform[5] = 0.970014;

  transform[6] = 0.222525;
  transform[7] = 0.970014;
  transform[8] = -0.0977491;

  transform[9] = 46.0531;
  transform[10] = 19.4709;
  transform[11] = 46.0531;

  igtl_transform_convert_byte_order(transform);

  header->version = 1;
  strncpy( header->name, "TRANSFORM", 12 );
  strncpy( header->device_name, "DeviceName", 20 );
  header->timestamp = 1234567890;
  header->body_size = IGTL_TRANSFORM_SIZE;
  header->crc = igtl_transform_get_crc(transform);
  igtl_header_convert_byte_order( header );

  FILE *fp;
  fp = fopen("transform.bin", "w");
  fwrite(header, IGTL_HEADER_SIZE+IGTL_TRANSFORM_SIZE, 1, fp);
  fclose(fp);

  int r = memcmp((const void*)message, (const void*)barray, IGTL_HEADER_SIZE+IGTL_TRANSFORM_SIZE);
  free(message);

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    return EXIT_FAILURE;
    }

}





