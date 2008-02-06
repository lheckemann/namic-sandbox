#ifndef __IGTL_HEADER_H
#define __IGTL_HEADER_H

#define IGTL_HEADER_VERSION   1
#define IGTL_HEADER_SIZE      58

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

struct igtl_header {
  unsigned short version;
  char           name[8];
  char           device_name[20];
  long long      timestamp;
  long long      body_size;
  long long      crc;
};

#pragma pack(0)

void igtl_header_convert_byte_order(struct igtl_header* header);

#ifdef __cplusplus
}
#endif
#endif __IGTL_HEADER_H


