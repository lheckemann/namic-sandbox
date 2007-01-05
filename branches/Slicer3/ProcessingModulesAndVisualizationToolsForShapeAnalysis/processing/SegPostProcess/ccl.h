// Header file foe ccl.c
// Connected component labeling in 3D

#ifndef _CCL_C_H__
#define _CCL_C_H__

#ifdef __cplusplus
extern "C" {
#endif


int DoConnectedComponentLabeling(unsigned char * image, int *dim, int metric) ;
// calls the connected component labeling code
// modifies image (result is stored in image)
// metric is either 6,18,or 26

#ifdef __cplusplus
}
#endif

#endif
