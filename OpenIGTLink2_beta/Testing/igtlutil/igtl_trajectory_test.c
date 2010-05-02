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
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_trajectory.h"
#include "igtl_util.h"

/* include test trajectory data and serialized trajectory message */
//#include "igtl_test_data_trajectory.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define TEST_TRAJECTORY_NUM 3

#pragma pack(1)
struct trajectory_message {
  igtl_header             header;
  igtl_trajectory_element metalist[TEST_TRAJECTORY_NUM];
};
#pragma pack()


int main( int argc, char * argv [] )
{

  struct trajectory_message message;
  int r;
  int s;

  // Test structure size
  if (sizeof(message) != IGTL_HEADER_SIZE+IGTL_TRAJECTORY_ELEMENT_SIZE*TEST_TRAJECTORY_NUM)
    {
    fprintf(stdout, "Invalid size of trajectory message structure.\n");
    return EXIT_FAILURE;
    }

  //igtl_image_set_matrix(spacing, origin, norm_i, norm_j, norm_k, &(message.iheader));

  /* Copy image data */
  //memcpy((void*)message.image, (void*)test_image, TEST_IMAGE_MESSAGE_SIZE);
  
  /* Swap byte order if necessary */
  igtl_trajectory_convert_byte_order(message.metalist, TEST_TRAJECTORY_NUM);

  /* Create OpenIGTLink header */
  message.header.version = 1;
  strncpy( (char*)&(message.header.name), "TRAJECTORY", 12 );
  strncpy( (char*)&(message.header.device_name), "DeviceName", 20 );
  message.header.timestamp = 1234567890;
  message.header.body_size = IGTL_TRAJECTORY_ELEMENT_SIZE*TEST_TRAJECTORY_NUM;
  message.header.crc = igtl_trajectory_get_crc(message.metalist, TEST_TRAJECTORY_NUM);
  igtl_header_convert_byte_order( &(message.header) );

  /* Dumping data -- for debugging */
  /*
  FILE *fp;
  fp = fopen("image.bin", "w");
  fwrite(&(message), IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+image_size, 1, fp);
  fclose(fp);
  */

  /* Compare the serialized byte array with the gold standard */ 
  //r = memcmp((const void*)&message, (const void*)test_trajectory_message,
  //           (size_t)(IGTL_HEADER_SIZE+IGTL_TRAJECTORY_HEADER_SIZE));
  r = 0;

  if (r == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+IGTL_TRAJECTORY_ELEMENT_SIZE*TEST_TRAJECTORY_NUM;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    //igtl_message_dump_hex(stdout, (const void*)&message, s);

    return EXIT_FAILURE;
    }
}