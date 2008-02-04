#ifndef __IGTL_UTIL_H
#define __IGTL_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif


#define BYTE_SWAP_INT16(S) (((S) & 0xFF) << 8 \
                            | (((S) >> 8) & 0xFF))
#define BYTE_SWAP_INT32(L) ((BYTE_SWAP_INT16 ((L) & 0xFFFF) << 16) \
                            | BYTE_SWAP_INT16 (((L) >> 16) & 0xFFFF))
#define BYTE_SWAP_INT64(LL) ((BYTE_SWAP_INT32 ((LL) & 0xFFFFFFFF) << 32) \
                             | BYTE_SWAP_INT32 (((LL) >> 32) & 0xFFFFFFFF))

int igtl_is_little_endian();

#ifdef __cplusplus
}
#endif

#endif  /*__IGTL_UTIL_H*/



