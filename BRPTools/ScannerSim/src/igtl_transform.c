#include <string.h>
#include "igtl_transform.h"
#include "igtl_util.h"


void igtl_transform_convert_byte_order(float* transform)
{
  int i = 0;
  long tmp[12];

  if (igtl_is_little_endian()) {
    memcpy((void*)tmp, (void*)transform, sizeof(float)*12);
    for (i = 0; i < 12; i ++) {
      tmp[i] = BYTE_SWAP_INT32(tmp[i]);
    }
    memcpy((void*)transform, (void*)tmp, sizeof(float)*12);
  }
}


