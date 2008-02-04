#include "igtl_util.h"
#include "igtl_header.h"

void igtl_header_convert_byte_order(struct igtl_header* header)
{
  if (igtl_is_little_endian()) {
    header->version   = BYTE_SWAP_INT16(header->version);
    header->timestamp = BYTE_SWAP_INT64(header->timestamp);
    header->body_size = BYTE_SWAP_INT64(header->body_size);
    header->crc       = BYTE_SWAP_INT64(header->crc);
  }  
}

