#include "igtl_util.h"

int igtl_is_little_endian()
{
  short a = 1; return ((char*)&a)[0];
}
