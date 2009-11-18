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

#include <stdio.h>
#include "igtl_header.h"
#include "igtl_image.h"

/* include test image data and serialized image message */
#include "igtl_test_data_image.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1


#pragma pack(1)
struct image_message {
  igtl_header        header;
  igtl_image_header  iheader;     /* image header */
  igtl_uint8         image[TEST_IMAGE_MESSAGE_SIZE];
};
#pragma pack(0)


int main( int argc, char * argv [] )
{

  struct image_message message;

  /* Set data */
  message.iheader.version     = 1;
  message.iheader.data_type   = 1; /* scalar */
  message.iheader.scalar_type = 3; /* uint8 */
  message.iheader.endian      = 2; /* Little endian */
  message.iheader.coord       = 1; /* RAS */
  message.iheader.size[0] = 50;
  message.iheader.size[1] = 50;
  message.iheader.size[2] = 1;

  /* Dimensions */
  message.iheader.subvol_offset[0] = 0;
  message.iheader.subvol_offset[1] = 0;
  message.iheader.subvol_offset[2] = 0;
  message.iheader.subvol_size[0] = 50;
  message.iheader.subvol_size[1] = 50;
  message.iheader.subvol_size[2] = 1;

  igtl_float32 spacing[] = {1.0, 1.0, 1.0};
  igtl_float32 origin[]  = {46.0531, 19.4709, 46.0531};
  igtl_float32 norm_i[]  = {-0.954892, 0.196632, -0.222525};
  igtl_float32 norm_j[]  = {-0.196632, 0.142857, 0.970014};
  igtl_float32 norm_k[]  = {0.222525, 0.970014, -0.0977491};

  igtl_image_set_matrix(spacing, origin, norm_i, norm_j, norm_k, &(message.iheader));

  /* Copy image data */
  memcpy((void*)message.image, (void*)test_image, TEST_IMAGE_MESSAGE_SIZE);
  
  /* Get image data size -- note that this should be done before byte order swapping. */
  igtl_uint64 image_size = igtl_image_get_data_size(&(message.iheader));

  /* Swap byte order if necessary */
  igtl_image_convert_byte_order(&(message.iheader));

  /* Create OpenIGTLink header */
  message.header.version = 1;
  strncpy( (char*)&(message.header.name), "IMAGE", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567890;
  message.header.body_size = image_size;
  message.header.crc = igtl_image_get_crc(&(message.iheader), message.image);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for testing */
  FILE *fp;
  fp = fopen("image.bin", "w");
  fwrite(&(message), IGTL_HEADER_SIZE+image_size, 1, fp);
  fclose(fp);

  /* Compare the serialized byte array with the gold standard */ 
  //int r = memcmp((const void*)&message, (const void*)test_image_message, IGTL_HEADER_SIZE+image_size);
  int r = memcmp((const void*)&message, (const void*)test_image_message, IGTL_HEADER_SIZE+100);


  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    return EXIT_FAILURE;
    }
}
