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


#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_ndarray.h"
#include "igtl_util.h"
#include <string.h>
#include <stdio.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* Include serialized test data (gold standard) */
#include "igtl_test_data_ndarray.h"

#pragma pack(1)

/*
 * Structure for byte array of test message body.
 *
 */

typedef struct {
  igtl_ndarray_header ndarray_header;
  double              matrix3x4x5[3][4][5];
} test_ndarray_message;

#pragma pack()


void generate_data(test_ndarray_message * msg)
{
  int i, j, k;

  msg->ndarray_header.type = IGTL_NDARRAY_STYPE_TYPE_FLOAT64;
  for (i = 0; i < 3; i ++)
    {
    for (j = 0; j < 4; j ++)
      {
      for (k = 0; k < 5; k ++)
        {
        msg->ndarray_header.
        }
      }
    }
    
  
  igtl_transform_convert_byte_order(body->transform);
}




int main( int argc, char * argv [] )
{
  /* Message structures and byte array */
  igtl_header header;
  void* bind_header;
  child_message_body child_body;
  
  igtl_bind_info bind_info;
  size_t bind_size;
  size_t child_body_size;
  int rh; /* Comparison result for header */
  int rb; /* Comparison result for BIND header section*/
  int rc; /* Comparison result for child body section */

  int s;

  igtl_bind_init_info(&bind_info);
  
  /* Generate transform message */
  generate_transform_body(&(child_body.transform));

  /* Generate image message */
  generate_image_body(&(child_body.image));

  /* Generate sensor message */
  generate_sensor_body(&(child_body.sensor));

  /* Set up BIND info structure */
  if (igtl_bind_alloc_info(&bind_info, 3) == 0)
    {
    return EXIT_FAILURE;
    }

  strncpy(bind_info.child_info_array[0].type, "TRANSFORM", IGTL_HEADER_TYPE_SIZE);
  strncpy(bind_info.child_info_array[0].name, "ChildTrans", IGTL_HEADER_NAME_SIZE);
  bind_info.child_info_array[0].size = sizeof(transform_message_body);
  bind_info.child_info_array[0].ptr = (void*)&child_body.transform;

  strncpy(bind_info.child_info_array[1].type, "IMAGE", IGTL_HEADER_TYPE_SIZE);
  strncpy(bind_info.child_info_array[1].name, "ChildImage", IGTL_HEADER_NAME_SIZE);
  bind_info.child_info_array[1].size = sizeof(image_message_body);
  bind_info.child_info_array[1].ptr = (void*)&child_body.image;

  strncpy(bind_info.child_info_array[2].type, "SENSOR", IGTL_HEADER_TYPE_SIZE);
  strncpy(bind_info.child_info_array[2].name, "ChildSensor", IGTL_HEADER_NAME_SIZE);
  bind_info.child_info_array[2].size = sizeof(sensor_message_body);
  bind_info.child_info_array[2].ptr = (void*)&child_body.sensor;

  bind_size = igtl_bind_get_size(&bind_info, IGTL_TYPE_PREFIX_NONE);
  bind_header = malloc(bind_size);


  
  if (bind_header == NULL)
    {
    igtl_bind_free_info(&bind_info);
    return EXIT_FAILURE;
    }

  igtl_bind_pack(&bind_info, bind_header, IGTL_TYPE_PREFIX_NONE);

  /* Calculate the sum of child body size and paddings (0 in this test program) */
  child_body_size = 
    bind_info.child_info_array[0].size +
    bind_info.child_info_array[1].size +
    bind_info.child_info_array[2].size;

  /* Set header */
  header.version = 1;
  strncpy( (char*)&(header.name), "BIND", 12 );
  strncpy( (char*)&(header.device_name), "DeviceName", 20 );
  header.timestamp = 1234567890;
  header.body_size = bind_size + sizeof(child_message_body);
  header.crc = igtl_bind_get_crc(&bind_info, IGTL_TYPE_PREFIX_NONE, bind_header);
  igtl_header_convert_byte_order( &(header) );


  /* Dumping data -- for testing */
  /*
  FILE *fp;
  fp = fopen("bind.bin", "w");
  fwrite(&(header), IGTL_HEADER_SIZE, 1, fp);
  fwrite(bind_header, bind_size, 1, fp);
  fwrite(&(child_body), child_body_size, 1, fp);
  fclose(fp);
  */


  /* Compare the serialized byte array with the gold standard */ 
  rh = memcmp((const void*)&header, (const void*)test_bind_message_header, IGTL_HEADER_SIZE);
  rb = memcmp((const void*)bind_header, (const void*)test_bind_message_bind_header, bind_size);
  rc = memcmp((const void*)&child_body, (const void*)test_bind_message_bind_body, child_body_size);

  igtl_bind_free_info(&bind_info);
  free(bind_header);

  if (rh == 0 && rb == 0 && rc == 0)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    /* Print first 256 bytes as HEX values in STDERR for debug */
    s = IGTL_HEADER_SIZE+bind_size+child_body_size;
    if (s > 256)
      {
      s = 256;
      }

    fprintf(stdout, "\n===== First %d bytes of the test message =====\n", s);
    igtl_message_dump_hex(stdout, (const void*)&header, s);

    return EXIT_FAILURE;
    }

}






