#include <mex.h>
#include <math.h>
#include "fast_sweeping.h"

void fast_sweeping(const double *fs, const int *fs_dims,        /* fs */
                   const double *mask, const int *mask_dims,    /* mask */
                   const double *grads, const int *grads_dims,  /* grads */
                   double *tstar, double *lstar, double *dstar, /* outputs */
                   const double *seed,                          /* seed */
                   double *eps)                                 /* convergence parameter */
{
    /* assume zero indexing */
    /* (i.e. the pixel at (0,0,0) in fs and mask corresponds to the pixel at (1,1,1) in tstar, lstar, and dstar) */

    /* get method for fs */
    /* size(fs) = (dimY,dimX,nSlices,nDir) */
    double get_fs(const int row, const int col, const int slice, const int dir) {
        return fs[fs_dims[0] * fs_dims[1] * fs_dims[2] * dir /* now in correct direction matrix */
                  + fs_dims[0] * fs_dims[1] * slice          /* now in correct slice */
                  + fs_dims[0] * col                         /* now in correct column */
                  + row];                                    /* now in correct row */
    }

    /* get method for mask */
    /* size(mask) = (dimY,dimX,nSlices) */
    double get_mask(const int row, const int col, const int slice) {
        return mask[fs_dims[0] * fs_dims[1] * slice          /* now in correct slice */
                    + fs_dims[0] * col                       /* now in correct column */
                    + row];                                  /* now in correct row */
    }

    const int tstar_dims[] = { fs_dims[0]+2, fs_dims[1]+2, fs_dims[2]+2 };
    /* get and set methods for tstar */
    /* size(tstar) = (dimY+2,dimX+2,nSlices+2) */
    double get_tstar(const int row, const int col, const int slice) {
        return tstar[tstar_dims[0] * tstar_dims[1] * slice   /* now in correct slice */
                     + tstar_dims[0] * col                   /* now in correct column */
                     + row];                                 /* now in correct row */
    }
    void set_tstar(const int row, const int col, const int slice, const double val) {
        tstar[tstar_dims[0] * tstar_dims[1] * slice          /* now in correct slice */
              + tstar_dims[0] * col                          /* now in correct column */
              + row]                                         /* now in correct row */
            = val;
    }

    /* get and set methods for lstar */
    /* size(lstar) = size(tstar) */
    double get_lstar(const int row, const int col, const int slice) {
        return lstar[tstar_dims[0] * tstar_dims[1] * slice   /* now in correct slice */
                     + tstar_dims[0] * col                   /* now in correct column */
                     + row];                                 /* now in correct row */
    }
    void set_lstar(const int row, const int col, const int slice, const double val) {
        lstar[tstar_dims[0] * tstar_dims[1] * slice          /* now in correct slice */
              + tstar_dims[0] * col                          /* now in correct column */
              + row]                                         /* now in correct row */
            = val;
    }

    const int dstar_dims[] = { fs_dims[0]+2, fs_dims[1]+2, fs_dims[2]+2, 3 };
    /* get and set methods for dstar */
    /* size(dstar) = (dimY+2,dimX+2,nSlices+2,3) */
    double get_dstar(const int row, const int col, const int slice, const int component) {
        return dstar[dstar_dims[0] * dstar_dims[1] * dstar_dims[2] * component /* now in correct component */
                     + dstar_dims[0] * dstar_dims[1] * slice                   /* now in correct slice */
                     + dstar_dims[0] * col                                     /* now in correct column */
                     + row];                                                   /* now in correct row */
    }
    void set_dstar(const int row, const int col, const int slice, const int component, const double val) {
        dstar[dstar_dims[0] * dstar_dims[1] * dstar_dims[2] * component /* now in correct component */
              + dstar_dims[0] * dstar_dims[1] * slice                   /* now in correct slice */
              + dstar_dims[0] * col                                     /* now in correct column */
              + row]                                                    /* now in correct row */
            = val;
    }

    /* get method for grads */
    /* size(grads) = (nDir,3) */
    double get_grads(const int row, const int col) {
        return grads[grads_dims[0] * col /* now in correct column */
                     + row];             /* now in correct row */
    }

    /*--------------------------------------------------------------------------------*/
    /* the real processing is done in this section */

    /* initialize the variables */
    double max_possible_fs = (double)floor(exp(3));
    double number_of_voxels =   (double)(fs_dims[0]*fs_dims[1]*fs_dims[2]);
    double INF = max_possible_fs*number_of_voxels; /* assumes that no arrival time will be greater than this */
    int wY, wX, wZ, y, x, z, dir, dir_sign, tstar_count, num_inf_neighbors;
    double fs_pt, x_comp, y_comp, z_comp, abs_x_comp, abs_y_comp, abs_z_comp, denom;
    double tstar_y, tstar_x, tstar_z, tmp, tstar_vect[7];
    double lstar_y, lstar_x, lstar_z, lstar_val;
    double delta_iteration;

    delta_iteration = INF*number_of_voxels; /* initialize the change in arrival times to be as large as possible */
    set_tstar((int)seed[0],(int)seed[1],(int)seed[2],0); /* initialize T* at the seed point to be 0 */
    set_lstar((int)seed[0],(int)seed[1],(int)seed[2],0); /* initialize L* at the seed point to be 0 */

    /* print initializations to screen */
    printf("  Initialization:  Total Number of Voxels = %f\n", number_of_voxels);
    printf("  Initialization:  INF = %f\n", INF);
    printf("  Initialization:  Seed Point Location, (y,x,z) = (%d,%d,%d)\n", (int)seed[0], (int)seed[1], (int)seed[2]);
    printf("  Initialization:  T* at Seed Point = %f\n", get_tstar((int)seed[0],(int)seed[1],(int)seed[2]));
    printf("  Initialization:  Total Change in Arrival Times = %f\n", delta_iteration);
    printf("  Initialization:  Stopping Criterion for Change in Arrival Times = %f\n", eps[0]);

    /* perform the fast sweeping */
    int iteration = 0;
    while( delta_iteration > eps[0] ) {
        iteration++;
        delta_iteration = 0;
        /* these 6 for loops iterate through the 8 sweeping directions */
        for ( wZ = -1; wZ <= 1; wZ += 2 ) {
            for ( wX = -1; wX <= 1; wX += 2 ) {
                for( wY = -1; wY <= 1; wY += 2 ) {
                    for ( z = (wZ < 0 ? fs_dims[2] : 1); (wZ < 0 ? z >= 1 : z <= fs_dims[2]); z += wZ ) {
                        for ( x = (wX < 0 ? fs_dims[1] : 1); (wX < 0 ? x >= 1 : x <= fs_dims[1]); x += wX ) {
                            for ( y = (wY < 0 ? fs_dims[0] : 1); (wY < 0 ? y >= 1 : y <= fs_dims[0]); y += wY ) {
                                if( (int)get_mask(y-1,x-1,z-1) == 1 ) {
                                    /* get all voxels in the neighborhood */                                
                                    tstar_vect[0] = get_tstar(y-1,x,z);
                                    tstar_vect[1] = get_tstar(y+1,x,z);
                                    tstar_vect[2] = get_tstar(y,x-1,z);
                                    tstar_vect[3] = get_tstar(y,x+1,z);
                                    tstar_vect[4] = get_tstar(y,x,z-1);
                                    tstar_vect[5] = get_tstar(y,x,z+1);
                                    tstar_vect[6] = get_tstar(y,x,z);

                                    /* iterate through all positive directions */
                                    for ( dir_sign = -1; dir_sign <= 1; dir_sign += 2 ) {
                                        for ( dir = 0; dir < fs_dims[3]; dir += 1 ) {
                                            /* get the local cost and direction for this position and direction */
                                            fs_pt = get_fs(y-1,x-1,z-1,dir); /* -1 accounts for the border extension stuff */
                                            y_comp = ((double)dir_sign)*get_grads(dir,0);
                                            x_comp = ((double)dir_sign)*get_grads(dir,1);
                                            z_comp = ((double)dir_sign)*get_grads(dir,2);

                                            /* choose the neighbors to use */
                                            if ( y_comp > 0 ) {
                                                tstar_y = tstar_vect[1];
                                                lstar_y = get_lstar(y+1,x,z);
                                            }
                                            else {
                                                tstar_y = tstar_vect[0];
                                                lstar_y = get_lstar(y-1,x,z);
                                            }
                                            if ( x_comp > 0 ) {
                                                tstar_x = tstar_vect[3];
                                                lstar_x = get_lstar(y,x+1,z);
                                            }
                                            else {
                                                tstar_x = tstar_vect[2];
                                                lstar_x = get_lstar(y,x-1,z);
                                            }
                                            if ( z_comp > 0 ) {
                                                tstar_z = tstar_vect[5];
                                                lstar_x = get_lstar(y,x,z+1);
                                            }
                                            else {
                                                tstar_z = tstar_vect[4];
                                                lstar_x = get_lstar(y,x,z-1);
                                            }

                                            /* take absolute value of directional components*/
                                            abs_y_comp = abs_double(y_comp);
                                            abs_x_comp = abs_double(x_comp);
                                            abs_z_comp = abs_double(z_comp);

                                            /* check if the tstar values are inf values */
                                            /* don't use tstar values if they are inf */
                                            num_inf_neighbors = 0;
                                            if( tstar_y >= INF ) {
                                                abs_y_comp = 0;
                                                num_inf_neighbors = num_inf_neighbors + 1;
                                            }
                                            if( tstar_x >= INF ) {
                                                abs_x_comp = 0;
                                                num_inf_neighbors = num_inf_neighbors + 1;
                                            }
                                            if( tstar_z >= INF ) {
                                                abs_z_comp = 0;
                                                num_inf_neighbors = num_inf_neighbors + 1;
                                            }

                                            /* compute the new arrival time */
                                            if( num_inf_neighbors < 3 ) {
                                                /* renormalize the directional weights */
                                                if( num_inf_neighbors > 0 ) {
                                                    denom = sqrt( y_comp * y_comp +
                                                                  x_comp * x_comp +
                                                                  z_comp * z_comp );
                                                    y_comp = y_comp / denom;
                                                    x_comp = x_comp / denom;
                                                    z_comp = z_comp / denom;
                                                    abs_y_comp = abs_double(y_comp);
                                                    abs_x_comp = abs_double(x_comp);
                                                    abs_z_comp = abs_double(z_comp);
                                                }

                                                /* compute the new arrival time */
                                                tmp = (tstar_y * abs_y_comp + tstar_x * abs_x_comp
                                                       + tstar_z * abs_z_comp + fs_pt)
                                                    / (abs_y_comp + abs_x_comp + abs_z_comp);

                                                /* update if new arrival time is lower than the old time */
                                                if ( tmp < get_tstar(y,x,z) ) {
                                                    if( tmp < 0 )
                                                        printf("ERROR: Negative arrival times were computed!\n");

                                                    /* compute the new euclidean length */
                                                    lstar_val = (lstar_y * abs_y_comp + lstar_x * abs_x_comp
                                                                 + lstar_z * abs_z_comp + 1)
                                                        / (abs_y_comp + abs_x_comp + abs_z_comp);

                                                    /* write results to the data structures */
                                                    set_tstar(y,x,z,tmp);
                                                    set_lstar(y,x,z,lstar_val);
                                                    set_dstar(y,x,z,0,y_comp);
                                                    set_dstar(y,x,z,1,x_comp);
                                                    set_dstar(y,x,z,2,z_comp);

                                                    /* update the convergence parameter */
                                                    delta_iteration = delta_iteration + tstar_vect[6]-tmp;
                                                }
                                            }
                                        } /* End dir for loop */
                                    } /* End dir_sign for loop */
                                }
                            }
                        }
                    }
                }
            }
        }
        printf("  For Iteration %d, Total Change in Arrival Times = %f\n", iteration, delta_iteration);
        fflush(stdout);
    } /* end while loop */
} /* end fast_sweeping function */


double abs_double( double x )
{
    return (x < 0 ? ((double)-1 * x ) : x);
}
