/* 
   FAST SWEEPING ALGORITHM
   to build, do
   >> mex -setup
   >> clear mex
   >> mex -g -o fast_sweeping fast_sweeping.c main_mex.c -lm

*/

#include <mex.h>
#include <math.h>
#include "fast_sweeping.h"

#define FS_IN prhs[0]
#define MASK_IN prhs[1]
#define GRADS_IN prhs[2]
#define SEED_IN prhs[3]
#define EPS_IN prhs[4]

#define TSTAR_OUT plhs[0]
#define LSTAR_OUT plhs[1]
#define DSTAR_OUT plhs[2]

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    printf("Starting the MEX code!\n");
    /* check usage method */ 
    int check_usage(void)
        {
            int usage(const char *msg)
                {
                    printf("usage: main_mex(localCosts, mask, grads, start_pt, epsilon)\n"
                           "  %s\n", msg);
                    return 0;
                }

            /* number of parameters */
            if (nrhs != 5)
                return usage("wrong number of input parameters");
            if (nlhs != 3)
                return usage("wrong number of output parameters");

            return 1; /* passed */
        }

    /*-- check command arguments --*/
    if (check_usage() == 0)
        return;

    /*-- input --*/
    const int *fs_dims = mxGetDimensions( FS_IN );
    const int *mask_dims = mxGetDimensions( MASK_IN );
    const int *grads_dims = mxGetDimensions( GRADS_IN );
    printf("  The fs dimensions are: %d %d %d %d!\n", fs_dims[0], fs_dims[1], fs_dims[2], fs_dims[3]);
    printf("  The mask dimensions are: %d %d %d!\n", mask_dims[0], mask_dims[1], mask_dims[2]);
    printf("  The gradient dimensions are: %d %d!\n", grads_dims[0], grads_dims[1]);

    double *fs = mxGetPr( FS_IN );
    double *mask = mxGetPr( MASK_IN );
    double *grads = mxGetPr( GRADS_IN );
    double *seed = mxGetPr( SEED_IN );
    double *eps = mxGetPr ( EPS_IN );

    /*-- output --*/
    /* extend borders by one pixel on all sides */
    const int tstar_dims_cnt = 3;
    const int tstar_dims[] = { fs_dims[0]+2, fs_dims[1]+2, fs_dims[2]+2 };
    const int dstar_dims_cnt = 4;
    const int dstar_dims[] = { fs_dims[0]+2, fs_dims[1]+2, fs_dims[2]+2, 3 };
    TSTAR_OUT = mxCreateNumericArray(tstar_dims_cnt, tstar_dims, mxDOUBLE_CLASS, mxREAL);
    LSTAR_OUT = mxCreateNumericArray(tstar_dims_cnt, tstar_dims, mxDOUBLE_CLASS, mxREAL);
    DSTAR_OUT = mxCreateNumericArray(dstar_dims_cnt, dstar_dims, mxDOUBLE_CLASS, mxREAL);
    double *tstar = mxGetPr( TSTAR_OUT );
    double *lstar = mxGetPr( LSTAR_OUT );
    double *dstar = mxGetPr( DSTAR_OUT );

    /*-- initialize tstar --*/
    double max_possible_fs = (double)floor(exp(3));
    double number_of_voxels =   (double)(fs_dims[0]*fs_dims[1]*fs_dims[2]);
    double INF = max_possible_fs*number_of_voxels; /* assumes that no arrival time will be greater than this */

    printf("  Floor(exp(3)) = %f\n", (double)floor(exp(3)));
    printf("  Total dimensions = %f\n", number_of_voxels);
    printf("  The value of INF (in main_mex.c) is: %f\n", INF);
    int i;
    for (i = 0; i < tstar_dims[0]*tstar_dims[1]*tstar_dims[2]; i++)
        {
            tstar[i] = INF;
            lstar[i] = INF;
        }

    /*-- functionality --*/
    fast_sweeping( fs, fs_dims, mask, mask_dims, grads, grads_dims, tstar, lstar, dstar, seed, eps );
    printf("Ending the MEX code!\n");
}
